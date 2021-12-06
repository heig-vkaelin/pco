#ifndef BUFFER2CONSO_H
#define BUFFER2CONSO_H

#include "abstractbuffer.h"
#include <pcosynchro/pcosemaphore.h>

template<typename T> class Buffer2ConsoSemaphore : public AbstractBuffer<T> {
protected:
    std::vector<T> elements;
    int writePointer, readPointer, nbElements, bufferSize;
    PcoSemaphore mutex, waitProd, waitConso;
    unsigned nbWaitingProd, nbWaitingConso;

public:
    Buffer2ConsoSemaphore(unsigned bufferSize) : elements(bufferSize),
        writePointer(0), readPointer(0), nbElements(0), bufferSize(bufferSize),
        mutex(1), waitProd(0), waitConso(0), nbWaitingProd(0), nbWaitingConso(0) {}

    virtual ~Buffer2ConsoSemaphore() {}

    virtual void put(T item) {
        mutex.acquire();
        if (nbElements == bufferSize) {
            nbWaitingProd++;
            mutex.release();
            waitProd.acquire();
        }
        elements[writePointer] = item;
        writePointer = (writePointer + 1) % bufferSize;
        nbElements++;
        if (nbWaitingConso > 0) {
            nbWaitingConso--;
            waitConso.release();
        } else {
            mutex.release();
        }
    }

    virtual T get(void) {
        T item;
        mutex.acquire();
        if (nbElements == 0) {
            nbWaitingConso++;
            mutex.release();
            waitConso.acquire();
        }
        item = elements[readPointer];
        readPointer = (readPointer + 1) % bufferSize;
        nbElements--;
        if (nbWaitingProd > 0) {
            nbWaitingProd--;
            waitProd.release();
        } else {
            mutex.release();
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
    Buffer2ConsoMesa(unsigned int bufferSize) {}

    virtual ~Buffer2ConsoMesa() {}

    virtual void put(T item) {}
    virtual T get(void) {}
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
