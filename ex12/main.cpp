

/******************************************************************************
Fichier: main.cpp
Auteur:  Yann Thoma
Date:    04.04.17.
Description: Gestion d'un pont pouvant supporter un certain poids.
    Les voitures pèsent 1, et les camions 10. L'accès au pont se fait sans
    dépassement, et le protocole doit assurer que la charge maximale n'est
    jamais dépassée. Ce fichier implémente un checker et le lancement des
    threads.
******************************************************************************/

#include <iostream>
#include <gtest/gtest.h>
#include <random>

#include <pcosynchro/pcologger.h>
#include <pcosynchro/pcothread.h>
#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcotest.h>

#include "bridgemanager.h"


class BridgeChecker
{
private:
    int maxWeight;
    int nbCars;
    int nbTrucks;
    std::recursive_mutex mutex;


    void check()
    {
        ASSERT_LE(totalWeight(), maxWeight) << "Too many vehicles on the bridge";
    }


public:

    BridgeChecker(int maxWeight) : maxWeight(maxWeight), nbCars(0), nbTrucks(0) {}

    int totalWeight() {
        int result;
        mutex.lock();
        result = nbCars + 10 * nbTrucks;
        mutex.unlock();
        return result;
    }

    void addCar()
    {
        mutex.lock();
        nbCars ++;
        check();
        mutex.unlock();
    }

    void addTruck()
    {
        mutex.lock();
        nbTrucks ++;
        check();
        mutex.unlock();
    }

    void removeCar()
    {
        mutex.lock();
        nbCars --;
        check();
        mutex.unlock();
    }

    void removeTruck()
    {
        mutex.lock();
        nbTrucks --;
        check();
        mutex.unlock();
    }

};




void carTask(BridgeChecker *checker, BridgeManager *manager,int tid, int nbAccess)
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(1, 1000);

    for (int i = 0; i < nbAccess; i++) {
        manager->carAccess();
        checker->addCar();
        logger() << "Tache " << tid << ": Voiture sur le pont" << std::endl;
        logger() << "Poids: " << checker->totalWeight() << std::endl;
        PcoThread::usleep(static_cast<uint64_t>(6.0 * dis(gen)));
        checker->removeCar();
        manager->carLeave();
        PcoThread::usleep(static_cast<uint64_t>(1.0 * dis(gen)));
    }

}

void truckTask(BridgeChecker *checker, BridgeManager *manager,int tid, int nbAccess)
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(1, 1000);

    for (int i = 0; i < nbAccess; i++) {
        manager->truckAccess();
        checker->addTruck();
        logger() << "Tache " << tid << ": Camion sur le pont" << std::endl;
        logger() << "Poids: " << checker->totalWeight() << std::endl;
        PcoThread::usleep(static_cast<uint64_t>(6.0 * dis(gen)));
        checker->removeTruck();
        manager->truckLeave();
        PcoThread::usleep(static_cast<uint64_t>(1.0 * dis(gen)));
    }
}


void test(BridgeManager *manager,int maxWeight, int nbCars, int nbTrucks, int nbAccess)
{
    BridgeChecker checker(maxWeight);
    std::vector<std::unique_ptr<PcoThread>> threadsVoiture(nbCars);
    std::vector<std::unique_ptr<PcoThread>> threadsCamion(nbTrucks);

    for(int t = 0; t < nbCars; t++){
        logger() << "Création de la tâche voiture " << t << std::endl;
        threadsVoiture[t] = std::make_unique<PcoThread>(carTask, &checker, manager, t, nbAccess);
    }
    for(int t = 0; t < nbTrucks; t++){
        logger() << "Création de la tâche camion " << t << std::endl;
        threadsCamion[t] = std::make_unique<PcoThread>(truckTask, &checker, manager,t, nbAccess);
    }

    for(int t = 0; t < nbCars; t++){
        threadsVoiture[t]->join();
    }
    for(int t = 0; t < nbTrucks; t++){
        threadsCamion[t]->join();
    }
    ASSERT_EQ(checker.totalWeight(), 0);
}


TEST(BridgeSimple, standard) {
    const int POIDS_MAX = 100;
    const int NB_THREADS_VOITURE = 50;
    const int NB_THREADS_CAMION = 10;
    const int NB_ACCESS = 10;
    ASSERT_DURATION_LE(5, {
                           std::unique_ptr<BridgeManager> manager;
                           int maxWeight = POIDS_MAX;
                           manager = std::make_unique<BridgeManager>(maxWeight);
                           // 50 vehicules with a max weight on the bridge of 100.0
                           test(manager.get(), maxWeight, NB_THREADS_VOITURE, NB_THREADS_CAMION, NB_ACCESS);
                       });
}



TEST(BridgeSimple, onlyTrucks) {
    // This test could block in case we use a single semaphore acquired several times without protection
    const int POIDS_MAX = 100;
    const int NB_THREADS_VOITURE = 0;
    const int NB_THREADS_CAMION = 100;
    const int NB_ACCESS = 1;
    ASSERT_DURATION_LE(15, {
                           PcoManager::getInstance()->setMaxSleepDuration(1000, PcoManager::EventType::SemaphoreAcquire);
                           std::unique_ptr<BridgeManager> manager;
                           int maxWeight = POIDS_MAX;
                           manager = std::make_unique<BridgeManager>(maxWeight);
                           test(manager.get(), maxWeight, NB_THREADS_VOITURE, NB_THREADS_CAMION, NB_ACCESS);
                           PcoManager::getInstance()->setMaxSleepDuration(0, PcoManager::EventType::SemaphoreAcquire);
                       });
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    logger().initialize(argc, argv);
    return RUN_ALL_TESTS();
}
