
#include "..\include\Map.hpp"
#include "..\include\Resource.hpp"
#include "..\include\Simulation.hpp"
#include "..\include\GameObject.hpp"

#include <iostream>
#include <algorithm>
#include <random>


using namespace std;


void Map::generateMap(){
    //rozmiar mapy
    size = 100;
}

void Map::addEntity(GameObject* object){
    //dodawanie do vektora objektów
    entities.push_back(object);
}

void Map::remove_Entity(GameObject* object){
    if(object == nullptr) return;

    //idiom erase-remove do usuwania elementu vektora
    auto it = std::remove(entities.begin(), entities.end(), object);
    if (it != entities.end()){
        entities.erase(it,entities.end());
    }
}

GameObject* Map::getTileContent(int x, int y, GameObject* ignore){
    for(GameObject* object : entities){
        // Sprawdzamy czy obiekt to nie jest ten, który chcemy zignorować
        if(object != ignore && object->get_X() == x && object->get_Y() == y){
            return object;
        }
    }
    return nullptr;
}

std::vector<GameObject*> Map::getObjectInGrid(int centerX, int centerY, int radius){
    std::vector<GameObject*> foundObjectPosition;
    //przszukujemy vektor i obliczamy rużnice miedzy obiektami
    for(GameObject* object : entities){
        if(object != nullptr){
            int difference_X = abs(object->get_X()- centerX);
            int difference_Y = abs(object->get_Y()- centerY);
            
            //jeśli rużnica jestrówna lub mniejsza zasięgowi wzroku to 
            //jest obiekt dodawany do vektora przetrzymujacego pozycje 
            if(difference_X <= radius && difference_Y <= radius){
                foundObjectPosition.push_back(object);
            }
        }
    }

    return foundObjectPosition;
}
