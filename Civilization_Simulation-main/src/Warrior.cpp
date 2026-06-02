#include "../include/Warrior.hpp"
#include "../include/Unit.hpp"
#include "../include/Civilization.hpp"
#include "../include/Scout.hpp"
#include "../include/Gatherer.hpp"
#include "../include/Map.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <cmath>

void Warrior::setProtectedTarget(Gatherer* target) {
    this->protectedTarget = target;
}

void Warrior::performAction(Map* map, Simulation* sim){
    if (map == nullptr || owner == nullptr) return;

    // określające środek obecnego patrolu
    int patrolCenterX = owner->get_X();
    int patrolCenterY = owner->get_Y();
    bool guardingResource = false;

    // szukamy aktywnego zbieracza, którego trzeba bronić
    for (Unit* unit : owner->getUnits()) {
        if (unit == nullptr || !unit->get_isActive()) continue;
        
        Gatherer* g = dynamic_cast<Gatherer*>(unit);
        
        //wojownik broni zbieracza, dopóki ten nie ma pełnego plecaka
        if (g != nullptr && !g->get_is_Waiting()) {
            
            // weryfikacja, czy inny wojownik już go nie chroni
            bool isAlreadyGuarded = false;
            for (Unit* otherUnit : owner->getUnits()) {
                Warrior* otherWarrior = dynamic_cast<Warrior*>(otherUnit);
                
                if (otherWarrior != nullptr && otherWarrior != this && otherWarrior->getProtectedTarget() == g) {
                    isAlreadyGuarded = true;
                    break; // skoro jest chroniony, przerywamy sprawdzanie innych wojowników
                }
            }

            if (!isAlreadyGuarded) {
                this->protectedTarget = g;
                
                // środkiem patrolu staje się aktualna pozycja zbieracza
                patrolCenterX = g->get_X();
                patrolCenterY = g->get_Y();
                guardingResource = true;
                break; // Znaleźliśmy zbieracza
            }
        }
    }

    if (!guardingResource) {
        this->protectedTarget = nullptr;
    }

    // wykonanie ruchu patrolowego wokół wyznaczonego środka
    static thread_local std::mt19937 generator(std::random_device{}());
    int minOffSet = -this->currentSpeed; 
    int maxOffSet = this->currentSpeed; 
    std::uniform_int_distribution<int> distribution(minOffSet, maxOffSet);

    int nextX = this->get_X() + distribution(generator);
    int nextY = this->get_Y() + distribution(generator);
    
    //  ograniczenia mapy
    if (nextX < 0) nextX = 0;
    if (nextX > 99) nextX = 99;
    if (nextY < 0) nextY = 0;
    if (nextY > 99) nextY = 99;

    //  badanie odległości od środka patrolu
    if (guardingResource) {
        // pilnowanie zbieracza: ograniczamy docelowy punkt do max 2 kratek w każdą stronę
        if (nextX > patrolCenterX + 2) nextX = patrolCenterX + 2;
        if (nextX < patrolCenterX - 2) nextX = patrolCenterX - 2;
        if (nextY > patrolCenterY + 2) nextY = patrolCenterY + 2;
        if (nextY < patrolCenterY - 2) nextY = patrolCenterY - 2;
    } 
    else {
        // pilnowanie bazy: oryginalny kod i oryginalne obliczenia dystansu pierwiastkowego
        int distX = nextX - patrolCenterX;
        int distY = nextY - patrolCenterY;
        int distanceSquared = (distX * distX) + (distY * distY);

        //  maksymalny dystans od środka patrolu
        int maxDistanceSquared = this->patrol_size * (owner->getBuildingsCount() + 2);

        //  jeśli punkt losowy wypadł za daleko
        if (distanceSquared > maxDistanceSquared) {
            nextX = (nextX > patrolCenterX) ? nextX - this->currentSpeed : nextX + this->currentSpeed;
            nextY = (nextY > patrolCenterY) ? nextY - this->currentSpeed : nextY + this->currentSpeed;
        }
    }

    this->targetX = nextX;
    this->targetY = nextY;

    this->move(map, this->targetX, this->targetY);
}