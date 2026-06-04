
#include "..\include\Map.hpp"
#include "..\include\Resource.hpp"
#include "..\include\Simulation.hpp"
#include "..\include\GameObject.hpp"

#include <iostream>
#include <algorithm>
#include <random>


using namespace std;

//rozmiar mapy
void Map::generateMap(){
    size = 100;
}

//dodawanie do vektora objektów
void Map::addEntity(GameObject* object){
    entities.push_back(object);
}

//usuwanie objektów z vektora
void Map::remove_Entity(GameObject* object){
    if(object == nullptr) return;

    //idiom erase-remove do usuwania elementu vektora
    auto it = std::remove(entities.begin(), entities.end(), object);
    if (it != entities.end()){
        entities.erase(it,entities.end());
    }
}

//sprawdzanie konkretnego pola
GameObject* Map::getTileContent(int x, int y, GameObject* ignore){
    for(GameObject* object : entities){
        // Sprawdzamy czy obiekt to nie jest ten, który chcemy zignorować
        if(object != ignore && object->get_X() == x && object->get_Y() == y){
            return object;
        }
    }
    return nullptr;
}

//skanowanie pola w promieniu wzroku
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
