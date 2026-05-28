#pragma once

#include "IUpdatable.hpp"
#include "Map.hpp"

class GameObject : public IUpdatable {
protected:

    int posX;

    int posY;

    bool isActive;

public:

    GameObject(int x, int y) : posX(x), posY(y), isActive(true){}

    int get_X() const{return posX; };
    int get_Y() const{return posY;};

    void set_X(int new_X){posX = new_X;};
    void set_Y(int new_Y){posY = new_Y;};

    bool get_isActive()const{return isActive;};
    void disactive(){isActive = false;};

    virtual ~GameObject(){}
    
    virtual void update() = 0;
};
