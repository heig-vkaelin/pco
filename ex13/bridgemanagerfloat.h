
#include <pcosynchro/pcosemaphore.h>


class BridgeManagerFloat {
private:
    PcoSemaphore waiting;
    PcoSemaphore mutex;
    PcoSemaphore fifo;

    float currentWeight;
    float maxWeight;
    int nbWaiting;
public:
    BridgeManagerFloat(float maxWeight) : waiting(0), mutex(1), fifo(1),
        currentWeight(0), maxWeight(maxWeight), nbWaiting(0) {}

    ~BridgeManagerFloat() {}

    void access(float weight) {
        fifo.acquire();
        mutex.acquire();
        while(currentWeight + weight > maxWeight) {
            ++nbWaiting;
            mutex.release();
            waiting.acquire();
            mutex.acquire();
        }
        currentWeight += weight;
        mutex.release();
        fifo.release();
    }

    void leave(float weight) {
        mutex.acquire();
        currentWeight -= weight;
        for(int i = 0; i < nbWaiting; ++i)
            waiting.release();
        nbWaiting = 0;
        mutex.release();
    }
};
