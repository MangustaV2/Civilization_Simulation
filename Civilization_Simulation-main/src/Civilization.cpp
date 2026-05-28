#include "../include/Civilization.hpp"
#include "../include/Gatherer.hpp"
#include "../include/Scout.hpp"
#include "../include/Warrior.hpp"
#include "../include/Simulation.hpp"
#include <algorithm>
#include <iostream>

void Civilization::setBasePosition(int x, int y) {
    this->baseX = x;
    this->baseY = y;
    
    // Baza zostaje postawiona, respią się jednostki
    units.push_back(new Gatherer(baseX, baseY, this));
    units.push_back(new Scout(baseX, baseY, this));
    units.push_back(new Warrior(baseX, baseY, this));
}

void Civilization::reportResources(int x, int y){

    std::pair<int, int> newResource = {x, y};
    
    if (std::find(knownResources.begin(),knownResources.end(),newResource) == knownResources.end()){
        knownResources.push_back(newResource);

    }
}

void Civilization::dispatchGatherer(){

    for (auto& unit : units){
        if (knownResources.empty())break;
        
        Gatherer* gatherer = dynamic_cast<Gatherer*>(unit);
        Warrior* warrior = dynamic_cast<Warrior*>(unit);
        if (gatherer && gatherer->get_is_Waiting()){
            
            std::pair<int, int> resourceCoords = knownResources.back();
            gatherer->assignTargetResource(resourceCoords.first, resourceCoords.second); 
            knownResources.pop_back();
            break; 
        }
    }
}

// Oddanie surowców
void Civilization::addResource(int amount) {
    if (amount > 0) {
        storedResources += amount;
    }
}

// Przegrana jednostki (powrót do bazy na 5 tur)
void Civilization::unitDefeated(Unit* defeatedUnit) {
    if (defeatedUnit != nullptr) {

        defeatedUnit->set_X(baseX);
        defeatedUnit->set_Y(baseY);
        
        std::cout << "Jednostka cywilizacji " << name << " wraca do bazy na odpoczynek!\n";
    }
}

// Budowanie miasta i benefity
bool Civilization::buildCity(Simulation* sim, Map* map) {

    int currentBuildings = buildings.size();
    
    if (currentBuildings >= 3) {
        return false;
    }
 
    int currentCost = (currentBuildings + 1) * 5;

    if (storedResources >= currentCost) {
        storedResources -= currentCost;

        buildings.push_back(new Building(currentCost, Benefit::INCREASE_ARMY));

        //tworzernie nowe jednostki w pamięci
        Scout* newScout = new Scout(baseX, baseY, this);
        Gatherer* newGatherer = new Gatherer(baseX, baseY, this);
        Warrior* newWarrior = new Warrior(baseX, baseY, this);

        units.push_back(newScout);
        units.push_back(newGatherer);
        units.push_back(newWarrior);

        //zgłaszanie nowo powstałe jednostki do mapy
        if (map != nullptr) {
            map->addEntity(newScout);
            map->addEntity(newGatherer);
            map->addEntity(newWarrior);
        }

        // nowa budowa
        if (sim != nullptr) {
            buildingTurns.push_back(sim->getTurnCounter()); 
        }

        return true;
    }
}

void Civilization::playTurn(Map* map, Simulation* sim, const std::vector<Civilization*>& allCivs) {
    
    
    {
        std::lock_guard<std::recursive_mutex> lock(map->getMutex());
        dispatchGatherer(); 
        buildCity(sim,map); 
    }

   
    std::vector<Unit*> unitsToUpdate;
    {
        std::lock_guard<std::recursive_mutex> lock(map->getMutex());
        unitsToUpdate = units;
    }

    //  pętla wykonująca akcje dla każdej jednostki z kopii
    for (Unit* unit : unitsToUpdate) {
        
        // sprawdzenike czy jednostka nadal istnieje i jest aktywna w grze
        if (unit == nullptr || !unit->get_isActive()) {
            continue;
        }

        // blok mapę na czas ruchu i potencjalnej walki jednostki
        std::lock_guard<std::recursive_mutex> lock(map->getMutex());
        
        // sprawdzenie po założeniu locka (wątki mogły coś zmienić w międzyczasie)
        if (!unit->get_isActive()) {
            continue;
        }
            
        unit->performAction(map); // Ruch jednostki

        // faza walki
        for (Civilization* otherCiv : allCivs) {
            if (otherCiv == this || otherCiv == nullptr) continue;

            // pobieramy kopię jednostek wroga, żeby nie uszkodzić pętli przeciwnika
            std::vector<Unit*> enemyUnits = otherCiv->getUnits();

            for (Unit* otherUnit : enemyUnits) {
                if (otherUnit == nullptr || !otherUnit->get_isActive()) continue;

                // sprawdzenie czy jednostki stoją na tym samym polu
                if (otherUnit->get_X() == unit->get_X() && otherUnit->get_Y() == unit->get_Y()) {
                    
                    // wywołanie rozstrzygnięcia walki
                    sim->resolveCombat(unit, otherUnit); 

                    // KLUCZOWE: Jeśli nasza jednostka poległa w tej walce, przerywamy jej turę!
                    if (unit == nullptr || !unit->get_isActive()) {
                        break; 
                    }
                }
            }
            
            // Jeśli nasza jednostka nie żyje, nie sprawdzamy już kolizji z kolejnymi cywilizacjami
            if (unit == nullptr || !unit->get_isActive()) {
                break; 
            }
        }
    }
}