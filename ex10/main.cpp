
#include <iostream>

#include <gtest/gtest.h>

#include <pcosynchro/pcologger.h>
#include <pcosynchro/pcothread.h>

#include "mutexfromsem.h"

class MutexFromSemTest : public ::testing::Test
{
public:
    int global{0};
    const int nbIterations = 1000000;
    MutexFromSem mutex;

    void runTask()
    {
        for(int i=0;i<nbIterations;i++) {
            mutex.lock();
            global = global + 1;
            mutex.unlock();
        }
    }
};

TEST_F(MutexFromSemTest, easy) {
    global = 0;
    std::vector<std::unique_ptr<PcoThread>> threads(2);
    for(int i = 0; i < 2; i++) {
        threads[i] = std::make_unique<PcoThread>(&MutexFromSemTest::runTask, this);
    }
    for(int i = 0; i < 2; i++) {
        threads[i]->join();
    }
    logger() << "Fin des taches : global = " << global
             << " (" << 2 * nbIterations << ")" << std::endl;
    ASSERT_EQ( global, 2 * nbIterations);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    logger().initialize(argc, argv);
    return RUN_ALL_TESTS();
}
