

#include <iostream>
#include <thread>
#include <mutex>

#include <gtest/gtest.h>

#include <pcosynchro/pcologger.h>

#include "pcobarrier.h"

class Checker
{
public:
    Checker(int nbThreads) : nbThreads(nbThreads) {}

    void arriving(int id)
    {
        mutex.lock();
        EXPECT_LT(id, nbThreads);
        nbArrived ++;
        EXPECT_LE(nbArrived, nbThreads);
        mutex.unlock();
    }

    void leaving(int id)
    {
        mutex.lock();
        EXPECT_EQ(nbArrived, nbThreads);
        nbLeaved ++;
        EXPECT_LE(nbLeaved, nbThreads);
        mutex.unlock();
    }

private:
    std::mutex mutex;
    int nbThreads;
    int nbArrived{0};
    int nbLeaved{0};
};

class PcoBarrierTest : public ::testing::Test
{

public:

    static const int NB_THREADS;

    static void task(PcoBarrier *barrier, Checker *checker, int id) {
        checker->arriving(id);
        logger() << "Thread " << id << " arriving" << std::endl;
        barrier->wait();
        checker->leaving(id);
        logger() << "Thread " << id << " leaving" << std::endl;
    }

    void testPcoBarrierTest(int nbThreads, PcoBarrier *barrier)
    {
        auto checker = std::make_unique<Checker>(nbThreads);
        std::vector<std::unique_ptr<std::thread> > threads(nbThreads);
        for(int i = 0; i < nbThreads; i++) {
            threads[i] = std::make_unique<std::thread>(task, barrier, checker.get(), i);
        }
        for(int i = 0; i < nbThreads; i++)
            threads[i]->join();
    }
};

const int PcoBarrierTest::NB_THREADS = 4;

TEST_F(PcoBarrierTest, Semaphore) {
    auto barrier = std::make_unique<PcoBarrier>(NB_THREADS);
    testPcoBarrierTest(NB_THREADS, barrier.get());
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    logger().initialize(argc, argv);
    return RUN_ALL_TESTS();
}
