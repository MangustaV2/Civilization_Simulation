#pragma once

#include <string>
#include <SFML/Graphics.hpp>

class Simulation; 

class Renderer {
private:
    sf::RenderWindow* window;
    int windowWidth;
    int windowHeight;
    
    sf::Font font;
    Simulation* sim;

    sf::RectangleShape sliderTrack;
    sf::RectangleShape sliderHandle;
    bool isDraggingSlider;

    void initSlider();
    void updateSliderFromMouse(int mouseX);

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