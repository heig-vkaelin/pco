#ifndef BUFFER2CONSO_H
#define BUFFER2CONSO_H

#include "abstractbuffer.h"
#include <pcosynchro/pcosemaphore.h>
#include <vector>

template<typename T> class Buffer2ConsoSemaphore : public AbstractBuffer<T> {
protected:
    PcoSemaphore mutex;
    PcoSemaphore full;
    PcoSemaphore empty;
    int nbGet;
    int nbWaitingProd,nbWaitingConso;
    int maxElem;
    int nbElem;
    int indexGet;
    int indexSet;
    std::vector<T> buffer;
public:
    Buffer2ConsoSemaphore(unsigned int bufferSize)
        : mutex(1),full(0),empty(0),maxElem(bufferSize),indexSet(0),indexGet(0),nbElem(0),buffer(maxElem),
    nbWaitingProd(0),nbWaitingConso(0),nbGet(2){}
    virtual ~Buffer2ConsoSemaphore() {}
    virtual void put(T item) {
        mutex.acquire();
        if(nbElem >= maxElem){
            ++nbWaitingProd;
            mutex.release();
            empty.acquire();
        }
        buffer[indexSet] = item;
        nbElem++;
        indexSet = (indexSet + 1) % maxElem;
        if(nbWaitingConso > 0){
            nbWaitingConso--;
            full.release();
        } else{
            mutex.release();
        }
    }
    virtual T get(void) {
        mutex.acquire();
        if(nbElem <= 0){
            ++nbWaitingConso;
            mutex.release();
            full.acquire();
        }
        T item = buffer[indexGet];
        if(nbGet > 1){
            nbGet--;
            if(nbWaitingConso > 0){
                --nbWaitingConso;
                full.release();
            } else{
                mutex.release();
            }
        } else{
            if(nbWaitingProd > 0){
                --nbWaitingProd;
                empty.release();
            } else{
                mutex.release();
            }
            nbElem--;
            indexGet = (indexGet + 1) % maxElem;
            nbGet = 2;
        }
        return item;
    }
};


#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcoconditionvariable.h>

template<typename T> class Buffer2ConsoMesa : public AbstractBuffer<T> {
protected:
    PcoMutex mutex;
    PcoConditionVariable full;
    PcoConditionVariable empty;
    int nbElem,maxElem;
    int indexSet,indexGet;
    int nbGet;
    int nbWaitConso,nbWaitProdu;
    std::vector<T> buffer;
public:
    Buffer2ConsoMesa(unsigned int bufferSize)
        : mutex(),buffer(bufferSize),nbElem(0),maxElem(bufferSize),nbGet(2),indexGet(0),indexSet(0),nbWaitProdu(0),nbWaitConso(0) {}

    virtual ~Buffer2ConsoMesa() {}

    virtual void put(T item) {
        mutex.lock();
        if(nbElem >= maxElem){
            nbWaitProdu++;
            full.wait(&mutex);

        }
        ++nbElem;
        buffer[indexSet] = item;
        indexSet = (indexSet+1) % maxElem;
        if(nbWaitConso > 0){
            nbWaitConso--;
            empty.notifyOne();
        }
        mutex.unlock();
    }
    virtual T get(void) {
        mutex.lock();
        if(nbElem <= 0){
            nbWaitConso++;
            empty.wait(&mutex);

        }
        T item = buffer[indexGet];
        if(nbGet > 1){
            --nbGet;
            if(nbWaitConso > 0){
                nbWaitConso--;
                empty.notifyOne();
            }
        } else{
            nbGet = 2;
            nbElem--;
            indexGet = (indexGet+1) % maxElem;
            if(nbWaitProdu > 0){
                nbWaitProdu--;
                full.notifyOne();
            }
        }
        mutex.unlock();
        return item;
    }
};


#include <pcosynchro/pcohoaremonitor.h>

template<typename T> class Buffer2ConsoHoare : public AbstractBuffer<T>, public PcoHoareMonitor {
protected:

public:
    Buffer2ConsoHoare(unsigned int bufferSize) {}

    virtual ~Buffer2ConsoHoare() {}

    virtual void put(T item) {}
    virtual T get(void) {}
};


#endif // BUFFER2CONSO_H
