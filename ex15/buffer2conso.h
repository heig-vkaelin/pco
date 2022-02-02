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
    PcoSemaphore waitFull, waitEmpty;
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
    static const unsigned MAX_CONSUME = 2;
    PcoMutex mutex;
    PcoConditionVariable isFull, isEmpty;
    bool hasElement;
    T element;
    unsigned nbConsume;

public:
    Buffer2ConsoMesa() : hasElement(false), nbConsume(0) {}

    virtual ~Buffer2ConsoMesa() {}

    virtual void put(T item) {
        mutex.lock();
        while (hasElement)
            isEmpty.wait(&mutex);

        element = item;
        hasElement = true;
        isFull.notifyAll();

        // Autre version pour éviter de réveiller tout le monde
        /* for (unsigned i = 0; i < MAX_CONSUME; ++i)
            isFull.notifyOne();
        */

        mutex.unlock();

    }
    
    virtual T get(void) {
        T item;
        mutex.lock();
        while (!hasElement)
            isFull.wait(&mutex);

        item = element;
        ++nbConsume;

        if (nbConsume == MAX_CONSUME) {
            nbConsume = 0;
            hasElement = false;
            isEmpty.notifyOne();
        }
        mutex.unlock();
        return item;
    }
};


#include <pcosynchro/pcohoaremonitor.h>

template<typename T> class Buffer2ConsoHoare : public AbstractBuffer<T>, public PcoHoareMonitor {
protected:
    static const unsigned MAX_CONSUME = 2;
    Condition notEmpty, notFull;
    T element;
    bool hasElement;
    unsigned nbConsume;
public:
    Buffer2ConsoHoare() : hasElement(false), nbConsume(0) {}

    virtual ~Buffer2ConsoHoare() {}

    virtual void put(T item) {
        monitorIn();
        if (hasElement)
            wait(notFull);

        element = item;
        hasElement = true;
        signal(notEmpty);
        monitorOut();
    }

    virtual T get(void) {
        T item;
        monitorIn();
        if (!hasElement)
            wait(notEmpty);

        item = element;
        ++nbConsume;

        if (nbConsume == MAX_CONSUME) {
            nbConsume = 0;
            hasElement = false;
            signal(notFull);
        } else {
            signal(notEmpty);
        }
        monitorOut();
        return item;
    }
};

#endif // BUFFER2CONSO_H
