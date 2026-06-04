#pragma once

#include <vector>
#include <utility>
#include "Building.hpp"
#include "Unit.hpp"
#include <string>

class Simulation;

class Civilization {
private:

    std::string name;

    int baseX;

    int baseY;

    int population;

    int turn;

    int storedResources;

    int resourcePowerCounter;

    int collectedArtifacts;

    std::vector<Building*> buildings;

    std::vector<Unit*> units;

    std::vector<std::pair<int, int>> knownResources;

    std::vector<int> buildingTurns;

    int deathTurn = -1;


public:

    Civilization(std::string civName) {
        this->name = civName;
        this->storedResources = 0;
        this->resourcePowerCounter = 0;
        this->population = 0;
        this->turn = 0;
        this->collectedArtifacts = 0;
        this->knownResources = {};
    }

    void reportResources(int x, int y);//odbiur kordynatów od zwiadowcy/wojownika

    void dispatchGatherer();//wysłanie wolnego zbieracza

    void addResource(int amount); // zrzut surowców do skarbca

    void unitDefeated(Unit* defeatedUnit); // jednostka przegrywa walkę

    bool buildCity(Simulation* sim, Map* map); // budowanie budynku

    int getBuildingsCount() const {return buildings.size();}//Sprawdzanie wygranej

    void playTurn(Map* map, Simulation* sim, const std::vector<Civilization*>& allCivs);//tura cywilizacji

    void setBasePosition(int x, int y);//ustawienie pozycji bazy cywilizacji

    
    // Funkcje do pobierania informacji o cywilizacji

    std::string getName() const { return name; }
    
    void setDeathTurn(int turn) {if (deathTurn == -1) deathTurn = turn;}

    int getBuildingTurn(size_t index) const {
        if (index < buildingTurns.size()) {
            return buildingTurns[index];
        }
        return 0; // Zwraca 0 jeśli budynku jeszcze nie ma
    }

    void addCollectedArtifact() { collectedArtifacts++; }

    int getCollectedArtifacts() const { return collectedArtifacts; }

    int get_X() const{return baseX; }

    int get_Y() const{return baseY; }

    int getStoredResources() const { return storedResources; }
    
    const std::vector<Unit*>& getUnits(){ return units; }

    const std::vector<Unit*>& getUnits() const { return units; }

    int getDeathTurn() const { return deathTurn; }
};