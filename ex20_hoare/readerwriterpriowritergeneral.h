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
#include <pcosynchro/pcohoaremonitor.h>

#include "abstractreaderwriter.h"


class ReaderWriterPrioWriterGeneral :
      public AbstractReaderWriter, public PcoHoareMonitor {
protected:
  int nbReadersWaiting;
  int nbWritersWaiting;
  int nbReaders;
  bool isWriting;
  Condition writerBlocker, readerBlocker;

public:
  ReaderWriterPrioWriterGeneral() :
    nbReadersWaiting(0),
    nbWritersWaiting(0),
    nbReaders(0),
    isWriting(false),
    writerBlocker(),
    readerBlocker()
    {}

  void lockReading() {
      monitorIn();

      if (isWriting || nbWritersWaiting) {
          ++nbReadersWaiting;
          wait(readerBlocker);
          --nbReadersWaiting;
      }
      ++nbReaders;

      if (nbReadersWaiting) {
          signal(readerBlocker);
      }

      monitorOut();
  }


  void unlockReading() {
      monitorIn();

      --nbReaders;
      if (nbReaders == 0 && nbWritersWaiting) {
          signal(writerBlocker);
      }

      monitorOut();
  }

  void lockWriting() {
      monitorIn();

      if (isWriting || nbReaders) {
          ++nbWritersWaiting;
          wait(writerBlocker);
          --nbWritersWaiting;
      }
      isWriting = true;

      monitorOut();

  }

  void unlockWriting() {
      monitorIn();

      isWriting = false;
      if (nbWritersWaiting) {
          signal(writerBlocker);
      } else if (nbReadersWaiting) {
          signal(readerBlocker);
      }

      monitorOut();
  }
};
#endif // READERWRITERPRIOWRITERGENERAL_H
