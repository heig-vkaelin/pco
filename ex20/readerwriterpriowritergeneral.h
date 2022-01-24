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
      public AbstractReaderWriter {
protected:
  PcoSemaphore mutex;

  PcoSemaphore readerBlocker;
  int nbReadersWaiting;

  PcoSemaphore writerBlocker;
  int nbWritersWaiting;

public:
  ReaderWriterPrioWriterGeneral() :
    mutex(1),
    nbReadersWaiting(0),
    nbWritersWaiting(0)
    {}

  void lockReading() {
  }

  void unlockReading() {
  }

  void lockWriting() {

  }

  void unlockWriting() {
  }
};
#endif // READERWRITERPRIOWRITERGENERAL_H
