
#include <iostream>
#include <algorithm>

#include <gtest/gtest.h>

#include <pcosynchro/pcologger.h>
#include <pcosynchro/pcothread.h>
#include <pcosynchro/pcosemaphore.h>

static PcoSemaphore s_t0;
static PcoSemaphore s_t1;
static PcoSemaphore s_t345;
static PcoSemaphore s_t26;


class Checker
{
public:

    std::vector<int> executions;

    void compute(int id)
    {
        static std::mutex mutex;
        mutex.lock();
        executions.push_back(id);
        mutex.unlock();
        logger() << "Task " << id << std::endl;
    }

    bool executedBefore(int id0, int id1)
    {
        auto it0 = find(executions.begin(), executions.end(), id0);
        auto it1 = find(executions.begin(), executions.end(), id1);
        return it0 < it1;
    }

    void check()
    {
        EXPECT_TRUE(executedBefore(0, 1));
        EXPECT_TRUE(executedBefore(0, 2));
        EXPECT_TRUE(executedBefore(1, 3));
        EXPECT_TRUE(executedBefore(1, 4));
        EXPECT_TRUE(executedBefore(1, 5));
        EXPECT_TRUE(executedBefore(3, 6));
        EXPECT_TRUE(executedBefore(4, 6));
        EXPECT_TRUE(executedBefore(5, 6));
        EXPECT_TRUE(executedBefore(2, 7));
        EXPECT_TRUE(executedBefore(6, 7));
    }
};

static Checker checker;

void t0()
{
    checker.compute(0);
    s_t0.release();
    s_t0.release();
}

void t1()
{
    s_t0.acquire();
    checker.compute(1);
    s_t1.release();
    s_t1.release();
    s_t1.release();
}

void t2()
{
    s_t0.acquire();
    checker.compute(2);
    s_t26.release();
}

void t3()
{
    s_t1.acquire();
    checker.compute(3);
    s_t345.release();
}


void t4()
{
    s_t1.acquire();
    checker.compute(4);
    s_t345.release();
}

void t5()
{
    s_t1.acquire();
    checker.compute(5);
    s_t345.release();
}

void t6()
{
    s_t345.acquire();
    s_t345.acquire();
    s_t345.acquire();
    checker.compute(6);
    s_t26.release();
}

void t7()
{
    s_t26.acquire();
    s_t26.acquire();
    checker.compute(7);
}



TEST(Sequentialite, Standard)
{
    PcoManager::getInstance()->setMaxSleepDuration(1000, PcoManager::EventType::ThreadCreation);

    // Il s'agit ici d'un exemple qui n'est pas forcément la solution
    std::vector<std::unique_ptr<PcoThread>> threads(8);
    threads[0] = std::make_unique<PcoThread>(t0);
    threads[1] = std::make_unique<PcoThread>(t1);
    threads[2] = std::make_unique<PcoThread>(t2);
    threads[3] = std::make_unique<PcoThread>(t3);
    threads[4] = std::make_unique<PcoThread>(t4);
    threads[5] = std::make_unique<PcoThread>(t5);
    threads[6] = std::make_unique<PcoThread>(t6);
    threads[7] = std::make_unique<PcoThread>(t7);

    for(int i = 0; i < 8; i++) {
        threads[i]->join();
    }

    checker.check();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    logger().initialize(argc, argv);
    return RUN_ALL_TESTS();
}
