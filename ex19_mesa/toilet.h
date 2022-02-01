#include <pcosynchro/pcohoaremonitor.h>
#include <pcosynchro/pcomutex.h>

#define MAX_N 10

class AbstractToilet {
public:
    AbstractToilet(int nbSeats) : nbSeats(nbSeats) {}; 
    virtual void manAccessing() = 0;
    virtual void manLeaving() = 0;
    virtual void womanAccessing() = 0;
    virtual void womanLeaving() = 0;
protected:
    int nbSeats;
};

class Toilet : public AbstractToilet, public PcoHoareMonitor {
private: 
PcoConditionVariable waitFemme, waitHomme;
PcoMutex mutex;
int n;
int nbHommeIn, nbFemmeIn;
int nbFemmeWaiting,nbHommeWaiting;
public:
Toilet(int nbSeats) : AbstractToilet(nbSeats), n(0),nbHommeIn(0),nbFemmeIn(0)
}

virtual void womanAccessing() {
    mutex.lock();
    if(nbHommeIn != 0 || nbFemmeIn >= nbSeats || n >= MAX_N){
        waitFemme.wait(&mutex);
    }
    ++n;
    ++nbFemmeIn;

    mutex.unlock();
}

virtual void manAccessing() {
    mutex.lock();
    if(nbFemmeIn != 0 || nbHommeIn >= nbSeats || n >= MAX_N){
        waitHomme.wait(&mutex);
    }
    ++n;
    ++nbHommeIn;

    mutex.unlock();
}

virtual void womanLeaving() {
    mutex.lock();
    --nbFemmeIn;
    if(n < MAX_N){
        waitFemme.notifyOne();
    } else if(!nbHommeIn){
        n = 0;
        for(int i = min(nbHommeWaiting,MAX_N);i > 0 ;--i)
            waitHomme.notifyOne();
    }
    mutex.unlock();
}

virtual void manLeaving() {
    mutex.lock()
    --nbHommeIn;
    if(n < MAX_N)
        waitHomme.notifyOne();
    else if(!nbHommeIn){
        n = 0;
        for(int i = min(nbFemmeWaiting,MAX_N);i > 0 ;--i)
            waitFemme.notifyOne();
    }
    mutex.unlock();
}

};