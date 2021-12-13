#ifndef PCOBARRIER_H
#define PCOBARRIER_H

#include <pcosynchro/pcohoaremonitor.h>

class PcoBarrier : PcoHoareMonitor
{
private:
    unsigned nbToWait;
    Condition cond;

public:
    PcoBarrier(unsigned nbToWait): nbToWait(nbToWait) {}

    ~PcoBarrier() {}

    void wait() {
        monitorIn();

        nbToWait--;
        if (nbToWait > 0) {
            PcoHoareMonitor::wait(cond);
        }
        signal(cond);

        monitorOut();
    }
};

#endif // PCOBARRIER_H
