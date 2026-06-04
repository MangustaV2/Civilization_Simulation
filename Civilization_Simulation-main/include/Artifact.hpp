#pragma once

#include <string>
#include "GameObject.hpp"
#include <vector>

enum class Effect {SPEED_BOOST, CAPACITY_BOOST, VISION_BOOST};

struct Effect_info
{
    Effect type;
    int force;
    int duration;
};


class Artifact : public GameObject {
private:

    std::string name;

    Effect_info effect;

    
public:
    Artifact(int x, int y);

    using GameObject::GameObject;

    void update() override;

    // Funkcje do pobierania informacji o efekcie artefaktu
    Effect getEffect() const{
        return effect.type;
    };

    int getDuration() const{
        return effect.duration;
    };

    int getForce() const{
        return effect.force;
    };
};

