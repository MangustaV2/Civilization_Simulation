#pragma once

#include <vector>
#include "GameState.hpp"
#include "Map.hpp"
#include <iostream>

class Civilization;
class Unit;

enum class SpawnType {
    RESOURCE,
    ARTIFACT,
    CIVILIZATION_BASE
};

class Simulation {
private:

    GameState currentState;

    int turnCounter;

    Map map;

    std::vector<Civilization*> civilizations;

    float resourceSpawnRate;

    Civilization* winner = nullptr;

    int resourceSpawnInterval;
    int artifactSpawnInterval;

public:

    virtual ~Simulation();

    Simulation();

    void run();

    void generateRandom(SpawnType type, Civilization* civ = nullptr);

    void saveToCSV();
    
    void resolveCombat(Unit* u1, Unit* u2);

    Map* getMap() { return &map; }

    const std::vector<Civilization*>& getCivilizations() const { return civilizations; }

    GameState getCurrentState() const { return currentState; }
    void setCurrentState(GameState state) { currentState = state; };
    Civilization* getWinner() const { return winner; }

    int getTurnCounter() const { return turnCounter; }
    
    void setResourceSpawnInterval(int val) { resourceSpawnInterval = val; }
    void setArtifactSpawnInterval(int val) { artifactSpawnInterval = val; }
};