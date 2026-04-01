#pragma once

#include "raylib.h"



class Timer 
{
public:
    Timer(const double interval = 0.25)
        : interval(interval), previousTime(GetTime()) {}

    // Checks if the interval has passed since the last reset.
    bool HasElapsed() 
    {
        const double currentTime = GetTime();
        if (currentTime - previousTime >= interval) 
        {
            previousTime = currentTime; // Reset timer
            return true;
        }
        return false;
    }

    // Manually resets the timer
    void Reset() 
    {
        previousTime = GetTime();
    }

    // Sets a new interval
    void SetInterval(const double newInterval) 
    {
        interval = newInterval;
    }

    // Gets the current interval
    double GetInterval() const 
    {
        return interval;
    }

private:
    double interval;       // Interval duration in seconds
    double previousTime;   // Last time the timer was checked
};
