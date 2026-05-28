#include "../include/Resource.hpp"
#include <iostream>

void Resource::update(){
    if(amount<=0){
        isActive = false;
    }
}

void Resource::reduceAmount(int qty) {
    this->amount -= qty;
    if (this->amount < 0) {
        this->amount = 0;
    }
}