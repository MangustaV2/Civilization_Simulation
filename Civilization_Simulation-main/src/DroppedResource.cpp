#include "../include/DroppedResource.hpp"
#include <iostream>


void DroppedResource::update() {
    if(amount<=0){
        isActive = false;
    }
}

int DroppedResource::extract(int requestedAmount) {
    if (amount >= requestedAmount) {
        amount -= requestedAmount;
        return requestedAmount;
    } else {
        int left = amount;
        amount = 0;
        return left;
    }
}

void DroppedResource::reduceAmount(int qty) {
    this->amount -= qty;
    if (this->amount < 0) {
        this->amount = 0;
    }
}