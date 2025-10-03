#include <raylib.h>
#include "screenManager.h"

constexpr Color BG = Color{45, 20, 25, 255};

screenManager::screenManager(float screenWidth, float screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight),
      currentScreen(nullptr), currentScreenType(screenType::Search) {}

screenManager::~screenManager() {
    cleanup();
}

void screenManager::initialize() {
    InitWindow(static_cast<int>(screenWidth), static_cast<int>(screenHeight), "Dictionary");
    SetTargetFPS(60);

    schScreen = std::make_unique<searchScreen>(screenWidth, screenHeight);
    datScreen = std::make_unique<dataScreen>(screenWidth, screenHeight);

    switchScreen(screenType::Search);
}

void screenManager::switchScreen(screenType screen) {
    if (currentScreen) {
        currentScreen->onExit();
    }

    currentScreenType = screen;
    switch (screen) {
        case screenType::Search:
            currentScreen = schScreen.get();
            break;
        case screenType::Data:
            currentScreen = datScreen.get();
            break;
    }

    if (currentScreen) {
        currentScreen->onEnter();
    }
}

void screenManager::handleScreenTransitions() {
    if (currentScreenType == screenType::Search && schScreen->hasSearched()) {
        std::string word = schScreen->getSearchedWord();
        schScreen->resetSearch();

        datScreen->loadWord(word);
        switchScreen(screenType::Data);
    }
    else if (currentScreenType == screenType::Data && datScreen->hasBackRequested()) {
        datScreen->resetBackRequest();
        switchScreen(screenType::Search);
    }
}

void screenManager::run() {
    while (!WindowShouldClose()) {
        handleScreenTransitions();

        if (currentScreen) {
            currentScreen->update();
        }

        BeginDrawing();
        ClearBackground(BG);
        
        if (currentScreen) {
            currentScreen->draw();
        }
        
        EndDrawing();
    }
}

void screenManager::cleanup() {
    if (currentScreen) {
        currentScreen->onExit();
    }
    CloseWindow();
}
