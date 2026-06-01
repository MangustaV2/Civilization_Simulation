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

    int tickDelayMs = 250;

    std::vector<std::string> eventLogs;
    std::mutex logMutex;

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

    int getTickDelay() const { return tickDelayMs; }
    void setTickDelay(int ms) { tickDelayMs = ms; }

    void inputFromCSV(const std::string& filename);

    void addLog(const std::string& message) {
        std::lock_guard<std::mutex> lock(logMutex);
        eventLogs.push_back(message);
        if (eventLogs.size() > 15) {
            eventLogs.erase(eventLogs.begin());
        }
    }

    std::vector<std::string> getLogsCopy() {
        std::lock_guard<std::mutex> lock(logMutex);
        return eventLogs;
    }
};