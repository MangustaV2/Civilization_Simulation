#pragma once 

#include "Unit.hpp"
#include "Gatherer.hpp"
#include "Scout.hpp"

class Civilization;
class Gatherer;

class Warrior : public Unit {
private:
    int targetX; 
    int targetY;
    bool hasSpecificTarget;
    int patrol_size;

    Gatherer* protectedTarget; 

public:
    Warrior(int x, int y, Civilization* owner) : Unit(x,y) {
        this->owner = owner;

        this->baseSpeed = 3;
        this->currentSpeed = 3;
        this->baseVisionRadius = 1;
        this->currentVisionRadius = 1;

        this->baseCapacity = 0;
        this->currentCapacity = 0;
        this->carriedResources = 0;
        this->patrol_size = 4;
        
        this->hasSpecificTarget = false;
        this->protectedTarget = nullptr;
        this->backToLifeCooldown = 0;
    }

    Gatherer* getProtectedTarget() const { return protectedTarget; }// Funkcja do pobierania zbieracza, którego wojownik ma chronić

    void setProtectedTarget(Gatherer* target);// Funkcja do ustawiania zbieracza, którego wojownik ma chronić

    void performAction(Map* map, Simulation* sim) override;//logika działania wojownika (patrolowanie i ochrona zbieracza)

};

