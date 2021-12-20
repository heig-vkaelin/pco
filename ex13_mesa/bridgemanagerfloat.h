#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcoconditionvariable.h>

// 13.2 Version Mesa

class BridgeManagerFloat {
private:
    float currentWeight, maxWeight;
    PcoMutex mutex;
    PcoConditionVariable canEnter;
public:
    BridgeManagerFloat(float maxWeight) : currentWeight(0), maxWeight(maxWeight) {}

    ~BridgeManagerFloat() {}

    void access(float weight) {
        mutex.lock();
        while (currentWeight + weight > maxWeight)
            canEnter.wait(&mutex);

        currentWeight += weight;
        mutex.unlock();
    }

    void leave(float weight) {
        mutex.lock();
        currentWeight -= weight;
        canEnter.notifyAll();
        mutex.unlock();
    }
};
