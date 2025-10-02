//
// Created by SAGNIK on 30-09-2025.
//

#include "ui/ui.h"
#include <raylib.h>
#include <memory>

// Constants
constexpr float SCREEN_WIDTH = 1920.0f;
constexpr float SCREEN_HEIGHT = 1080.0f;
constexpr int TARGET_FPS = 60;

// Font sizes
constexpr int WORD_FONT_SIZE = 128;
constexpr int PHONETIC_FONT_SIZE = 40;
constexpr int POS_FONT_SIZE = 40;
constexpr int DEFINITION_FONT_SIZE = 24;

// Font paths
const char* FONT_TINY5 = "D:/fonts/Inter,Source_Code_Pro,Tiny5/Tiny5/Tiny5-regular.ttf";
const char* FONT_NOTO_SANS = "D:/fonts/Noto_Sans/static/NotoSans-SemiBold.ttf";
const char* FONT_INTER = "D:/fonts/Inter,Source_Code_Pro,Tiny5/Inter/static/Inter_18pt-BoldItalic.ttf";
const char* FONT_MERRIWEATHER = "D:/fonts/Merriweather/static/Merriweather_24pt-Regular.ttf";

// Dark red color scheme
constexpr Color BG_HEADER = Color{ 45, 20, 20, 255 };
constexpr Color BG_CONTENT = Color{ 35, 15, 15, 255 };
constexpr Color TEXT_PRIMARY = Color{ 240, 200, 200, 255 };
constexpr Color TEXT_ACCENT = Color{ 220, 120, 120, 255 };

int main() {
    InitWindow(static_cast<int>(SCREEN_WIDTH), static_cast<int>(SCREEN_HEIGHT), "Dictionary");
    SetTargetFPS(TARGET_FPS);

    // Word data
    std::string word = "Destiny";
    std::string phonetic = "/ˈdɛstɪni/";
    std::string pos = "_noun";
    std::string definition = "That to which any person or thing is destined; a predetermined state; a condition predestined by the Divine or by human will.";

    // Load codepoints for phonetic text
    int codePointsCount = 0;
    int* codepoints = LoadCodepoints(phonetic.c_str(), &codePointsCount);

    // Load fonts
    Font wordFont = LoadFontEx(FONT_TINY5, WORD_FONT_SIZE, nullptr, 0);
    SetTextureFilter(wordFont.texture, TEXTURE_FILTER_POINT);

    Font phoneticFont = LoadFontEx(FONT_NOTO_SANS, PHONETIC_FONT_SIZE, codepoints, codePointsCount);
    UnloadCodepoints(codepoints);
    SetTextureFilter(phoneticFont.texture, TEXTURE_FILTER_POINT);

    Font posFont = LoadFontEx(FONT_INTER, POS_FONT_SIZE, nullptr, 0);
    Font definitionFont = LoadFontEx(FONT_MERRIWEATHER, DEFINITION_FONT_SIZE, nullptr, 0);

    // Create root frame
    Frame rootFrame({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, BG_HEADER, Padding(0, 0));
    rootFrame.layoutMode = Frame::Layout::Vertical;
    rootFrame.spacing = 0.0f;

    // Create header frame
    auto headFrame = std::make_unique<Frame>(
        Rectangle{ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 3 },
        BG_HEADER,
        Padding(150, 80)
    );
    headFrame->layoutMode = Frame::Layout::Vertical;

    // Create content frame
    auto tailFrame = std::make_unique<Frame>(
        Rectangle{ 0, 0, SCREEN_WIDTH, (SCREEN_HEIGHT * 2) / 3 },
        BG_CONTENT,
        Padding(100, 80, 0, 80)
    );
    tailFrame->layoutMode = Frame::Layout::Vertical;

    // Create word element
    auto wordElement = std::make_unique<TextElement>(word, WORD_FONT_SIZE, TEXT_PRIMARY);
    wordElement->font = wordFont;
    wordElement->useCustomFont = true;
    Vector2 wordSize = MeasureTextEx(wordFont, word.c_str(), static_cast<float>(WORD_FONT_SIZE), 1.0f);
    wordElement->bounds.width = wordSize.x;
    wordElement->bounds.height = wordSize.y;

    // Create phonetic element
    auto phoneticElement = std::make_unique<TextElement>(phonetic, PHONETIC_FONT_SIZE, TEXT_PRIMARY);
    phoneticElement->font = phoneticFont;
    phoneticElement->useCustomFont = true;
    Vector2 phoneticSize = MeasureTextEx(phoneticFont, phonetic.c_str(), static_cast<float>(PHONETIC_FONT_SIZE), 1.0f);
    phoneticElement->bounds.width = phoneticSize.x;
    phoneticElement->bounds.height = phoneticSize.y;

    // Create POS element
    auto posElement = std::make_unique<TextElement>(pos, POS_FONT_SIZE, TEXT_ACCENT);
    posElement->font = posFont;
    posElement->useCustomFont = true;

    // Create horizontal line frame for phonetic and POS
    auto lineFrame = std::make_unique<Frame>(Rectangle{ 0, 0, SCREEN_WIDTH, 0 }, BLANK);
    lineFrame->layoutMode = Frame::Layout::Horizontal;

    // Create spacer elements
    auto horizontalGap = std::make_unique<spacerElement>(20.0f, phoneticElement->bounds.height);
    auto verticalGap = std::make_unique<spacerElement>(0.0f, -20.0f);

    // Create definition element
    Rectangle tailDrawArea = tailFrame->getDrawArea({ tailFrame->drawArea.x, tailFrame->drawArea.y });
    auto definitionElement = std::make_unique<TextElement>(definition, DEFINITION_FONT_SIZE, TEXT_PRIMARY);
    definitionElement->font = definitionFont;
    definitionElement->useCustomFont = true;
    definitionElement->useWrapText = true;
    definitionElement->wrapLength = tailDrawArea.width;
    definitionElement->wrap_text();

    // Build layout hierarchy
    lineFrame->AddChild(std::move(phoneticElement));
    lineFrame->AddChild(std::move(horizontalGap));
    lineFrame->AddChild(std::move(posElement));

    headFrame->AddChild(std::move(wordElement));
    headFrame->AddChild(std::move(verticalGap));
    headFrame->AddChild(std::move(lineFrame));

    tailFrame->AddChild(std::move(definitionElement));

    rootFrame.AddChild(std::move(headFrame));
    rootFrame.AddChild(std::move(tailFrame));

    // Main loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BG_HEADER);
        rootFrame.draw({ 0, 0 });
        EndDrawing();
    }

    // Cleanup
    UnloadFont(wordFont);
    UnloadFont(phoneticFont);
    UnloadFont(posFont);
    UnloadFont(definitionFont);
    CloseWindow();

    return 0;
}
