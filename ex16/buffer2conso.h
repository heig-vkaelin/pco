#ifndef BUFFER2CONSO_H
#define BUFFER2CONSO_H

#include "abstractbuffer.h"
#include <pcosynchro/pcosemaphore.h>

template<typename T> class Buffer2ConsoSemaphore : public AbstractBuffer<T> {
protected:

public:
    Buffer2ConsoSemaphore(unsigned int bufferSize) {}

    virtual ~Buffer2ConsoSemaphore() {}

    virtual void put(T item) {}
    virtual T get(void) {}
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
