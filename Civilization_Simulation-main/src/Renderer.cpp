#include "..\include\Simulation.hpp"
#include "..\include\Civilization.hpp"
#include "..\include\Gatherer.hpp"
#include "..\include\Scout.hpp"
#include "..\include\Warrior.hpp"
#include "..\include\Resource.hpp"
#include "..\include\DroppedResource.hpp"
#include "..\include\Artifact.hpp"
#include "..\include\Renderer.hpp"
#include <iostream>

Renderer::Renderer(int width, int height, Simulation* simulation) 
    : sim(simulation) {
    
    // Pobieramy rozdzielczość monitora
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    windowWidth = desktopMode.width;
    windowHeight = desktopMode.height;

    // Fullscreen
    window = new sf::RenderWindow(desktopMode, "Symulacja Cywilizacji", sf::Style::Fullscreen);
    window->setFramerateLimit(60);

    // Czcionka
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "[OSTRZEZENIE] Nie udalo sie zaladowac czcionki arial.ttf!\n";
    }

    isDraggingSlider = false;
    initSlider();
}

// Suwak:
void Renderer::initSlider() {
    float trackWidth = 300.0f;
    float trackHeight = 10.0f;
    
    // Tor suwaka na środku u dołu ekranu
    sliderTrack.setSize(sf::Vector2f(trackWidth, trackHeight));
    sliderTrack.setFillColor(sf::Color(100, 100, 100));
    sliderTrack.setPosition(windowWidth / 2.0f - trackWidth / 2.0f, windowHeight - 60.0f);

    // Przycisk suwaka
    sliderHandle.setSize(sf::Vector2f(20.0f, 30.0f));
    sliderHandle.setFillColor(sf::Color(200, 200, 200));
    sliderHandle.setOrigin(10.0f, 15.0f);
    sliderHandle.setPosition(windowWidth / 2.0f, windowHeight - 55.0f);
}

// Funkcja obsługująca zmianę wartości na podstawie myszki:
void Renderer::updateSliderFromMouse(int mouseX) {
    float minX = sliderTrack.getPosition().x;
    float maxX = minX + sliderTrack.getSize().x;

    float newX = mouseX;
    if (newX < minX) newX = minX;
    if (newX > maxX) newX = maxX;

    sliderHandle.setPosition(newX, sliderHandle.getPosition().y);

    float percentage = (newX - minX) / sliderTrack.getSize().x; 
    
    int minDelay = 10;   // 10 ms (max w prawo)
    int maxDelay = 1000; // 1000 ms (max w lewo)
    
    // Odwracamy proporcję: 0% to najwolniej (maxDelay), 100% to najszybciej (minDelay)
    int delay = maxDelay - percentage * (maxDelay - minDelay);
    
    sim->setTickDelay(delay);
}

Renderer::~Renderer() {
    delete window;
}

void Renderer::renderFrame() {
    sf::Event event;
    
    // Obsługa zdarzeń i sterowanie stanami
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
        }

        // Obsługa suwaka
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (sliderHandle.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) ||
                sliderTrack.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                isDraggingSlider = true;
                updateSliderFromMouse(event.mouseButton.x);
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDraggingSlider = false;
        }
        else if (event.type == sf::Event::MouseMoved) {
            if (isDraggingSlider) {
                updateSliderFromMouse(event.mouseMove.x);
            }
        }

    if (sim->getCurrentState() == GameState::MENU) {

            // Wciśnięcie entera
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {

                    sim->inputFromCSV("input.csv"); // Wczytanie danych z pliku CSV

                    sim->setCurrentState(GameState::SIMULATION);
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    window->close();
                }
            }
        }
        
        // Pozostałe stany
        else if (sim->getCurrentState() == GameState::GAME_OVER) {
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) sim->setCurrentState(GameState::MENU);
                else if (event.key.code == sf::Keyboard::Escape) window->close();
            }
        }
    }

    // Czyszczenie ekranu
    window->clear(sf::Color::Black);

    // Rysowanie w zależności od aktualnego stanu maszyny stanów
    switch (sim->getCurrentState()) {
        case GameState::MENU:
            drawMenu();
            break;

        case GameState::SIMULATION:
            drawMap();
            drawEntities();
            drawUI();
            break;

        case GameState::GAME_OVER:
            drawGameOver();
            break;
    }

    // Wyświetlenie klatki na ekranie
    window->display();
}

void Renderer::drawMenu() {
    sf::Text title;
    title.setFont(font);
    title.setString("SYMULACJA CYWILIZACJI");
    title.setCharacterSize(60);
    title.setFillColor(sf::Color::Green);
    title.setStyle(sf::Text::Bold);
    
    // Centrowanie tekstu na środku ekranu
    float textX = (windowWidth / 2.0f) - (title.getGlobalBounds().width / 2.0f);
    title.setPosition(textX, windowHeight * 0.2f); // Troche wyżej niż na środku

    window->draw(title);

    sf::Text info;
    info.setFont(font);
    info.setString("Nacisnij ENTER, aby wczytac dane z pliku i rozpoczac symulacje\nNacisnij ESC, aby wyjsc");
    info.setCharacterSize(30);
    info.setFillColor(sf::Color::Yellow);
    info.setPosition((windowWidth / 2.0f) - (info.getGlobalBounds().width / 2.0f), windowHeight * 0.8f);
    window->draw(info);

}

void Renderer::drawGameOver() {
    // Półprzezroczyste tło przyciemniające mapę
    sf::RectangleShape overlay(sf::Vector2f(windowWidth, windowHeight));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window->draw(overlay);

    Civilization* winner = sim->getWinner();
    if (winner != nullptr) {
        sf::Text text;
        text.setFont(font);
        text.setString("GAME OVER\n\nCywilizacja wzniosla 3 budynki!\n\nSzczegolowe dane zostaly zapisane w pliku CSV. Nacisnij ESC aby wyjsc.");
        text.setCharacterSize(static_cast<int>(windowHeight * 0.05f));
        text.setFillColor(sf::Color::Red);
        text.setStyle(sf::Text::Bold);
    
        // Centrowanie tekstu
        float textX = (windowWidth / 2.0f) - (text.getGlobalBounds().width / 2.0f);
        float textY = (windowHeight / 2.0f) - (text.getGlobalBounds().height / 2.0f);
        text.setPosition(textX, textY);

        window->draw(text);
    }
    else {
        sf::Text text;
        text.setFont(font);
        text.setString("GAME OVER\n\nOsiagnieto limit 500 tur!\n\nSzczegolowe dane zostaly zapisane w pliku CSV. Nacisnij ESC aby wyjsc.");
        text.setCharacterSize(static_cast<int>(windowHeight * 0.05f));
        text.setFillColor(sf::Color::Red);
        text.setStyle(sf::Text::Bold);
    
        // Centrowanie tekstu
        float textX = (windowWidth / 2.0f) - (text.getGlobalBounds().width / 2.0f);
        float textY = (windowHeight / 2.0f) - (text.getGlobalBounds().height / 2.0f);
        text.setPosition(textX, textY);

        window->draw(text);
    }
}

void Renderer::drawMap() {
    float mapSize = static_cast<float>(windowHeight); 

    // Tło mapy
    sf::RectangleShape mapBackground(sf::Vector2f(mapSize, mapSize));
    mapBackground.setFillColor(sf::Color(24, 45, 24)); 
    mapBackground.setPosition(0.0f, 0.0f);
    window->draw(mapBackground);

    // Linia oddzielająca mapę od panelu UI
    sf::RectangleShape separator(sf::Vector2f(4.0f, mapSize));
    separator.setFillColor(sf::Color(80, 80, 80));
    separator.setPosition(mapSize, 0.0f);
    window->draw(separator);
}

void Renderer::drawTile(int x, int y, std::string text, sf::Color color) {
    // Dynamiczny rozmiar kafelka
    float tileSize = static_cast<float>(windowHeight) / 100.0f; 

    // Rysowanie kafelka
    sf::RectangleShape tile(sf::Vector2f(tileSize - 1.0f, tileSize - 1.0f)); 
    tile.setFillColor(color);
    tile.setPosition(x * tileSize, y * tileSize);
    window->draw(tile);

    // Rysowanie litery jednostki/obiektu wewnątrz kafelka
    if (!text.empty()) {
        sf::Text label;
        label.setFont(font);
        label.setString(text);
        label.setCharacterSize(static_cast<int>(tileSize * 0.8f)); 
        label.setFillColor(sf::Color::Black); 
        label.setStyle(sf::Text::Bold);
        
        // Wyśrodkowanie tekstu wewnątrz kafelka
        float textX = (x * tileSize) + (tileSize / 2.0f) - (label.getGlobalBounds().width / 2.0f);
        float textY = (y * tileSize) + (tileSize / 2.0f) - (label.getGlobalBounds().height / 2.0f) - (tileSize * 0.2f);
        label.setPosition(textX, textY);
        
        window->draw(label);
    }
}

void Renderer::drawEntities() {
    // Pobieramy wektor wszystkich obiektów na mapie
    for (GameObject* obj : sim->getMap()->entities) {
        if (!obj || !obj->get_isActive()) continue;

        int x = obj->get_X();
        int y = obj->get_Y();

        // Sprawdzanie czy to Unit
        Unit* unit = dynamic_cast<Unit*>(obj);
        if (unit) {
            std::string civName = unit->getOwner()->getName();
            sf::Color civColor;
            
            // Dobór koloru dla konkretnych cywilizacji
            if (civName == "Rzym") civColor = sf::Color(128, 0, 128); // Purpurowy
            else if (civName == "Grecja") civColor = sf::Color::Blue; // Niebieski
            else if (civName == "Egipt") civColor = sf::Color(255, 215, 0); // Żółty
            else if (civName == "Chiny") civColor = sf::Color::Red; // Czerwony
            else civColor = sf::Color::White;

            std::string unitLetter = "?";
            if (dynamic_cast<Gatherer*>(unit)) unitLetter = "G";
            else if (dynamic_cast<Scout*>(unit)) unitLetter = "S";
            else if (dynamic_cast<Warrior*>(unit)) unitLetter = "W";

            drawTile(x, y, unitLetter, civColor);
            continue; 
        }

        // Sprawdzanie czy to Resource
        if (dynamic_cast<Resource*>(obj) || dynamic_cast<DroppedResource*>(obj)) {
            drawTile(x, y, "R", sf::Color(128, 128, 128));
            continue;
        }

        // Sprawdzanie czy to Artifact
        if (dynamic_cast<Artifact*>(obj)) {
            drawTile(x, y, "A", sf::Color(255, 105, 180));
            continue;
        }
    }

    // Bazy
    for (Civilization* civ : sim->getCivilizations()) {
        std::string civName = civ->getName();
        sf::Color civColor;
        std::string baseLabel;

        if (civName == "Rzym") { civColor = sf::Color(128, 0, 128); baseLabel = "R"; }
        else if (civName == "Grecja") { civColor = sf::Color::Blue; baseLabel = "GR"; }
        else if (civName == "Egipt") { civColor = sf::Color(255, 215, 0); baseLabel = "E"; }
        else if (civName == "Chiny") { civColor = sf::Color::Red; baseLabel = "C"; }

        drawTile(civ->get_X(), civ->get_Y(), baseLabel, civColor);
    }
}

void Renderer::drawUI() {

    // Rysowanie suwaka
    window->draw(sliderTrack);
    window->draw(sliderHandle);

    // Tekst nad suwakiem
    sf::Text sliderText;
    sliderText.setFont(font);
    sliderText.setString("Tempo: ");
    sliderText.setCharacterSize(20);
    sliderText.setFillColor(sf::Color::White);
    sliderText.setPosition(sliderTrack.getPosition().x - 20.0f, sliderTrack.getPosition().y - 35.0f);
    window->draw(sliderText);

    // Ustawienie ramki w prawym górnym rogu
    float boxWidth = 280.0f;
    float boxHeight = 220.0f;
    float posX = windowWidth - boxWidth - 20.0f;
    float posY = 20.0f;

    // Rysowanie tła tabelki
    sf::RectangleShape uiBox(sf::Vector2f(boxWidth, boxHeight));
    uiBox.setPosition(posX, posY);
    uiBox.setFillColor(sf::Color(30, 30, 30, 220));
    uiBox.setOutlineThickness(2.0f);
    uiBox.setOutlineColor(sf::Color(100, 100, 100));
    window->draw(uiBox);

    // Wyświetlanie aktualnej tury
    sf::Text turnText;
    turnText.setFont(font);
    turnText.setString("TURA: " + std::to_string(sim->getTurnCounter()));
    turnText.setCharacterSize(22);
    turnText.setFillColor(sf::Color::White);
    turnText.setStyle(sf::Text::Bold);
    turnText.setPosition(posX + 15.0f, posY + 15.0f);
    window->draw(turnText);

    // Linia oddzielająca licznik tur od statystyk budynków
    sf::RectangleShape line(sf::Vector2f(boxWidth - 30.0f, 2.0f));
    line.setPosition(posX + 15.0f, posY + 50.0f);
    line.setFillColor(sf::Color(70, 70, 70));
    window->draw(line);

    // Nagłówek sekcji budynków
    sf::Text bldTitle;
    bldTitle.setFont(font);
    bldTitle.setString("Ilość budynków:");
    bldTitle.setCharacterSize(16);
    bldTitle.setFillColor(sf::Color(180, 180, 180));
    bldTitle.setPosition(posX + 15.0f, posY + 60.0f);
    window->draw(bldTitle);

    // Wyświetlanie liczby budynków dla każdej cywilizacji
    float currentY = posY + 90.0f;
    
    for (Civilization* civ : sim->getCivilizations()) {
        std::string civName = civ->getName();
        sf::Color civColor;
        
        // Dopasowanie kolorów czcionki do kolorów nacji na mapie
        if (civName == "Rzym") civColor = sf::Color(210, 100, 210);      // Purpurowy
        else if (civName == "Grecja") civColor = sf::Color(100, 160, 255); // Niebieski
        else if (civName == "Egipt") civColor = sf::Color(255, 215, 0);    // Złoty
        else if (civName == "Chiny") civColor = sf::Color(255, 100, 100);  // Czerwony
        else civColor = sf::Color::White;

        sf::Text bldText;
        bldText.setFont(font);
        bldText.setString(civName + " | Budynki: " + std::to_string(civ->getBuildingsCount()) + " | Surowce: " + std::to_string(civ->getStoredResources()));
        bldText.setCharacterSize(14);

        // Komunikat o budynkach
        sf::Text civText;
        civText.setFont(font);
        civText.setString(civName + ": " + std::to_string(civ->getBuildingsCount()) + " / 3");
        civText.setCharacterSize(18);
        civText.setFillColor(civColor);
        civText.setPosition(posX + 15.0f, currentY);
        window->draw(civText);

        currentY += 28.0f; // Przesunięcie w dół tekstu
    }

//Tabela wydarzeń w symulacji
    float logStartY = currentY + 40.0f; 
    
    sf::RectangleShape logLine(sf::Vector2f(windowWidth - posX - 30.0f, 2.0f));
    logLine.setPosition(posX + 15.0f, logStartY);
    logLine.setFillColor(sf::Color(70, 70, 70));
    window->draw(logLine);

    // Tytuł tabeli
    sf::Text logTitle;
    logTitle.setFont(font);
    logTitle.setString("Wydarzenia: ");
    logTitle.setCharacterSize(16);
    logTitle.setFillColor(sf::Color(255, 200, 50));
    logTitle.setPosition(posX + 15.0f, logStartY + 15.0f);
    window->draw(logTitle);

    // Pobieramy i rysujemy logi z symulacji
    std::vector<std::string> logs = sim->getLogsCopy();
    float textY = logStartY + 45.0f;

    for (const std::string& msg : logs) {
        sf::Text logText;
        logText.setFont(font);
        logText.setString(msg);
        logText.setCharacterSize(14);
        logText.setFillColor(sf::Color(200, 200, 200));
        logText.setPosition(posX + 15.0f, textY);
        window->draw(logText);

        textY += 20.0f; // Odstęp między linijkami logów
    }

}