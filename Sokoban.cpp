// Copyright Sanjana Singh 2024
#include <sstream>
#include <iomanip>
#include <fstream>
#include "Sokoban.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

namespace SB {

Sokoban::Sokoban()
:boardWidth(0),
boardHeight(0),
playerPosition(),
initialPlayerPosition(),
gameBoard(),
initialGameBoard(),
textures(),
sprites(),
elapsedTime(0.f),
gameWon(false),
moveCount(0),
victorySoundPlayed(false),
lastDirection(Direction::Down),
victorySoundBuffer(),
victorySound()
{loadTextures();
    if (!victorySoundBuffer.loadFromFile("victory.wav")) {
        std::cerr << "Failed to load victory sound!" << std::endl;
    }
    victorySound.setBuffer(victorySoundBuffer);
}

Sokoban::Sokoban(const std::string& filename) : Sokoban() {
    std::ifstream file(filename);
    if (file) {
        file >> *this;
        initialGameBoard = gameBoard;
        initialPlayerPosition = playerPosition;
    }
}

void Sokoban::loadTextures() {
    textures['#'].loadFromFile("block_06.png");
    textures['A'].loadFromFile("crate_03.png");
    textures['a'].loadFromFile("ground_04.png");
    textures['.'].loadFromFile("ground_01.png");
    textures['1'].loadFromFile("crate_03.png");
    textures['@'].loadFromFile("player_05.png");
    textures['U'].loadFromFile("player_08.png");
    textures['D'].loadFromFile("player_05.png");
    textures['L'].loadFromFile("player_20.png");
    textures['R'].loadFromFile("player_17.png");
    for (const auto& [key, texture] : textures) {
    sprites[key] = sf::Sprite(texture);
    }
}

bool Sokoban::isWon() const {
    return gameWon;
}

sf::Vector2i Sokoban::playerLoc() const {
    return playerPosition;
}

void Sokoban::movePlayer(Direction direction) {
    if (gameWon) return;
    int dx = 0, dy = 0;
    switch (direction) {
        case Direction::Up: dy = -1; break;
        case Direction::Down: dy = 1; break;
        case Direction::Left: dx = -1; break;
        case Direction::Right: dx = 1; break;
    }
    int newX = playerPosition.x + dx;
    int newY = playerPosition.y + dy;
    if (isValidMove(newX, newY)) {
        saveState();
        if (gameBoard[newY][newX] == 'A' || gameBoard[newY][newX] == '1') {
            if (pushBox(newX, newY, direction)) {
                playerPosition = {newX, newY};
                moveCount++;
            }
        } else {
            playerPosition = {newX, newY};
            moveCount++;
        }
        lastDirection = direction;
        checkWinCondition();
    }
}

bool Sokoban::isValidMove(int x, int y) const {
    return x >= 0 && x < boardWidth && y >= 0 && y < boardHeight && gameBoard[y][x] != '#';
}

bool Sokoban::pushBox(int x, int y, Direction direction) {
    int dx = 0, dy = 0;
    switch (direction) {
        case Direction::Up: dy = -1; break;
        case Direction::Down: dy = 1; break;
        case Direction::Left: dx = -1; break;
        case Direction::Right: dx = 1; break;
    }
    int newBoxX = x + dx;
    int newBoxY = y + dy;
    if (isValidMove(newBoxX, newBoxY) && gameBoard[newBoxY][newBoxX] != 'A'
     && gameBoard[newBoxY][newBoxX] != '1') {
        gameBoard[newBoxY][newBoxX] = (gameBoard[newBoxY][newBoxX] == 'a') ? '1' : 'A';
        gameBoard[y][x] = (gameBoard[y][x] == '1') ? 'a' : '.';
        return true;
    }
    return false;
}

void Sokoban::checkWinCondition() {
    gameWon = true;
    for (const auto& row : gameBoard) {
        for (char cell : row) {
            if (cell == 'A') {
                gameWon = false;
                return;
            }
        }
    }
    if (gameWon && !victorySoundPlayed) {
        victorySound.play();
        victorySoundPlayed = true;
    }
}

int Sokoban::width() const {
    return boardWidth;
}

int Sokoban::height() const {
    return boardHeight;
}

int Sokoban::pixelWidth() const {
    return boardWidth * TILE_SIZE;
}

int Sokoban::pixelHeight() const {
    return boardHeight * TILE_SIZE;
}

void Sokoban::updateElapsedTime(float deltaTime) {
    elapsedTime += deltaTime;
}

std::string Sokoban::getElapsedTimeString() const {
    int totalSeconds = static_cast<int>(elapsedTime);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << minutes << ":" <<
     std::setfill('0') << std::setw(2) << seconds;
    return ss.str();
}

void Sokoban::reset() {
    elapsedTime = 0.f;
    playerPosition = initialPlayerPosition;
    gameBoard = initialGameBoard;
    gameWon = false;
    victorySoundPlayed = false;
    undoStack.clear();
    redoStack.clear();
    moveCount = 0;
    lastDirection = Direction::Down;
}

void Sokoban::saveState() {
    undoStack.push_back({playerPosition, gameBoard, moveCount});
    redoStack.clear();
}

void Sokoban::undo() {
    if (!undoStack.empty()) {
        redoStack.push_back({playerPosition, gameBoard, moveCount});
        auto state = undoStack.back();
        undoStack.pop_back();
        playerPosition = state.playerPosition;
        gameBoard = state.gameBoard;
        moveCount = state.moveCount;
        gameWon = false;
    }
}

void Sokoban::redo() {
    if (!redoStack.empty()) {
        undoStack.push_back({playerPosition, gameBoard, moveCount});
        auto state = redoStack.back();
        redoStack.pop_back();
        playerPosition = state.playerPosition;
        gameBoard = state.gameBoard;
        moveCount = state.moveCount;
        checkWinCondition();
    }
}

int Sokoban::getMoveCount() const {
    return moveCount;
}

sf::Sprite Sokoban::getPlayerSprite() const {
    switch (lastDirection) {
        case Direction::Up:    return sprites.at('U');
        case Direction::Down:  return sprites.at('D');
        case Direction::Left:  return sprites.at('L');
        case Direction::Right: return sprites.at('R');
        default:               return sprites.at('@');  // Default sprite
    }
}

std::istream& operator>>(std::istream& in, Sokoban& sokoban) {
    in >> sokoban.boardHeight >> sokoban.boardWidth;
    sokoban.gameBoard.resize(sokoban.boardHeight, std::vector<char>(sokoban.boardWidth));
    std::string line;
    std::getline(in, line);
    for (int y = 0; y < sokoban.boardHeight; ++y) {
        std::getline(in, line);
        for (int x = 0; x < sokoban.boardWidth; ++x) {
            sokoban.gameBoard[y][x] = line[x];
            if (line[x] == '@') {
                sokoban.playerPosition = {x, y};
                sokoban.gameBoard[y][x] = '.';
            }
        }
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const Sokoban& sokoban) {
    for (int y = 0; y < sokoban.boardHeight; ++y) {
        for (int x = 0; x < sokoban.boardWidth; ++x) {
            if (x == sokoban.playerPosition.x && y == sokoban.playerPosition.y) {
                out << '@';
            } else {
                out << sokoban.gameBoard[y][x];
            }
        }
        out << '\n';
    }
    return out;
}

void Sokoban::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (int y = 0; y < boardHeight; ++y) {
        for (int x = 0; x < boardWidth; ++x) {
            char cell = gameBoard[y][x];
            sf::Sprite floorSprite = sprites.at('.');
            floorSprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
            target.draw(floorSprite, states);
            if (cell != '.') {
                sf::Sprite cellSprite = sprites.at(cell);
                cellSprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                target.draw(cellSprite, states);
            }
            if (x == playerPosition.x && y == playerPosition.y) {
                sf::Sprite playerSprite = getPlayerSprite();
                playerSprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                target.draw(playerSprite, states);
            }
        }
    }
    if (gameWon) {
        sf::Font font;
        if (!font.loadFromFile("font.ttf")) {
            return;
        }
        sf::Text victoryText;
        victoryText.setFont(font);
        victoryText.setString("You Win!");
        victoryText.setCharacterSize(48);
        victoryText.setFillColor(sf::Color::Yellow);
        sf::FloatRect textRect = victoryText.getLocalBounds();
        victoryText.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
        victoryText.setPosition(pixelWidth() / 2.0f, pixelHeight() / 2.0f);
        sf::RectangleShape background(sf::Vector2f(pixelWidth(), pixelHeight()));
        background.setFillColor(sf::Color(0, 0, 0, 128));
        target.draw(background, states);
        target.draw(victoryText, states);
    }
}

}  // namespace SB
