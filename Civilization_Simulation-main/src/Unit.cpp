#include "../include/Civilization.hpp"
#include "../include/GameObject.hpp"
#include "../include/Unit.hpp"

Unit::~Unit(){}


void Unit::update(){
    if (cooldownTime > 0){
        cooldownTime--;
    }
    
    if (activeBuffDuration > 0 ){
        activeBuffDuration--;

        if (activeBuffDuration == 0){

            currentSpeed = baseSpeed;
            currentCapacity = baseCapacity;
            currentVisionRadius = baseVisionRadius;

        }   
    }

    if(backToLifeCooldown > 0){
        backToLifeCooldown--;
        if (backToLifeCooldown == 0){
            this->isActive = true;
        }
    }
}

void Unit::pickUpArtifact(Artifact* artifact){
    
    if (artifact != nullptr){
        
        this->owner->addCollectedArtifact();
        activeBuffDuration = artifact->getDuration();
        int force = artifact->getForce();

        switch (artifact->getEffect()){

        case Effect::SPEED_BOOST:
            this->currentSpeed = baseSpeed + force;
            break;

        case Effect::CAPACITY_BOOST :
            this->currentCapacity = baseCapacity + force;
            break;

        case Effect::VISION_BOOST :
            this->baseVisionRadius = baseVisionRadius + force;
            break;
        }
    }
}

void Unit::defeat() {
    
    this->isActive = false;
    this->carriedResources = 0;
    if (this->owner != nullptr) {
        this->set_X(this->owner->get_X());
        this->set_Y(this->owner->get_Y());
    }
    this->backToLifeCooldown = 40; 
}

void Unit::move(Map* map, int targetX, int targetY) {
    int current_X = this->get_X();
    int current_Y = this->get_Y();

    for (int step = 0; step < this->currentSpeed; ){

        if (-1 == targetX && -1 == targetY){
            break;
        }
        
        if (current_X == targetX && current_Y == targetY){
            break;
        }
        
        if (current_X < targetX){
            current_X++;
            step++;
        }
        else if (current_X > targetX) {
            current_X--;
            step++;
        };

        if (current_Y < targetY){
            current_Y++;
            step++;
        }
        else if (current_Y > targetY) {
            current_Y--;
            step++;
        };

        GameObject* objectOnTile = map->getTileContent(current_X, current_Y);
        if (objectOnTile != nullptr) {
            Artifact* foundArtifact = dynamic_cast<Artifact*>(objectOnTile);
            if (foundArtifact != nullptr) {
                this->pickUpArtifact(foundArtifact);
                map->remove_Entity(foundArtifact);
                delete foundArtifact;
            }
        }
    };

    this->set_X(current_X); 
    this->set_Y(current_Y);
}



