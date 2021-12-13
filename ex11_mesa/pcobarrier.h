#ifndef PCOBARRIER_H
#define PCOBARRIER_H

#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcosemaphore.h>
#include <pcosynchro/pcoconditionvariable.h>

class PcoBarrier
{
private:
    unsigned nbToWait;
    PcoConditionVariable cond;
    PcoMutex mutex;

public:
    PcoBarrier(unsigned nbToWait): nbToWait(nbToWait), cond(), mutex() {}

    ~PcoBarrier() {}

    void wait() {
        mutex.lock();

        --nbToWait;
        while (nbToWait > 0) {
            cond.wait(&mutex);
        }
        cond.notifyOne();

        mutex.unlock();
    }
};

#endif // PCOBARRIER_H
