#include <pcosynchro/pcosemaphore.h>
#include <iostream>


class MutexFromSem
{
protected:
    PcoSemaphore blocking;
    PcoSemaphore mutex;

    bool locked;
    int nbBlocked;

public:
    MutexFromSem() : blocking(0), mutex(1), locked(false), nbBlocked(0)
    {
    }

    ~MutexFromSem()
    {
    }

    void lock()
    {
        mutex.acquire();
        if (!locked) {
            locked = true;
            mutex.release();
        } else {
            nbBlocked++;
            mutex.release();
            blocking.acquire();
        }
    }

    void unlock()
    {
        mutex.acquire();
        if (locked) {
            if (nbBlocked) {
                nbBlocked--;
                blocking.release();
            } else {
                locked = false;
            }
        }
        mutex.release();
    }

    bool trylock()
    {
        // TODO
        return !locked;
    }  //! Returns true if the mutex can be acquired, false if it is already locked
};
