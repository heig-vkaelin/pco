#include <pcosynchro/pcohoaremonitor.h>

// 13.3 Version Hoare

class BridgeManagerFloat : PcoHoareMonitor {
private:
    float currentWeight, maxWeight;
    unsigned nbWaiting;
    Condition cond;
public:
    BridgeManagerFloat(float maxWeight) : currentWeight(0), maxWeight(maxWeight), nbWaiting(0) {}

    ~BridgeManagerFloat() {}

    void access(float weight) {
        monitorIn();
        while (currentWeight + weight > maxWeight) {
            ++nbWaiting;
            wait(cond);
        }

        currentWeight += weight;
        monitorOut();
    }

    void leave(float weight) {
        monitorIn();
        currentWeight -= weight;
        unsigned n = nbWaiting;
        for (unsigned i = 0; i < n; ++i) {
            --nbWaiting;
            signal(cond);
        }
        monitorOut();
    }
};
