#pragma once

#include "GameObject.hpp"

class Resource : public GameObject {
protected:

    int amount;

public:
    Resource(int x, int y, int amount) : GameObject(x, y), amount(amount){}

    using GameObject::GameObject;
    
    void update() override;

    int getAmount() const{return amount;};

    void reduceAmount(int qty);
};
