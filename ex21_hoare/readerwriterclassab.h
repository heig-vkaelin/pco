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


class ReaderWriterClassAB : public PcoHoareMonitor
{
protected:
    int nbWaitingA;
    int nbWaitingB;
    int nbA;
    int nbB;
    Condition blockerA, blockerB;

public:
  ReaderWriterClassAB() : nbWaitingA(0), nbWaitingB(0), nbA(0), nbB(0), blockerA(), blockerB()
  {
  }

  void lockA() {
      monitorIn();

      if (nbB) {
          ++nbWaitingA;
          wait(blockerA);
          --nbWaitingA;
      }
      ++nbA;

      if (nbWaitingA)
          signal(blockerA);

      monitorOut();
  }

  void unlockA() {
      monitorIn();

      --nbA;
      if (nbA == 0 && nbWaitingB) {
          signal(blockerB);
      }

      monitorOut();
  }

  void lockB() {
      monitorIn();

      if (nbA) {
          ++nbWaitingB;
          wait(blockerB);
          --nbWaitingB;
      }
      ++nbB;

      if (nbWaitingB)
          signal(blockerB);

      monitorOut();
  }

  void unlockB() {
      monitorIn();

      --nbB;
      if (nbB == 0 && nbWaitingA) {
          signal(blockerA);
      }

      monitorOut();
  }
};
#endif // READERWRITERCLASSAB_H
