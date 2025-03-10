// Copyright Sanjana Singh 2024
#ifndef SOKOBAN_HPP
#define SOKOBAN_HPP

#include <iostream>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

namespace SB {
enum class Direction { Up, Down, Left, Right };

class Sokoban : public sf::Drawable {
 public:
    Sokoban();
    explicit Sokoban(const std::string& filename);
    bool isWon() const;
    sf::Vector2i playerLoc() const;
    void movePlayer(Direction direction);
    int width() const;
    int height() const;
    void updateElapsedTime(float deltaTime);
    std::string getElapsedTimeString() const;
    void reset();
    void undo();
    void redo();
    int getMoveCount() const;

    static const int TILE_SIZE = 64;
    int pixelWidth() const;
    int pixelHeight() const;

    friend std::istream& operator>>(std::istream& in, Sokoban& sokoban);
    friend std::ostream& operator<<(std::ostream& out, const Sokoban& sokoban);

 protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

 private:
    int boardWidth;
    int boardHeight;
    sf::Vector2i playerPosition;
    sf::Vector2i initialPlayerPosition;
    std::vector<std::vector<char>> gameBoard;
    std::vector<std::vector<char>> initialGameBoard;
    std::map<char, sf::Texture> textures;
    std::map<char, sf::Sprite> sprites;
    float elapsedTime;
    bool gameWon;
    int moveCount;
    bool victorySoundPlayed;
    Direction lastDirection;
    sf::SoundBuffer victorySoundBuffer;
    sf::Sound victorySound;

    void loadTextures();
    bool isValidMove(int x, int y) const;
    bool pushBox(int x, int y, Direction direction);
    void checkWinCondition();
    struct GameState {
        sf::Vector2i playerPosition;
        std::vector<std::vector<char>> gameBoard;
        int moveCount;
    };
    std::vector<GameState> undoStack;
    std::vector<GameState> redoStack;
    void saveState();
    sf::Sprite getPlayerSprite() const;
};
}  // namespace SB

#endif
