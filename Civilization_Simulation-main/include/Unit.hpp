#pragma once

#include "GameObject.hpp"
#include "Artifact.hpp"
#include "Map.hpp"

class Civilization;

class Unit : public GameObject {
protected:

    int cooldownTime;

    int baseSpeed;

    int currentSpeed;

    int baseCapacity;

    int currentCapacity;

    int carriedResources;

    int activeBuffDuration;
    
    int baseVisionRadius;

    int currentVisionRadius;

    int backToLifeCooldown;

    Civilization* owner;

public:

    using GameObject::GameObject;

    void update() override;

    void move(Map* map, int targetX, int targetY);

    void pickUpArtifact(Artifact* artifact);

    void defeat();

    virtual ~Unit();

    virtual void performAction(Map* map) = 0;
    
    int getCarriedResources() const { return carriedResources; }
    
    Civilization* getOwner() const { return owner; }
};