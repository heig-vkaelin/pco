
#include <pcosynchro/pcosemaphore.h>

#define CARWEIGHT 1
#define TRUCKWEIGHT 10

class BridgeManager
{
private:
    PcoSemaphore semAccess;
    std::mutex mutexAccess;

    void leaveBridge(unsigned weight) {
        for (unsigned i = 0; i < weight; ++i) {
            semAccess.release();
        }
    }

    void waitToAccess(unsigned weight) {
        mutexAccess.lock();
        for (unsigned i = 0; i < weight; ++i) {
            semAccess.acquire();
        }
        mutexAccess.unlock();
    }

public:
    BridgeManager(unsigned maxWeight) : semAccess(maxWeight) {}

    ~BridgeManager() {}

    void carAccess() {
        waitToAccess(CARWEIGHT);
    }

    void truckAccess() {
        waitToAccess(TRUCKWEIGHT);
    }

    void carLeave() {
        leaveBridge(CARWEIGHT);
    }

    void truckLeave() {
        leaveBridge(TRUCKWEIGHT);
    }
};
