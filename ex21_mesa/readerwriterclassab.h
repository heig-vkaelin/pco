/** @file readerwriterclassab.h
 *  @brief Reader-writer with equal priority between two classes
 *
 *  Implementation of a reader-writer resource manager with equal
 *  priority for both classes. Mutual exclusion between classes must be
 *  ensured
 *
 *
 *
 *  @author Yann Thoma
 *  @date 15.05.2017
 *  @bug No known bug
 */

#ifndef READERWRITERCLASSAB_H
#define READERWRITERCLASSAB_H

#include <pcosynchro/pcosemaphore.h>
#include <pcosynchro/pcohoaremonitor.h>
#include <pcosynchro/pcoconditionvariable.h>

#include "abstractreaderwriter.h"


class ReaderWriterClassAB
{
protected:
int nbA,nbB;
PcoConditionVariable condA,condB;
PcoMutex mutex;

public:
  ReaderWriterClassAB() :nbA(0),nbB(0)
  {
  }

  void lockA() {
    mutex.lock();
    while(nbB > 0){
        condA.wait(&mutex);
    }
    nbA++;

    mutex.unlock();
  }

  void unlockA() {
    mutex.lock();
    nbA--;
    if(nbA <= 0){
        condB.notifyAll();
    }
    mutex.unlock();
  }

  void lockB() {
    mutex.lock();
    while(nbA > 0){
        condB.wait(&mutex);
    }
    nbB++;

    mutex.unlock();

  }

  void unlockB() {
    mutex.lock();
    nbB--;
    if(nbB <= 0){
        condA.notifyAll();
    }
    mutex.unlock();
  }
};
#endif // READERWRITERCLASSAB_H
