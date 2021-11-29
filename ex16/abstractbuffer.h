#ifndef ABSTRACTBUFFER_H
#define ABSTRACTBUFFER_H

template<typename T>
class AbstractBuffer {
public:
    virtual void put(T item) = 0;
    virtual T get() = 0;
};

#endif // ABSTRACTBUFFER_H
