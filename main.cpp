// Copyright Sanjana Singh 2024
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Sokoban.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <level_file>" << std::endl;
        return 1;
    }

    SB::Sokoban game(argv[1]);

    sf::RenderWindow window(sf::VideoMode(game.pixelWidth(), game.pixelHeight()), "Sokoban");
    sf::Clock clock;
    sf::Font font;
    if (!font.loadFromFile("font.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return 1;
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::W:
                    case sf::Keyboard::Up:
                        game.movePlayer(SB::Direction::Up);
                        break;
                    case sf::Keyboard::S:
                    case sf::Keyboard::Down:
                        game.movePlayer(SB::Direction::Down);
                        break;
                    case sf::Keyboard::A:
                    case sf::Keyboard::Left:
                        game.movePlayer(SB::Direction::Left);
                        break;
                    case sf::Keyboard::D:
                    case sf::Keyboard::Right:
                        game.movePlayer(SB::Direction::Right);
                        break;
                    case sf::Keyboard::R:
                        game.reset();
                        break;
                    case sf::Keyboard::X:
                        game.undo();
                        break;
                    case sf::Keyboard::Y:
                        game.redo();
                        break;
                    default:
                        break;
                }
            }
        }

        float deltaTime = clock.restart().asSeconds();
        game.updateElapsedTime(deltaTime);

        window.clear(sf::Color::Black);
        window.draw(game);

        // Draw elapsed time
        sf::Text timeText(game.getElapsedTimeString(), font, 20);
        timeText.setPosition(10, 10);
        window.draw(timeText);

        // Draw move count
        sf::Text moveText("Moves: " + std::to_string(game.getMoveCount()), font, 20);
        moveText.setPosition(10, 40);
        window.draw(moveText);

        window.display();
    }

    return 0;
}
