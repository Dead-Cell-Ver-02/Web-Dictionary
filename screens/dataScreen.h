#ifndef DATA_SCREEN_H
#define DATA_SCREEN_H

#include <memory>
#include <string>
#include <raylib.h>
#include "screen.h"
#include "ui.h"
#include "fetcher.h"

class dataScreen : public Screen {
public:
    dataScreen(float screenWidth, float screenHeight);
    ~dataScreen() override = default;

    void onEnter() override;
    void onExit() override;
    void update() override;
    void draw() override;

    // Load new word data
    void loadWord(const std::string& word);
    bool hasBackRequested() const { return shouldGoBack; }
    void resetBackRequest() { shouldGoBack = false; }

private:
    float screenWidth;
    float screenHeight;
    std::unique_ptr<Frame> rootFrame;

    // Word data
    WordData currentWordData;
    bool shouldGoBack;

    // Fonts
    Font wordFont;
    Font phoneticFont;
    Font posFont;
    Font definitionFont;

    // UI element pointers (for updates)
    ButtonElement* backButtonPtr;

    void buildUI(const WordData& data);
    void loadFonts(const WordData& data);
    void unloadFonts();
};

#endif // DATA_SCREEN_H
