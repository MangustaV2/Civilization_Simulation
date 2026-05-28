#include "../include/Artifact.hpp"
#include "../include/GameState.hpp"
#include "../include/Map.hpp"
#include "../include/Civilization.hpp"
#include "../include/Unit.hpp"
#include "../include/Simulation.hpp"
#include "../include/Resource.hpp"
#include "../include/DroppedResource.hpp"
#include "../include/Gatherer.hpp"
#include "../include/Scout.hpp"
#include "../include/Warrior.hpp"
#include "../include/Renderer.hpp"

#include <fstream>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <future>

using namespace std;

Simulation::Simulation() {
    currentState = GameState::MENU;
    turnCounter = 0;

    resourceSpawnInterval = 5;
    artifactSpawnInterval = 10;

    // Tworzenie 4 startowych cywilizacji
    civilizations.push_back(new Civilization("Rzym"));
    civilizations.push_back(new Civilization("Grecja"));
    civilizations.push_back(new Civilization("Egipt"));
    civilizations.push_back(new Civilization("Chiny"));
}

void Simulation::run() {
    cout << "=== ROZPOCZYNAMY SYMULACJE CYWILIZACJI! ===" << endl;
    map.generateMap();
    cout << "--- TRWA ROZSTAWIANIE BAZ I JEDNOSTEK CYWILIZACJI ---" << endl;

    for (Civilization*civ : civilizations) {
        // Losowanie baz cywilizacji
        generateRandom(SpawnType::CIVILIZATION_BASE, civ);

        for (Unit* startUnit : civ->getUnits()) {
            if (startUnit != nullptr) {
                map.addEntity(startUnit);
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        generateRandom(SpawnType::RESOURCE);
    }
    for(int i = 0; i < 4; i++) {
        generateRandom(SpawnType::ARTIFACT);
    }

    // Tworzenie obiektu Renderera
    Renderer renderer(1920, 1080, this);

    // Główna pętla renderowania (działa dopóki okno SFML nie zostanie zamknięte)
    while (renderer.isWindowOpen()) {
        
        // Renderowanie klatki
        renderer.renderFrame();

        if (currentState == GameState::SIMULATION) {
            
            std::lock_guard<std::recursive_mutex> lock(map.getMutex());
            turnCounter++;

            // Resp surowców co ilość wpisaną z inputa
            if (resourceSpawnInterval > 0 &&turnCounter % resourceSpawnInterval == 0) {
                generateRandom(SpawnType::RESOURCE);
            }

            // Resp artefaktów co ilość wpisaną z inputa
            if (artifactSpawnInterval > 0 && turnCounter % artifactSpawnInterval == 0) {
                generateRandom(SpawnType::ARTIFACT);
            }

            for (Civilization* civ : civilizations) {
                civ->playTurn(&map, this, civilizations);
            }

            for(GameObject* obj : map.entities) {
                if (obj != nullptr && obj->get_isActive()) {
                    obj->update();
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(125));

            //Warunek zwycięstwa
            for (Civilization* civ : civilizations) {
                // Czy wybudowali 3 budynki
                if (civ->getBuildingsCount() >= 3) {
                    winner = civ;
                    currentState = GameState::GAME_OVER;
                    saveToCSV();
                    break;
                }
            }
        }
        if (turnCounter >= 500 && currentState != GameState::GAME_OVER) {
            currentState = GameState::GAME_OVER;
            saveToCSV(); 
            break;
        }
    }
}

/* void Simulation::run() {

        // B. Ruch i Akcje każdej cywilizacji
        std::vector<std::future<void>> turnFutures;

        for (Civilization* civ : civilizations) {
            // std::async uruchamia metodę playTurn w osobnym wątku systemowym
            turnFutures.push_back(std::async(std::launch::async, &Civilization::playTurn, civ, &map, this, std::ref(civilizations)));
        }

        // Oczekiwanie synchronizacyjne - główny wątek czeka, aż wszystkie 4 podwątki skończą tę turę
        for (auto& future : turnFutures) {
            future.get();
        }
}
*/

void Simulation::generateRandom(SpawnType type, Civilization* civ) {
    srand(time(0));
    int attempts = 0;
    int maxAttempts = 500;
    
    while (attempts < maxAttempts) {
        int rx = rand() % (map.size > 1 ? map.size - 1 : 1);
        int ry = rand() % (map.size > 1 ? map.size - 1 : 1);

        GameObject* is_available = map.getTileContent(rx, ry);
        
        // Sprawdzamy czy pole jest puste
        if (is_available == nullptr) {
            
            if (type == SpawnType::RESOURCE) {
                // Losujemy złoże surowca wielkości 2 do 6
                map.addEntity(new Resource(rx, ry, (rand() % 5) + 2));
                break;
            } 
            else if (type == SpawnType::ARTIFACT) {
                // Losujemy artefakt
                map.addEntity(new Artifact(rx, ry));
                break;
            } 
            else if (type == SpawnType::CIVILIZATION_BASE && civ != nullptr) {
                // Szukamy miejsca na bazę sprawdzając, czy nie jest za blisko innych
                bool tooClose = false;
                int minimumDistance = 30; //losowo, potem się zmieni

                for (Civilization* otherCiv : civilizations) {
                    if (otherCiv != civ) {
                        // Obliczamy pitagorasem
                        double distance = std::sqrt(std::pow(otherCiv->get_X() - rx, 2) + std::pow(otherCiv->get_Y() - ry, 2));
                        
                        // Jeśli odległość jest mniejsza niż wymagana, odrzucamy te koordynaty
                        if (distance < minimumDistance) {
                            tooClose = true;
                            break;
                        }
                    }
                }

                // Jeśli zachowaliśmy odpowiedni dystans, stawiamy bazę
                if (!tooClose) {
                    civ->setBasePosition(rx, ry);
                    std::cout << "[Silnik] Rozmieszczono baze cywilizacji na kordynatach: (" << rx << ", " << ry << ")\\n";
                    break;
                }
            }
        }
        attempts++;
    }
}

void Simulation::saveToCSV() {
    std::ofstream file("Wyniki_Symulacji.csv");

    if (!file.is_open()) {
        std::cerr << "Nie udalo sie otworzyc pliku do zapisu!\n";
        return;
    }
    file << "Nazwa Cywilizacji;Zgromadzone Surowce;Zgromadzone Artefakty;Ilosc Budynkow;Tura I budynku;Tura II budynku;Tura III budynku;Okres przetrwania\n";

    for (Civilization* civ : civilizations) {
        if (civ == nullptr) continue;

        //okresu przerwania
        std::string survivalTime;
        if (civ->getDeathTurn() == -1) {
            survivalTime = std::to_string(turnCounter) + " (Przetrwal)";
        } else {
            survivalTime = std::to_string(civ->getDeathTurn()) + " (Zniszczony)";
        }

        //tur budowy
        int t1 = civ->getBuildingTurn(0);
        int t2 = civ->getBuildingTurn(1);
        int t3 = civ->getBuildingTurn(2);

        std::string b1 = (t1 > 0) ? std::to_string(t1) : "-";
        std::string b2 = (t2 > 0) ? std::to_string(t2) : "-";
        std::string b3 = (t3 > 0) ? std::to_string(t3) : "-";

        file << civ->getName() << ";"<< civ->getStoredResources()<< ";"<<civ->getCollectedArtifacts() << ";"<< civ->getBuildingsCount() << ";"<< b1 << ";"<< b2 << ";"<< b3 << ";"<< survivalTime << "\n";
    }

    file.close();
}
    
void Simulation::resolveCombat(Unit* u1, Unit* u2) {
    // Sprawdzamy typy obu jednostek (Gatherer - 0, Scout - 1, Warrior - 2)
    int type1 = -1, type2 = -1;

    if (dynamic_cast<Gatherer*>(u1)) type1 = 0; 

    else if (dynamic_cast<Scout*>(u1)) type1 = 1;

    else if (dynamic_cast<Warrior*>(u1)) type1 = 2;

    if (dynamic_cast<Gatherer*>(u2)) type2 = 0;

    else if (dynamic_cast<Scout*>(u2)) type2 = 1;

    else if (dynamic_cast<Warrior*>(u2)) type2 = 2;

    // Szanse na wygraną dla jednostki u1 (w procentach)
    int winChanceU1 = 50; // domyślnie 50%

    if      (type1 == 0 && type2 == 0) winChanceU1 = 50; // G vs G

    else if (type1 == 0 && type2 == 1) winChanceU1 = 40; // G vs S

    else if (type1 == 0 && type2 == 2) winChanceU1 = 25; // G vs W
    
    else if (type1 == 1 && type2 == 0) winChanceU1 = 60; // S vs G

    else if (type1 == 1 && type2 == 1) winChanceU1 = 50; // S vs S

    else if (type1 == 1 && type2 == 2) winChanceU1 = 30; // S vs W
    
    else if (type1 == 2 && type2 == 0) winChanceU1 = 75; // W vs G

    else if (type1 == 2 && type2 == 1) winChanceU1 = 70; // W vs S

    else if (type1 == 2 && type2 == 2) winChanceU1 = 50; // W vs W

    // Rzut kością (losujemy liczbę 0-99)
    int roll = rand() % 100;
    
    Unit* winner = nullptr;
    Unit* loser = nullptr;

    // Jeśli szansa to np. 40% to roll musi wylosować od 0 do 39 aby u1 wygrało
    if (roll < winChanceU1) {
        winner = u1;
        loser = u2;
    } else {
        winner = u2;
        loser = u1;
    }

    cout << "\n=== DOSZLO DO WALKI! ===\n";
    cout << "Szansa Jednostki 1 na wygrana: " << winChanceU1 << "%\n";
    cout << "Wylosowano wartosc: " << roll << " -> Wygrywa: " 
              << (winner == u1 ? "Jednostka 1" : "Jednostka 2") << "!\n";

    // Zrzucanie surowców
    int droppedAmount = loser->getCarriedResources();
    if (droppedAmount > 0) {
        DroppedResource* drop = new DroppedResource(loser->get_X(), loser->get_Y(), droppedAmount);
        map.addEntity(drop);
    }

    if (loser->getOwner() != nullptr) {
        loser->getOwner()->unitDefeated(loser); // Przesunięcie pozycji bazy i komunikat
    }
    loser->defeat(); // Faktyczna dezaktywacja jednostki i ustawienie cooldownu 40 tur
    
    map.remove_Entity(loser);
}

Simulation::~Simulation() {
    // Usuwanie cywilizacji
    for (Civilization* civ : civilizations) {
        delete civ;
    }
    civilizations.clear();

    // Usuwanie obiektów z mapy
    for (GameObject* obj : map.entities) {
        delete obj;
    }
    map.entities.clear();
}



