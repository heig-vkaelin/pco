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
#include "readerwriterclassab.h"


class ReaderWriterChecker {
public:
    void classAIn()
    {
        checkerLock.lock();
        nbA++;
        // Check after getting in
        check();
        checkerLock.unlock();

    }

    void classAOut()
    {
        checkerLock.lock();
        // Check before leaving
        check();
        nbA--;
        checkerLock.unlock();
    }

    void classBIn()
    {
        checkerLock.lock();
        nbB++;
        // Check after getting in
        check();
        checkerLock.unlock();
    }

    void classBOut()
    {
        checkerLock.lock();
        // Check before leaving
        check();
        nbB--;
        checkerLock.unlock();
    }

protected:

    void check()
    {
        ASSERT_EQ((nbB == 0) || (nbA == 0), true);
    }

    std::mutex checkerLock;

    int nbB{0};
    int nbA{0};
};



void taskWriter(int tid, int nbIterations, ReaderWriterClassAB *resource, ReaderWriterChecker *checker)
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(1, 1000);

    for(int iter = 0; iter < nbIterations; iter ++) {
        resource->lockA();
        checker->classAIn();
        logger() << "Task " << tid << ": accessing" << std::endl;
        PcoThread::usleep(static_cast<uint64_t>(6.0 * dis(gen)));
        checker->classAOut();
        resource->unlockA();
        PcoThread::usleep(static_cast<uint64_t>(6.0 * dis(gen)));
    }
}


void taskReader(int tid, int nbIterations, ReaderWriterClassAB *resource, ReaderWriterChecker *checker)
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(1, 1000);

    for(int iter = 0; iter < nbIterations; iter ++) {
        resource->lockB();
        checker->classBIn();
        logger() << "Task " << tid << ": accessing" << std::endl;
        PcoThread::usleep(static_cast<uint64_t>(6.0 * dis(gen)));
        checker->classBOut();
        resource->unlockB();
        PcoThread::usleep(static_cast<uint64_t>(6.0 * dis(gen)));
    }
}

/*
void testReaderWriterClassAB(int nbThreadsA, int nbThreadsB, int nbIterations, ReaderWriterClassAB *protocol)
{
    ASSERT_DURATION_LE(15, {

                           std::vector<std::unique_ptr<std::thread> > threadsA(nbThreadsA);
                           std::vector<std::unique_ptr<std::thread> > threadsB(nbThreadsB);

                           auto checker = std::make_unique<ReaderWriterChecker>();

                           for(int t=0; t<nbThreadsB; t++){
                               logger() << "In main: creating thread B " << t << std::endl;
                               threadsB[t] = std::make_unique<std::thread>(classBTask, t, nbIterations, protocol, checker.get());
                           }
                           for(int t=0; t<nbThreadsA; t++){
                               logger() << "In main: creating thread A " << t << std::endl;
                               threadsA[t] = std::make_unique<std::thread>(classATask, t, nbIterations, protocol, checker.get());
                           }

                           for(int t=0; t<nbThreadsB; t++){
                               threadsB[t]->join();
                           }
                           for(int t=0; t<nbThreadsA; t++){
                               threadsA[t]->join();
                           }
                       });
}

TEST(ReaderWriterClassAB, Sem) {
    std::unique_ptr<ReaderWriterClassAB> resource = std::make_unique<ReaderWriterClassABSem>();
    testReaderWriterClassAB(3, 2, 1000, resource.get());
}

TEST(ReaderWriterClassAB, Mesa) {
    std::unique_ptr<ReaderWriterClassAB> resource = std::make_unique<ReaderWriterClassABMesa>();
    testReaderWriterClassAB(3, 2, 1000, resource.get());
}

TEST(ReaderWriterClassAB, Hoare) {
    std::unique_ptr<ReaderWriterClassAB> resource = std::make_unique<ReaderWriterClassABHoare>();
    testReaderWriterClassAB(3, 2, 1000, resource.get());
}

TEST(ReaderWriterClassAB, SemAlternative) {
    std::unique_ptr<ReaderWriterClassAB> resource = std::make_unique<ReaderWriterClassABSemAlternative>();
    testReaderWriterClassAB(3, 2, 1000, resource.get());
}
*/

template<typename T>
class ReaderWriterClassABTest : public ::testing::Test
{
protected:

    void testReaderWriterClassAB(int nbThreadsA, int nbThreadsB, int nbIterations)
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

using MyTypes = ::testing::Types<ReaderWriterClassAB>;

TYPED_TEST_CASE(ReaderWriterClassABTest, MyTypes);

TYPED_TEST(ReaderWriterClassABTest, Standard) {
    this->testReaderWriterClassAB(3, 2, 1000);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    logger().initialize(argc, argv);
    return RUN_ALL_TESTS();
}
