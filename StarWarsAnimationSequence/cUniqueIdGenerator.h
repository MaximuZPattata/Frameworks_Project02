#pragma once

#include <chrono>

class cUniqueIDGenerator 
{
private:
    unsigned int counter;

public:
    unsigned int GenerateUnqiueID() 
    {
        auto timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();

        return static_cast<unsigned int>(timestamp) + counter++;
    }
};