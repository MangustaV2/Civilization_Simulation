#pragma once

#include "GameObject.hpp"

class DroppedResource : public GameObject {
private:

    int amount;

public:
    DroppedResource(int x, int y, int amount) : GameObject(x, y), amount(amount) {}

    using GameObject::GameObject;
    
    void update() override;

    void reduceAmount(int qty);// Funkcja do redukcji ilości surowca (np. gdy zbieracz zbiera surowce)

    int extract(int requestedAmount);// Funkcja do pobierania surowców z upuszczonego zasobu

    // Funkcja do pobierania aktualnej ilości surowca w upuszczonym zasobie
    int getAmount() const{
        return amount;
    };
};
