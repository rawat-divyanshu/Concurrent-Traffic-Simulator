#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

class Vehicle;

enum TrafficLightPhase { red, green };

template <class T>
class MessageQueue
{
public:
T receive();
void send(T &&msg);
std::deque<TrafficLightPhase> _queue;

private:
std::condition_variable _cond;
std::mutex _mtx;
};

class TrafficLight : TrafficObject
{
public:
    TrafficLight();
    
    TrafficLightPhase getCurrentPhase();
    
    void waitForGreen();
    
    void simulate();

private:
    void cycleThroughPhases();
    
    TrafficLightPhase _currentPhase;
    
    MessageQueue<TrafficLightPhase> _msgQueue;

    std::condition_variable _condition;
    
    std::mutex _mutex;
};

#endif