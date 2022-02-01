#include <pcosynchro/pcohoaremonitor.h>
#include <pcosynchro/pcomutex.h>

#define MAX_N 10

class AbstractToilet {
public:
	AbstractToilet(int nbSeats) : nbSeats(nbSeats) {}; 
	virtual void manAccessing() = 0;
	virtual void manLeaving() = 0;
	virtual void womanAccessing() = 0;
	virtual void womanLeaving() = 0;
protected:
	int nbSeats;
};

class Toilet : public AbstractToilet, public PcoHoareMonitor {
private: 

Condition manBlocking, womanBlocking;
int n;
int manWaiting, womanWaiting;
int nbManIn, nbWomanIn;


public:
Toilet(int nbSeats) : AbstractToilet(nbSeats),
	manBlocking(), womanBlocking(), n(0), manWaiting(0), womanWaiting(0), nbManIn(0), nbWomanIn(0) {
}

virtual void womanAccessing() {
	monitorIn();
	
	if (nbManIn != 0 || n >= MAX_N || nbWomanIn >= nbSeats) {
		++womanWaiting;
		wait(womanBlocking);
		--womanWaiting;
	}
	
	++n;
	++nbWomanIn;
	
	if (n < MAX_N && womanWaiting && nbWomanIn < nbSeats) {
		signal(womanBlocking);
	}
	
	monitorOut();
}

virtual void manAccessing() {
	monitorIn();
	
	if (nbWomanIn != 0 || n >= MAX_N || nbManIn >= nbSeats) {
		++manWaiting;
		wait(manBlocking);
		--manWaiting;
	}
	
	++n;
	++nbManIn;
	
	if (n < MAX_N && manWaiting && nbManIn < nbSeats) {
		signal(manBlocking);
	}
	
	monitorOut();
}

virtual void womanLeaving() {
	monitorIn();
	
	--nbWomanIn;
	if (n < MAX_N) {
		if (womanWaiting) {
			signal(womanBlocking);
		}
	} else if (nbWomanIn == 0) {
		n = 0;
		if (manWaiting) {
			signal(manBlocking);
		}
	}
	
	monitorOut();
}

virtual void manLeaving() {
	monitorIn();
	
	--nbManIn;
	if (n < MAX_N) {
		if (manWaiting) {
			signal(manBlocking);
		}
	} else if (nbManIn == 0) {
		n = 0;
		if (womanWaiting) {
			signal(womanBlocking);
		}
	}
	
	monitorOut();
}

};
