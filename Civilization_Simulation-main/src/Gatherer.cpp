#include "..\include\Gatherer.hpp"
#include "..\include\Unit.hpp"
#include "..\include\Civilization.hpp"
#include "..\include\Resource.hpp"
#include "..\include\DroppedResource.hpp"
#include "..\include\Map.hpp"
#include "..\include\Simulation.hpp"
#include <iostream>
#include <algorithm>


void Gatherer::performAction(Map* map, Simulation* sim){

    if (map == nullptr || !isActive) return;

    if (this->currentState == State::WaitingInBase) {
        if (!this->isWaiting) {
            this->currentState = State::GoingToResource; // Cywilizacja przypisała nam zasób
        }
    }

    if (this->currentState == State::GoingToResource) {
        // przejdzie max ilość kroków w stronę surowca
        this->move(map, targetX, targetY); 

        // Sprawdzamy, czy po wykonaniu ruchu stanęliśmy dokładnie na surowcu
        if (this->get_X() == this->targetX && this->get_Y() == this->targetY) {
            this->currentState = State::Mining; 
        }
    }
    
    else if (this->currentState == State::Mining) {
        if (!isWaiting && this->get_X() == this->targetX && this->get_Y() == this->targetY) {
            GameObject* tileContent = map->getTileContent(this->targetX, this->targetY, this);

            if (tileContent == nullptr || !tileContent->get_isActive()) {
                // Wraca do bazy jak wykopie
                targetX = this->owner->get_X();
                targetY = this->owner->get_Y();
                this->currentState = State::ReturningToBase;
                return;
            }

            // Przypadek 1: Kopanie (1 tura - 1 surowiec)
            Resource* regularResource = dynamic_cast<Resource*>(tileContent);
            if (regularResource != nullptr) {
                if (carriedResources < currentCapacity && regularResource->getAmount() > 0) {
                    regularResource->reduceAmount(1);
                    carriedResources++;
                }

                // Jeśli plecak pełny LUB złoże zniknęło -> wracamy
                if (carriedResources >= currentCapacity || regularResource->getAmount() <= 0) {
                    targetX = this->owner->get_X();
                    targetY = this->owner->get_Y();
                    this->currentState = State::ReturningToBase;
                }
                return;
            }

            // Przypadek 2: Podnoszenie (DroppedResource)
            DroppedResource* droppedResource = dynamic_cast<DroppedResource*>(tileContent);
            if (droppedResource != nullptr) {
                int spaceLeft = currentCapacity - carriedResources;
                int toTake = std::min(spaceLeft, droppedResource->getAmount());

                if (toTake > 0) {
                    droppedResource->reduceAmount(toTake);
                    carriedResources += toTake;
                }

                // Zawsze po podniesieniu powrót do bazy
                targetX = this->owner->get_X();
                targetY = this->owner->get_Y();
                this->currentState = State::ReturningToBase;
                return;
            }
        }
    }   

    else if (this->currentState == State::ReturningToBase)
    {
        // Wywołujemy funkcję - przejdzie max ilość kroków w stronę bazy
        this->move(map, targetX, targetY); 

        // Sprawdzamy, czy po wykonaniu ruchu stanęliśmy dokładnie na bazie
        if (this->get_X() == this->targetX && this->get_Y() == this->targetY) {
            this->currentState = State::ResourceDischarge; 
        }
    }


    if (this->currentState == State::ResourceDischarge) {
        //Jesteśmy w bazie z pełnym plecakiem
        if (this->get_X() == this->owner->get_X() && this->get_Y() == this->owner->get_Y()) {
        if (carriedResources > 0) {
            owner->addResource(carriedResources);
            sim->addLog("[ZBIORY] Zbieracz (" + this->owner->getName() + ") zrzucil " + std::to_string(carriedResources) + " surowcow.");
            carriedResources = 0;
            isWaiting = true; 
            targetX = -1;
            targetY = -1;
            this->currentState = State::WaitingInBase;
        }
        return; 
        }
    }
}

