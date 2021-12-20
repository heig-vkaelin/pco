#ifndef BUFFER2CONSO_H
#define BUFFER2CONSO_H

#include <pcosynchro/pcosemaphore.h>

#include "abstractbuffer.h"

template<typename T> class Buffer2ConsoSemaphore : public AbstractBuffer<T> {
protected:
    PcoSemaphore waitFull, waitEmpty, mutex;
    T element;

public:
    Buffer2ConsoSemaphore() : waitFull(0), waitEmpty(2), mutex(1) {}

    virtual ~Buffer2ConsoSemaphore() {}

    virtual void put(T item) {
        mutex.acquire();
        waitEmpty.acquire();
        waitEmpty.acquire();
        mutex.release();
        element = item;
        waitFull.release();
        waitFull.release();
    }
    virtual T get(void) {
        T item;
        waitFull.acquire();
        item = element;
        waitEmpty.release();
        return item;
    }
};

#define N 2

template<typename T> class Buffer2ConsoSemaphoreGeneral : public AbstractBuffer<T> {
protected:
    PcoSemaphore waitFull, waitEmpty, mutex;
    T element;
    int consume;

public:
    Buffer2ConsoSemaphoreGeneral() : waitFull(0), waitEmpty(1), consume(0) {}

    virtual ~Buffer2ConsoSemaphoreGeneral() {}

    virtual void put(T item) {
        waitEmpty.acquire();
        element = item;
        waitFull.release();
    }
    virtual T get(void) {
        T item;
        waitFull.acquire();
        item = element;
        consume++;
        if (consume == N) {
           consume = 0;
           waitEmpty.release();
        } else {
            waitFull.release();
        }
        return item;
    }
};


#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcoconditionvariable.h>

template<typename T> class Buffer2ConsoMesa : public AbstractBuffer<T> {
protected:
    PcoMutex mutex;

public:
    Buffer2ConsoMesa() {}

    virtual ~Buffer2ConsoMesa() {}

    virtual void put(T item) {

    }
    virtual T get(void) {

    }
};


#include <pcosynchro/pcohoaremonitor.h>

template<typename T> class Buffer2ConsoHoare : public AbstractBuffer<T>, public PcoHoareMonitor {
protected:

public:
    Buffer2ConsoHoare() {}

    virtual ~Buffer2ConsoHoare() {}

    virtual void put(T item) {

    }
    virtual T get(void) {

    }
};


#endif // BUFFER2CONSO_H
