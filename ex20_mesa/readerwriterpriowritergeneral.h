/** @file readerwritepriowritergeneral.h
 *  @brief Reader-writer with priority for readers
 *
 *  Implementation of a reader-writer resource manager with priority
 *  for readers. This class follows the "general" way of solving
 *  concurrent problems: internal variables, one mutex, and semaphores
 *  as synchronization primitives.
 *
 *
 *  @author Yann Thoma
 *  @date 08.05.2017
 *  @bug No known bug
 */

#ifndef READERWRITERPRIOWRITERGENERAL_H
#define READERWRITERPRIOWRITERGENERAL_H

#include <pcosynchro/pcosemaphore.h>
#include <pcosynchro/pcoconditionvariable.h>

#include "abstractreaderwriter.h"


class ReaderWriterPrioWriterGeneral :
      public AbstractReaderWriter {
protected:
    int nbLecteur;
    bool isWriting;
    int lecteurWaiting, redacteurWaiting;
    PcoConditionVariable waitLecteur, waitRedacteur;
    PcoMutex mutex;
public:
  ReaderWriterPrioWriterGeneral() :
      nbLecteur(0), isWriting(false), lecteurWaiting(0), redacteurWaiting(0)
      {}

    void lockReading() {
      mutex.lock();
      while (isWriting || redacteurWaiting) {
          waitLecteur.wait(&mutex);
      }
      ++nbLecteur;
      mutex.unlock();
    }


    void unlockReading() {
      mutex.lock();
      --nbLecteur;
      if (redacteurWaiting && !nbLecteur) {
          waitRedacteur.notifyOne();
      }
      mutex.unlock();
    }

    void lockWriting() {
      mutex.lock();
      while (nbLecteur || isWriting) {
          ++redacteurWaiting;
          waitRedacteur.wait(&mutex);
          --redacteurWaiting;
      }
      isWriting = true;
      mutex.unlock();
    }

    void unlockWriting() {
      mutex.lock();
      isWriting = false;
      if (redacteurWaiting) {
          waitRedacteur.notifyOne();
      } else {
          waitLecteur.notifyAll();
      }
      mutex.unlock();
    }
};
#endif // READERWRITERPRIOWRITERGENERAL_H
