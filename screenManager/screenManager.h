#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <memory>
#include "screen.h"
#include "searchScreen.h"
#include "dataScreen.h"

class screenManager {
public:
    enum class screenType {
        Search,
        Data
    };

    screenManager(float screenWidth, float screenHeight);
    ~screenManager();

    void initialize();
    void run();
    void cleanup();
    void switchScreen(screenType screen);

private:
    float screenWidth;
    float screenHeight;

    std::unique_ptr<searchScreen> schScreen;
    std::unique_ptr<dataScreen> datScreen;

    Screen* currentScreen;
    screenType currentScreenType;

    void handleScreenTransitions();
};

#endif // SCREEN_MANAGER_H
