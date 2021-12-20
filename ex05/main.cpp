#include <iostream>
#include <gtest/gtest.h>

#include <pcosynchro/pcologger.h>
#include <pcosynchro/pcosemaphore.h>
#include <pcosynchro/pcothread.h>

static int nbRuns = 0;

static PcoSemaphore sem(0);

void taskRun(int id) {
    sem.acquire();
    nbRuns ++;
    logger() << "Tache" << id << "executes" << std::endl;
}


TEST(Synchro, Standard)
{
    const int NB_THREADS = 2;
    int i;
    std::vector<std::unique_ptr<PcoThread>> threads(NB_THREADS);

    nbRuns = 0;

    for (i = 0; i < NB_THREADS; i++) {
        threads[i] = std::make_unique<PcoThread>(taskRun, i);
    }

    PcoThread::usleep(1000000);
    sem.release();
    sem.release();

    EXPECT_EQ(nbRuns, 0);

    for (i = 0; i < NB_THREADS; i++)
        threads[i]->join();

    EXPECT_EQ(nbRuns, NB_THREADS);
}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    logger().initialize(argc, argv);
    return RUN_ALL_TESTS();
}

