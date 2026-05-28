#pragma once
#include "Unit.hpp"

class Gatherer : public Unit {
private:
    enum class State {
        WaitingInBase ,
        GoingToResource ,
        Mining ,
        ReturningToBase ,
        ResourceDischarge 
    };

    bool isWaiting;

    int targetX;
    
    int targetY;

    State currentState = State::WaitingInBase;

public:
    using Unit::Unit;

    Gatherer(int x, int y, Civilization* owner) : Unit(x,y){
        this->owner = owner;

        this->baseSpeed = 2;
        this->currentSpeed = 2;
        this->baseVisionRadius = 0;
        this->currentVisionRadius = 0;

        this->baseCapacity = 2;
        this->currentCapacity = 2;
        this->carriedResources = 0;

        this->cooldownTime = 0;
        this->activeBuffDuration = 0;

        this->isWaiting = true;
        this->targetX = -1;
        this->targetY = -1;
        this->backToLifeCooldown = 0;
        
    }

    void performAction(Map* map) override;

    bool get_is_Waiting(){return isWaiting;};

    void assignTargetResource(int x, int y){
        targetX = x;
        targetY = y;
        isWaiting = false;
    }

    int getTargetX() const { return targetX; }

    int getTargetY() const { return targetY; }
};
