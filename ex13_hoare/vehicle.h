#ifndef VEHICLE_H
#define VEHICLE_H



class Vehicle
{
    float weight;
public:
    Vehicle(float weight) : weight(weight) {};
    float getWeight() const {return weight;};
    void stop(){};
    void start(){};
};


#endif // VEHICLE_H
