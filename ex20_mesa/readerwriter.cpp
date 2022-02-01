/** @file readerwriter.cpp
 *  @brief Test of reader-writer protocols
 *
 *  This code instantiates a reader-writer protocol and starts
 *  a certain number of readers and writers threads.
 *  A checker performs a light check in order to ensure
 *  mutual exclusion between readers and writers.
 *  You just need to change the resource creation to create
 *  various protocols.
 *
 *  @author Yann Thoma
 *  @date 08.05.2017
 *  @bug No known bug
 */

// General includes
#include <iostream>
#include <random>

#include <gtest/gtest.h>

#include <pcosynchro/pcologger.h>
#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcothread.h>
#include <pcosynchro/pcotest.h>

// Application related includes
#include "abstractreaderwriter.h"
#include "readerwriterpriowritergeneral.h"


class ReaderWriterChecker {
public:
    void writerIn()
    {
        checkerLock.lock();
        nbWriters++;
        // Check after getting in
        check();
        checkerLock.unlock();

    }

    void writerOut()
    {
        checkerLock.lock();
        // Check before leaving
        check();
        nbWriters--;
        checkerLock.unlock();
    }

    void readerIn()
    {
        checkerLock.lock();
        nbReaders++;
        // Check after getting in
        check();
        checkerLock.unlock();
    }

    void readerOut()
    {
        checkerLock.lock();
        // Check before leaving
        check();
        nbReaders--;
        checkerLock.unlock();
    }

protected:

    void check()
    {
        ASSERT_EQ((nbReaders == 0) || (nbWriters == 0), true);
        ASSERT_LE(nbWriters, 1);
        ASSERT_GE(nbWriters, 0);
        ASSERT_GE(nbReaders, 0);
    }

    std::mutex checkerLock;

    int nbReaders{0};
    int nbWriters{0};
};



void taskWriter(int tid, int nbIterations, AbstractReaderWriter *resource, ReaderWriterChecker *checker)
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(1, 1000);

    for(int iter = 0; iter < nbIterations; iter ++) {
        resource->lockWriting();
        checker->writerIn();
        logger() << "Task W" << tid << ": accessing " << iter << std::endl;
        PcoThread::usleep(static_cast<uint64_t>(dis(gen)));
        checker->writerOut();
        resource->unlockWriting();
        logger() << "Task W" << tid << ": leaving" << std::endl;
        PcoThread::usleep(static_cast<uint64_t>(dis(gen)));
    }
}


void taskReader(int tid, int nbIterations, AbstractReaderWriter *resource, ReaderWriterChecker *checker)
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(1, 1000);

    for(int iter = 0; iter < nbIterations; iter ++) {
        resource->lockReading();
        checker->readerIn();
        logger() << "Task R" << tid << ": accessing " << iter << std::endl;
        PcoThread::usleep(static_cast<uint64_t>(dis(gen)));
        checker->readerOut();
        resource->unlockReading();
        logger() << "Task R" << tid << ": leaving" << std::endl;
        PcoThread::usleep(static_cast<uint64_t>(dis(gen)));
    }
}

template<typename T>
class ReaderWriterPrioWriterTest : public ::testing::Test
{
protected:

    void testReaderWriterPrioWriter(int nbThreadsA, int nbThreadsB, int nbIterations)
    {
        ASSERT_DURATION_LE(15, {
                               std::unique_ptr<T> protocol = std::make_unique<T>();

                               std::vector<std::unique_ptr<std::thread> > threadsA(nbThreadsA);
                               std::vector<std::unique_ptr<std::thread> > threadsB(nbThreadsB);

                               auto checker = std::make_unique<ReaderWriterChecker>();

                               for(int t=0; t<nbThreadsB; t++){
                                   logger() << "In main: creating thread B " << t << std::endl;
                                   threadsB[t] = std::make_unique<std::thread>(taskReader, t, nbIterations, protocol.get(), checker.get());
                               }
                               for(int t=0; t<nbThreadsA; t++){
                                   logger() << "In main: creating thread A " << t << std::endl;
                                   threadsA[t] = std::make_unique<std::thread>(taskWriter, t, nbIterations, protocol.get(), checker.get());
                               }

                               for(int t=0; t<nbThreadsB; t++){
                                   threadsB[t]->join();
                               }
                               for(int t=0; t<nbThreadsA; t++){
                                   threadsA[t]->join();
                               }
                           });
    }
};

using MyTypes = ::testing::Types<ReaderWriterPrioWriterGeneral>;

TYPED_TEST_CASE(ReaderWriterPrioWriterTest, MyTypes);

TYPED_TEST(ReaderWriterPrioWriterTest, Standard) {
    this->testReaderWriterPrioWriter(3, 2, 200);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    logger().initialize(argc, argv);
    return RUN_ALL_TESTS();
}
