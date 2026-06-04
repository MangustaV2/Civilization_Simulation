#pragma once

#include <vector>
#include <mutex>
#include "GameObject.hpp"

class GameObject;
class Map {
public:

    int size;

    std::vector<GameObject*> entities;

    std::recursive_mutex mapMutex;

public:

    void generateMap();//rozmieszczenie startowe

    void addEntity(GameObject* obj);//dodawanie obiekyu do vektora

    void remove_Entity(GameObject* object);//usuwanie obiektu z vektora

    GameObject* getTileContent(int x, int y, GameObject* ignore = nullptr);//sprawdzanie konkretnego pola
    
    std::vector<GameObject*> getObjectInGrid(int centerX, int centerY, int radius);//skanowanie FoV

    std::recursive_mutex& getMutex() { return mapMutex; }
};
