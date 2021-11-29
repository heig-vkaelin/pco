/******************************************************************************
Fichier: exemple.c
Auteur:  Yann Thoma
Date:    05.05.09.
Description: Tampon simple.
    Les producteur doivent attendre qu'un consommateur ait lu
    la donnée avant de pouvoir en remettre une nouvelle.
******************************************************************************/


#include <iostream>
#include <random>

#include <gtest/gtest.h>

#include <pcosynchro/pcologger.h>
#include <pcosynchro/pcotest.h>
#include <pcosynchro/pcosemaphore.h>
#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcothread.h>

#include "buffer2conso.h"

using namespace std;


class Scenario
{
public:
    int getItem()
    {
        mutex.lock();
        int item = currentItem;
        currentItem ++;
        mutex.unlock();
        return item;
    }

private:
    std::mutex mutex;
    int currentItem{0};

};

class Checker
{
public:
    Checker(int _nbProducers, int nbConsumers, int _nbProductionsPerProducer) :
        nbProducers(_nbProducers), nbConsumers(nbConsumers),
        nbProductionsPerProducer(_nbProductionsPerProducer),
        items(_nbProducers * _nbProductionsPerProducer)
    {}

    void produce(int item)
    {
        mutex.lock();
        ASSERT_GE(item, 0);
        ASSERT_LT(item, items.size());
        items[item] += 2;
        logger() << "Item : " << item << " . Nb items : " << items[item] << std::endl;
        EXPECT_EQ(items[item], 2);
        mutex.unlock();
    }

    void consume(int item)
    {
        mutex.lock();
        ASSERT_GE(item, 0);
        ASSERT_LT(item, items.size());
        items[item]--;
        logger() << "Item : " << item << " . Nb items : " << items[item] << std::endl;
        EXPECT_GE(items[item], 0);
        mutex.unlock();
    }

    void finalCheck()
    {
        for (int i = 0; i < nbProducers * nbProductionsPerProducer; i++) {
            EXPECT_EQ(items[i], 0);
        }
    }

    int nbProducers;
    int nbConsumers;
    int nbProductionsPerProducer;
    std::vector<int> items;
    std::mutex mutex;
};

void taskProducer(AbstractBuffer<int> *buffer, Checker *checker, Scenario *scenario, int nbProduce) {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(1, 3000000);

    int item = 0;
    for (int i = 0; i < nbProduce; i++) {
        item = scenario->getItem();
        logger() << "Tache producteur veut déposer: " << item << endl;

        checker->produce(item);
        // produire item
        buffer->put(item);
        logger() << "Tache producteur a déposé: " << item << endl;
        PcoThread::usleep(dis(gen));
    }
}

void taskConsumer(AbstractBuffer<int> *buffer, Checker *checker, int nbConsume) {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(1, 3000000);

    int item;
    for (int i = 0; i < nbConsume; i++) {
        logger() << "                                  Tache consommateur attend" << endl;
        item = buffer->get();
        checker->consume(item);
        logger() << "                                  Tache consommateur a récupéré: " << item << endl;
        PcoThread::usleep(dis(gen));
    }
}

/*
TEST (ProdConsoSimple, Standard) {
    ASSERT_DURATION_LE(20, {
    const int NB_THREADS_CONSUMER = 4;
    const int NB_THREADS_PRODUCER = 1;
    const int NB_PRODUCTIONS_PER_PRODUCER = 6;
    const int NB_CONSUMPTIONS_PER_CONSUMER = 3;
    std::vector<std::unique_ptr<std::thread>> producers(NB_THREADS_PRODUCER);
    std::vector<std::unique_ptr<std::thread>> consumers(NB_THREADS_CONSUMER);

    auto checker = std::make_unique<Checker>(NB_THREADS_PRODUCER, NB_THREADS_CONSUMER, NB_PRODUCTIONS_PER_PRODUCER);

    auto scenario = std::make_unique<Scenario>();

    //auto buffer = std::make_unique<Buffer2ConsoSemaphore<int>>();
    //auto buffer = std::make_unique<Buffer2ConsoSemaphoreGeneral<int>>();
    auto buffer = std::make_unique<Buffer2ConsoSemaphoreGeneral<int>>();
    //auto buffer = std::make_unique<Buffer2ConsoHoare<int>>();

    PcoManager::getInstance()->setMaxSleepDuration(1000, PcoManager::EventType::WaitConditionWait);
    for(int i=0; i<NB_THREADS_PRODUCER; i++) {
        producers[i] = std::make_unique<std::thread>(taskProducer, buffer.get(), checker.get(), scenario.get(), NB_PRODUCTIONS_PER_PRODUCER);
    }
    for(int i=0; i<NB_THREADS_CONSUMER; i++) {
        consumers[i] = std::make_unique<std::thread>(taskConsumer, buffer.get(), checker.get(), NB_CONSUMPTIONS_PER_CONSUMER);
    }
    for(int i=0; i<NB_THREADS_PRODUCER; i++) {
        producers[i]->join();
    }
    for(int i=0; i<NB_THREADS_CONSUMER; i++) {
        consumers[i]->join();
    }
    checker->finalCheck();
                       })
}
*/

template<typename T>
class ProdConsTest : public ::testing::Test
{
protected:

    void test() {
    ASSERT_DURATION_LE(20, {
    const int NB_THREADS_CONSUMER = 4;
    const int NB_THREADS_PRODUCER = 1;
    const int NB_PRODUCTIONS_PER_PRODUCER = 6;
    const int NB_CONSUMPTIONS_PER_CONSUMER = 3;
    std::vector<std::unique_ptr<PcoThread>> producers(NB_THREADS_PRODUCER);
    std::vector<std::unique_ptr<PcoThread>> consumers(NB_THREADS_CONSUMER);

    auto checker = std::make_unique<Checker>(NB_THREADS_PRODUCER, NB_THREADS_CONSUMER, NB_PRODUCTIONS_PER_PRODUCER);

    auto scenario = std::make_unique<Scenario>();

    auto buffer = std::make_unique<T>();

    PcoManager::getInstance()->setMaxSleepDuration(1000, PcoManager::EventType::WaitConditionWait);
    for(int i=0; i<NB_THREADS_PRODUCER; i++) {
        producers[i] = std::make_unique<PcoThread>(taskProducer, buffer.get(), checker.get(), scenario.get(), NB_PRODUCTIONS_PER_PRODUCER);
    }
    for(int i=0; i<NB_THREADS_CONSUMER; i++) {
        consumers[i] = std::make_unique<PcoThread>(taskConsumer, buffer.get(), checker.get(), NB_CONSUMPTIONS_PER_CONSUMER);
    }
    for(int i=0; i<NB_THREADS_PRODUCER; i++) {
        producers[i]->join();
    }
    for(int i=0; i<NB_THREADS_CONSUMER; i++) {
        consumers[i]->join();
    }
    checker->finalCheck();
                       })
}
};

using MyTypes = ::testing::Types</*Buffer2ConsoSemaphore<int>,
                                 Buffer2ConsoSemaphoreGeneral<int>,
                                 Buffer2ConsoMesa<int>,
                                 Buffer2ConsoMesa2<int>,*/
                                 Buffer2ConsoHoare<int>
                                    >;
TYPED_TEST_CASE(ProdConsTest, MyTypes);

TYPED_TEST(ProdConsTest, Standard) {
    this->test();
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    logger().initialize(argc, argv);
    return RUN_ALL_TESTS();
}
