#include "../include/Artifact.hpp"
#include <iostream>
#include <random>

Artifact::Artifact(int x, int y) : GameObject(x, y){
    std::vector<std::string> names = {"Escarlata_Medallion","Blessed_Talisman","White_Rabbit_Talisman","Soul_of_Alexander","Magic_Dragon_Charm","Winged_Pickaxe_Insignia"}; 
    this->name = name[rand() % names.size()];

    std::vector<Effect> possibleEffects = {Effect::SPEED_BOOST, Effect::CAPACITY_BOOST, Effect::VISION_BOOST};
    this->effect.type = possibleEffects[rand() % possibleEffects.size()];

    if (effect.type == Effect::VISION_BOOST)
    {
        this->effect.force = 1;
    }else{
        this->effect.force = (rand() % 3) + 1;
    }
    
    this->effect.duration = (rand() % 10) + 5;
}

void Artifact::update() {}