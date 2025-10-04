#ifndef SEARCH_SCREEN_H
#define SEARCH_SCREEN_H

#include <memory>
#include <string>
#include <raylib.h>
#include "screen.h"
#include "ui.h"

class searchScreen : public Screen {
public:
    searchScreen(float screenWidth, float screenHeight);
    ~searchScreen() override = default;

    void onEnter() override;
    void onExit() override;
    void update() override;
    void draw() override;
    void handleInput() override;

    // Get the searched word
    std::string getSearchedWord() const { return searchQuery; }
    bool hasSearched() const { return shouldNavigate; }
    void resetSearch() { shouldNavigate = false; }

private:
    float screenWidth;
    float screenHeight;
    std::unique_ptr<Frame> rootFrame;

    // Search state
    std::string searchQuery;
    bool isInputActive;
    bool shouldNavigate;
    int cursorPosition;
    float cursorBlinkTimer;
    bool showCursor;

    // Fonts
    Font titleFont;
    Font inputFont;
    Font subtitleFont;
    Font buttonFont;

    // UI element pointers (for updates)
    TextElement* inputTextPtr;
    Frame* inputFramePtr;

    void buildUI();
    void loadFonts();
    void unloadFonts();
};

#endif // SEARCH_SCREEN_H
