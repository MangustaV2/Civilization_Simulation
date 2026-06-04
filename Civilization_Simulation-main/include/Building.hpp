#pragma once

enum class Benefit { INCREASE_ARMY };

class Building {
private:
    int cost;
    Benefit benefit;

public:
    Building(int cost, Benefit benefit) : cost(cost), benefit(benefit) {}

    // Funkcje do pobierania informacji o budynku
    int getCost() const { return cost; }
    Benefit getBenefit() const { return benefit; }
};
