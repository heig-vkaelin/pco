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

#include "abstractreaderwriter.h"


class ReaderWriterClassAB
{
protected:

public:
  ReaderWriterClassAB() {
  }

  void lockA() {
  }

  void unlockA() {
  }

  void lockB() {

  }

  void unlockB() {
  }
};
#endif // READERWRITERCLASSAB_H
