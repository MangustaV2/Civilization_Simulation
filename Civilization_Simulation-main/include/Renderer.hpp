#pragma once

#include <string>
#include <SFML/Graphics.hpp>
class Simulation; 

struct InputField {
    sf::RectangleShape box;
    sf::Text label;
    sf::Text textDisplay;
    std::string value;
    bool isSelected;
    int minVal;
    int maxVal;
};

class Renderer {
private:
    sf::RenderWindow* window;
    int windowWidth;
    int windowHeight;
    
    sf::Font font;
    Simulation* sim;

    std::vector<InputField> inputs;
    void initInputFields();

public:
    Renderer(int width, int height, Simulation* simulation);
    ~Renderer();

    bool isWindowOpen() const { return window && window->isOpen(); }

    void renderFrame();
    void drawMenu();
    void drawGameOver();
    void drawMap();
    void drawTile(int x, int y, std::string text, sf::Color color);
    void drawEntities();
    void drawUI();
};