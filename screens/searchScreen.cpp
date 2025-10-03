#include "searchScreen.h"
#include <iostream>

// Font Sizes
constexpr int TITLE_SIZE = 72;
constexpr int INPUT_SIZE = 128;
constexpr int SUBTITLE_SIZE = 24;
constexpr int BUTTON_SIZE = 32;

// Font paths
static const char *FONT_BYTESIZED5 = "D:/fonts/Bytesized/Bytesized-Regular.ttf";
static const char *FONT_NOTO_SANS = "D:/fonts/Noto_Sans/static/NotoSans-SemiBold.ttf";
static const char *FONT_MERRIWEATHER = "D:/fonts/Merriweather/static/Merriweather_24pt-Regular.ttf";
static const char *FONT_BUTTON = "D:/fonts/Inter,Source_Code_Pro,Tiny5/Inter/static/Inter_18pt-BoldItalic.ttf"; // Button font

// Colors
constexpr Color BG_HEADER = Color{45, 20, 20, 255};
constexpr Color TEXT_PRIMARY = Color{240, 200, 200, 255};
constexpr Color TEXT_ACCENT = Color{220, 120, 120, 255};
constexpr Color INPUT_BG = Color{50, 25, 25, 255};

searchScreen::searchScreen(float screenWidth, float screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight),
      isInputActive(true), shouldNavigate(false), cursorPosition(0), cursorBlinkTimer(0.0f), showCursor(true) {}

void searchScreen::onEnter() {
    searchQuery = "";
    isInputActive = true;
    shouldNavigate = false;
    cursorPosition = 0;

    loadFonts();
    buildUI();
}

void searchScreen::onExit() {
    rootFrame.reset();
    unloadFonts();
}

void searchScreen::loadFonts() {
    titleFont = LoadFontEx(FONT_BYTESIZED5, TITLE_SIZE, nullptr, 0);
    SetTextureFilter(titleFont.texture, TEXTURE_FILTER_POINT);

    inputFont = LoadFontEx(FONT_NOTO_SANS, INPUT_SIZE, nullptr, 0);
    SetTextureFilter(inputFont.texture, TEXTURE_FILTER_POINT);

    subtitleFont = LoadFontEx(FONT_MERRIWEATHER, SUBTITLE_SIZE, nullptr, 0);
    SetTextureFilter(subtitleFont.texture, TEXTURE_FILTER_POINT);
    
    buttonFont = LoadFontEx(FONT_BUTTON, BUTTON_SIZE, nullptr, 0);
    SetTextureFilter(buttonFont.texture, TEXTURE_FILTER_POINT);
}

void searchScreen::unloadFonts() {
    UnloadFont(titleFont);
    UnloadFont(inputFont);
    UnloadFont(subtitleFont);
    UnloadFont(buttonFont);
}

void searchScreen::buildUI() {
    rootFrame = std::make_unique<Frame>(
        Rectangle{0, 0, screenWidth, screenHeight},
        BG_HEADER,
        Padding(0, 0)
    );
    rootFrame->layoutMode = Frame::Layout::Vertical;
    rootFrame->spacing = 0.0f;
    rootFrame->align = {Alignment::Horizontal::Center, Alignment::Vertical::Center};

    auto contentFrame = std::make_unique<Frame>(
        Rectangle{0, 0, screenWidth * 0.5f, screenHeight * 0.5f},
        BLANK,
        Padding(20, 20)
    );
    contentFrame->layoutMode = Frame::Layout::Vertical;
    contentFrame->spacing = 20.0f;
    contentFrame->align = {Alignment::Horizontal::Left, Alignment::Vertical::Top};

    auto title = std::make_unique<TextElement>("Dictionary", TITLE_SIZE, TEXT_PRIMARY);
    title->font = titleFont;
    title->useCustomFont = true;
    Vector2 titleSize = MeasureTextEx(titleFont, "Dictionary", TITLE_SIZE, 1.0f);
    title->bounds.width = titleSize.x;
    title->bounds.height = titleSize.y;

    auto subtitle = std::make_unique<TextElement>("Dictionary", SUBTITLE_SIZE, TEXT_PRIMARY);
    subtitle->font = subtitleFont;
    subtitle->useCustomFont = true;
    Vector2 subtitleSize = MeasureTextEx(titleFont, "Dictionary", SUBTITLE_SIZE, 1.0f);
    subtitle->bounds.width = subtitleSize.x;
    subtitle->bounds.height = subtitleSize.y;

    auto inputFrame = std::make_unique<Frame>(
        Rectangle{0, 0, 600, 80},
        INPUT_BG,
        Padding(20, 15)
    );
    inputFrame->layoutMode = Frame::Layout::Vertical;

    auto inputText = std::make_unique<TextElement>(
        searchQuery.empty() ? "" : searchQuery,
        INPUT_SIZE,
        TEXT_PRIMARY
    );
    inputText->font = inputFont;
    inputText->useCustomFont = true;
    inputTextPtr = inputText.get();

    inputFrame->AddChild(std::move(inputText));

    auto searchButton = ButtonElement::createAutoSize("Search", 32, Padding(15, 40),
        [this]() {
            if (!searchQuery.empty()) {
                shouldNavigate = true;
                std::cout << "Searching for: " << searchQuery << "\n";
            }
        });
    
    // Set custom font for the button
    searchButton->font = buttonFont; // Using dedicated button font
    searchButton->useCustomFont = true;
    
    searchButton->style.normalColor = Color{180, 100, 100, 255};
    searchButton->style.hoverColor = Color{200, 120, 120, 255};
    searchButton->style.pressedColor = Color{160, 80, 80, 255};
    searchButton->style.textNormalColor = TEXT_PRIMARY;
    searchButton->style.textHoverColor = WHITE;
    searchButton->style.cornerRadius = 8.0f;

    contentFrame->AddChild(std::move(title));
    contentFrame->AddChild(std::move(subtitle));
    contentFrame->AddChild(SpacerElement::createVertical(20.0f));
    contentFrame->AddChild(std::move(inputFrame));
    contentFrame->AddChild(SpacerElement::createVertical(10.0f));
    contentFrame->AddChild(std::move(searchButton));
    
    rootFrame->AddChild(std::move(contentFrame));
}

void searchScreen::handleInput() {
    int key = GetCharPressed();
    
    while (key > 0) {
        if ((key >= 32) && (key <= 125)) {
            searchQuery += static_cast<char>(key);
            cursorPosition++;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !searchQuery.empty()) {
        searchQuery.pop_back();
        if (cursorPosition > 0) cursorPosition--;
    }

    if (IsKeyPressed(KEY_ENTER) && !searchQuery.empty()) {
        shouldNavigate = true;
        std::cout << "Searching for: " << searchQuery << "\n";
    }

    if (inputTextPtr) {
        inputTextPtr->setText(searchQuery.empty() ? "" : searchQuery);
    }

    
}

void searchScreen::update() {
    handleInput();
    
    cursorBlinkTimer += GetFrameTime();
    if (cursorBlinkTimer >= 0.5f) {
        showCursor = !showCursor;
        cursorBlinkTimer = 0.0f;
    }

    rootFrame->update({0, 0});
}

void searchScreen::draw() {
    rootFrame->draw({0, 0});

    if (isInputActive && showCursor && inputTextPtr) {
        Vector2 textSize = MeasureTextEx(inputFont, searchQuery.c_str(), 48.0f, 1.0f);
        float cursorX = (screenWidth - 600) / 2 + 20 + textSize.x + 5;
        float cursorY = screenHeight / 2 - 40 + 20;
        DrawRectangle(static_cast<int>(cursorX), static_cast<int>(cursorY), 3, 48, TEXT_PRIMARY);
    }
}
