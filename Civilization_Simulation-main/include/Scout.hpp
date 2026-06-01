#pragma once

#include "Unit.hpp"


class Civilization;

class Scout : public Unit {
public:

    Scout(int x, int y, Civilization* owner) : Unit(x,y){
        this->owner = owner;

        this->baseSpeed = 4;
        this->currentSpeed = 4;
        this->baseVisionRadius = 6;
        this->currentVisionRadius = 6;

        this->baseCapacity = 0;
        this->currentCapacity = 0;
        this->carriedResources = 0;

        this->cooldownTime = 0;
        this->activeBuffDuration = 0;
        this->backToLifeCooldown = 0;
    }
    
    void performAction(Map* map, Simulation* sim) override;
};
