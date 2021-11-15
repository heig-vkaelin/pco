#ifndef PCOBARRIER_H
#define PCOBARRIER_H

#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcosemaphore.h>

class PcoBarrier
{
private:
    unsigned nbBlocked;
    unsigned nbToWait;
    PcoSemaphore blocking;
    PcoSemaphore mutex;

public:
    PcoBarrier(unsigned nbToWait): nbBlocked(0), nbToWait(nbToWait), blocking(0), mutex(1) {
    }

    ~PcoBarrier() {}

    void wait() {
        mutex.acquire();
        if (nbBlocked + 1 == nbToWait) { // Tout relacher
            unsigned n = nbBlocked;
            mutex.release();
            for (unsigned i = 0; i < n; ++i) {
                blocking.release();
            }
        } else { // Mettre en attente
            nbBlocked++;
            mutex.release();
            blocking.acquire();
        }
    }
};

#endif // PCOBARRIER_H
