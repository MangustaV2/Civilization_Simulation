#pragma once

#include "GameObject.hpp"

class DroppedResource : public GameObject {
private:

    int amount;

public:
    DroppedResource(int x, int y, int amount) : GameObject(x, y), amount(amount) {}

    using GameObject::GameObject;
    
    void update() override;

    int getAmount() const{
        return amount;
    };

    void reduceAmount(int qty);

    int extract(int requestedAmount);
};
