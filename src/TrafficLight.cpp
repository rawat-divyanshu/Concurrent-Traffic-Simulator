#include <iostream>
#include <random>
#include "TrafficLight.h"
 
template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> ulck(_mtx);
    _cond.wait(ulck, [this] {return !_queue.empty();});
    T returnValue = std::move(_queue.back());
    _queue.pop_back();
    return returnValue;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lck(_mtx);
    _queue.emplace_back(std::move(msg));
    _cond.notify_one();
}

 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        TrafficLightPhase receivedPhase = _msgQueue.receive();
        if(receivedPhase == TrafficLightPhase::green) {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}


void TrafficLight::cycleThroughPhases()
{
    std::random_device rd;
  	std::uniform_int_distribution<int> distribution(4000,6000);
  	long cycleDuration = distribution(rd);
    auto lastUpdate = std::chrono::system_clock::now();
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if(timeSinceLastUpdate >= cycleDuration) {
            _currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;
            _msgQueue.send(std::move(_currentPhase));
            lastUpdate = std::chrono::system_clock::now();
            cycleDuration = distribution(rd);
        }
    }
}

