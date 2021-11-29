

/******************************************************************************
Fichier: main.cpp
Auteur:  Yann Thoma
Date:    04.04.17.
Description: Gestion d'un pont pouvant supporter un certain poids.
    Les véhicules peuvent avoir un poids quelconque, en virgule flottante.
    L'accès au pont se fait sans dépassement, et le protocole doit assurer que
    la charge maximale n'est jamais dépassée.
    Ce fichier contient un checker et le lancement des threads.

Modification:
    08.01.2020. Utilisation de la librairie pcosynchro et de gtest
******************************************************************************/


#include <iostream>
#include <gtest/gtest.h>
#include <random>

#include <pcosynchro/pcologger.h>
#include <pcosynchro/pcothread.h>
#include <pcosynchro/pcomutex.h>

#include "bridgemanagerfloat.h"
#include "vehicle.h"



class BridgeFloatChecker
{
private:
    float currentWeight{0};
    float maxWeight{0};

    std::mutex mutex;


    void check()
    {
        ASSERT_LE(totalWeight(), maxWeight) << "Too many vehicles on the bridge";
        ASSERT_GE(totalWeight(), 0) << "Error. Less than 0 kilos on the bridge";
    }


public:

    BridgeFloatChecker(float maxWeight) : maxWeight(maxWeight) {}

    float totalWeight() {
        float result;
        mutex.lock();
        result = currentWeight;
        mutex.unlock();
        return result;
    }

    void addVehicle(float weight)
    {
        mutex.lock();
        currentWeight += weight;
        mutex.unlock();
        check();
    }

    void removeVehicle(float weight)
    {
        mutex.lock();
        currentWeight -= weight;
        mutex.unlock();
        check();
    }
};

void vehicleTask(BridgeFloatChecker *checker, BridgeManagerFloat *manager, Vehicle *vehicle, int nbAccess)
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(1, 1000);

//    logger() << "Poids du véhicule : " << vehicle->getWeight() << std::endl;

    for(int i = 0; i < nbAccess; i++) {
        manager->access(vehicle->getWeight());
        checker->addVehicle(vehicle->getWeight());
//        logger() << "Tache " << tid << ": Voiture sur le pont" << std::endl;
//        logger() << "Poids: " << checker->totalWeight() << std::endl;
        PcoThread::usleep(static_cast<uint64_t>(6.0 * dis(gen)));
        checker->removeVehicle(vehicle->getWeight());
        manager->leave(vehicle->getWeight());
        PcoThread::usleep(static_cast<uint64_t>(1.0 * dis(gen)));
    }

}


void test(BridgeManagerFloat *manager, float maxWeight, int nbVehicles)
{
    BridgeFloatChecker checker(maxWeight);
    std::vector<std::unique_ptr<Vehicle> > vehicles(nbVehicles);
    std::vector<std::unique_ptr<PcoThread> > threadsVoiture(nbVehicles);
    for(int t = 0; t < nbVehicles; t++){
//        logger() << "Création de la tâche voiture " << t << std::endl;
        vehicles[t] = std::make_unique<Vehicle>(t);
        threadsVoiture[t] = std::make_unique<PcoThread>(vehicleTask, &checker, manager, vehicles[t].get(), 3);
    }

    for(int t = 0; t < nbVehicles; t++){
        threadsVoiture[t]->join();
    }
    float totalWeight = checker.totalWeight();
    ASSERT_EQ(totalWeight, 0);
}

TEST(BridgeFloat, standard) {

    PcoManager::getInstance()->setMaxSleepDuration(0.0, PcoManager::EventType::SemaphoreRelease);
    PcoManager::getInstance()->setMaxSleepDuration(0.0, PcoManager::EventType::SemaphoreAcquire);
    std::unique_ptr<BridgeManagerFloat> manager;
    float maxWeight = 100.0;
    manager = std::make_unique<BridgeManagerFloat>(maxWeight);
    // 50 vehicules with a max weight on the bridge of 100.0
    test(manager.get(), maxWeight, 50);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    logger().initialize(argc, argv);
    return RUN_ALL_TESTS();
}
