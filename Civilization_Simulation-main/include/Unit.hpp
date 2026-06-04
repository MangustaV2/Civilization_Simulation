#pragma once

#include "GameObject.hpp"
#include "Artifact.hpp"
#include "Map.hpp"

class Civilization;
class Simulation;

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

    void move(Map* map, int targetX, int targetY);// Ruch jednostki w stronę docelowych koordynatów

    void pickUpArtifact(Artifact* artifact);// Funkcja do podnoszenia artefaktów przez jednostkę

    void defeat();// Funkcja wywoływana, gdy jednostka zostaje pokonana (np. w walce)

    virtual ~Unit();

    virtual void performAction(Map* map, Simulation* sim) = 0;//logika działania jednostki (zależna od typu)
    
    int getCarriedResources() const { return carriedResources; }// Funkcja do pobierania ilości nośnych surowców
    
    Civilization* getOwner() const { return owner; }// Funkcja do pobierania właściciela jednostki
};