// Copyright Sanjana Singh 2024
#include <iostream>
#include <cassert>
#include <stdexcept>
#include "Sokoban.hpp"

class SokobanTest {
 private:
    SB::Sokoban* game;

 public:
    void SetUp(const std::string& levelFile) {
        game = new SB::Sokoban(levelFile);
    }

    void TearDown() {
        delete game;
        game = nullptr;
    }

    void RunAll() {
        std::cout << "Running all tests..." << std::endl;

        TestLevelLoading();

        SetUp("test_level.lvl");
        TestBasicMovement();
        TestBoxPushing();
        TestBorderCollision();
        TestWinCondition();
        TestUndoRedo();
        TestDimensions();
        TearDown();

        std::cout << "All tests completed." << std::endl;
    }

    void TestLevelLoading() {
        std::cout << "Testing level loading..." << std::endl;
        try {
            SB::Sokoban validGame("test_level.lvl");
            std::cout << "Successfully loaded valid level file." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Unexpected exception when loading valid level: " << e.what() << std::endl;
            assert(false && "Valid level loading should not throw an exception");
        }
        try {
            SB::Sokoban invalidGame("nonexistent_level.lvl");
            std::cout <<
            "Note: No exception thrown for nonexistent level."
            << std::endl;
            assert(invalidGame.width() == 0 && invalidGame.height() == 0
            && "Invalid game should have zero dimensions");
        } catch (const std::exception& e) {
            std::cout << "Exception caught as expected: " << e.what() << std::endl;
        }

        std::cout << "Level loading test passed." << std::endl;
    }

    void TestBasicMovement() {
        std::cout << "Testing basic movement..." << std::endl;
        auto initialPos = game->playerLoc();
        game->movePlayer(SB::Direction::Right);
        assert(game->playerLoc() != initialPos && "Player should move right");
        game->movePlayer(SB::Direction::Left);
        assert(game->playerLoc() == initialPos && "Player should move back to initial position");
        std::cout << "Basic movement test passed." << std::endl;
    }

    void TestBoxPushing() {
        std::cout << "Testing box pushing..." << std::endl;
        auto initialPos = game->playerLoc();
        game->movePlayer(SB::Direction::Right);
        assert(game->playerLoc() != initialPos && "Player should move when pushing a box");
        std::cout << "Box pushing test passed." << std::endl;
    }

    void TestBorderCollision() {
        std::cout << "Testing border collision..." << std::endl;
        auto initialPos = game->playerLoc();
        for (int i = 0; i < 10; ++i) {
            game->movePlayer(SB::Direction::Left);
        }
        assert(game->playerLoc() != initialPos && "Player should have moved");
        assert(game->playerLoc().x != 0 && "Player should not be at the left edge");
        std::cout << "Border collision test passed." << std::endl;
    }

    void TestWinCondition() {
        std::cout << "Testing win condition..." << std::endl;
        assert(!game->isWon() && "Game should not be in won state initially");
        std::cout << "Win condition test passed." << std::endl;
    }

    void TestUndoRedo() {
        std::cout << "Testing undo and redo..." << std::endl;
        auto initialPos = game->playerLoc();
        game->movePlayer(SB::Direction::Right);
        auto newPos = game->playerLoc();
        game->undo();
        assert(game->playerLoc() == initialPos && "Undo should return to initial position");
        game->redo();
        assert(game->playerLoc() == newPos && "Redo should return to new position");
        std::cout << "Undo and redo test passed." << std::endl;
    }

    void TestDimensions() {
        std::cout << "Testing dimensions..." << std::endl;
        assert(game->pixelWidth() == game->width() * SB::Sokoban::TILE_SIZE &&
        "Pixel width should match tile size * width");
        assert(game->pixelHeight() == game->height() * SB::Sokoban::TILE_SIZE &&
        "Pixel height should match tile size * height");
        std::cout << "Dimensions test passed." << std::endl;
    }
};

int main() {
    SokobanTest test;
    try {
        test.RunAll();
        std::cout << "All tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
