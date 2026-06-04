#include "../include/Scout.hpp"
#include "../include/Resource.hpp"
#include "../include/DroppedResource.hpp"
#include "../include/Civilization.hpp"
#include "../include/Map.hpp"
#include <random>
#include <vector>

void Scout::performAction(Map* map, Simulation* sim) {
    if (map == nullptr || owner == nullptr) return;

    bool hasSpecificTarget = false;
    int targetX = this->get_X();
    int targetY = this->get_Y();

    // SKANOWANIA OTOCZENIA 
    std::vector<GameObject*> objectsInSight = map->getObjectInGrid(this->get_X(), this->get_Y(), this->currentVisionRadius);
    
    for (GameObject* obj : objectsInSight) {
        if (obj == nullptr || !obj->get_isActive()) continue;

        // Zgłaszanie surowców cywilizacji (podstawowe zadanie zwiadowcy)
        Resource* naturalResource = dynamic_cast<Resource*>(obj);
        if (naturalResource != nullptr) {
            owner->reportResources(naturalResource->get_X(), naturalResource->get_Y());
            continue;
        }

        // Szukanie artefaktów do zebrania
        Artifact* artifact = dynamic_cast<Artifact*>(obj);
        if (artifact != nullptr && !hasSpecificTarget) {
            // Znaleźliśmy artefakt! Ustawiamy go jako priorytetowy cel.
            targetX = artifact->get_X();
            targetY = artifact->get_Y();
            hasSpecificTarget = true; 
        }
    }

    // DECYZJA RUCHU
    if (!hasSpecificTarget) {
        // Jeśli nie widzimy artefaktu, wędrujemy losowo 
        static thread_local std::mt19937 generator(std::random_device{}());
        
        int minOffSet = -this->currentSpeed; 
        int maxOffSet = this->currentSpeed; 
        
        std::uniform_int_distribution<int> distribution(minOffSet, maxOffSet);

        targetX = this->get_X() + distribution(generator);
        targetY = this->get_Y() + distribution(generator);

        // Zabezpieczenie przed wyjściem poza granice mapy
        if (targetX < 0) targetX = 0;
        if (targetX > 99) targetX = 99;
        if (targetY < 0) targetY = 0;
        if (targetY > 99) targetY = 99;
    }

    // WYKONANIE RUCHU
    // Zwiadowca idzie w stronę artefaktu lub w losowe miejsce
    this->move(map, targetX, targetY);

    // PODNOSZENIE (Po wykonaniu ruchu)
    std::vector<GameObject*> objectsOnTile = map->getObjectInGrid(this->get_X(), this->get_Y(), 0);
    
    for (GameObject* obj : objectsOnTile) {
        if (obj == nullptr || !obj->get_isActive()) continue;

        Artifact* foundArtifact = dynamic_cast<Artifact*>(obj);
        if (foundArtifact != nullptr) {
            
            this->pickUpArtifact(foundArtifact); 
            map->remove_Entity(foundArtifact); 
            break; 
        }
    }
}