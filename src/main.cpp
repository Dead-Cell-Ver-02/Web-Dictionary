//
// Created by SAGNIK on 30-09-2025.
//


#include <raylib.h>
#include <iostream>
#include <set>

//
// Created by SAGNIK on 30-09-2025.
//

#include "ui/ui.h"
#include <nlohmann/json.hpp>
#include "fetcher/fetcher.h"

// Constants
constexpr float SCREEN_WIDTH = 1920.0f;
constexpr float SCREEN_HEIGHT = 1080.0f;
constexpr int TARGET_FPS = 60;

// Font sizes
constexpr int WORD_FONT_SIZE = 128;
constexpr int PHONETIC_FONT_SIZE = 48;
constexpr int POS_FONT_SIZE = 48;
constexpr int DEFINITION_FONT_SIZE = 24;

// Font paths
const char *FONT_TINY5 = "D:/fonts/Inter,Source_Code_Pro,Tiny5/Tiny5/Tiny5-regular.ttf";
const char *FONT_NOTO_SANS = "D:/fonts/Noto_Sans/static/NotoSans-SemiBold.ttf";
const char *FONT_INTER = "D:/fonts/Inter,Source_Code_Pro,Tiny5/Inter/static/Inter_18pt-BoldItalic.ttf";
const char *FONT_MERRIWEATHER = "D:/fonts/Merriweather/static/Merriweather_24pt-Regular.ttf";

// Dark red color scheme
constexpr Color BG_HEADER = Color{45, 20, 20, 255};
constexpr Color BG_CONTENT = Color{35, 15, 15, 255};
constexpr Color TEXT_PRIMARY = Color{240, 200, 200, 255};
constexpr Color TEXT_ACCENT = Color{220, 120, 120, 255};

int main()
{
    InitWindow(static_cast<int>(SCREEN_WIDTH), static_cast<int>(SCREEN_HEIGHT), "Dictionary");
    SetTargetFPS(TARGET_FPS);

    // Word Data
    WordData data = fetchWordData("honey");
    
    // Load codepoints for phonetic text
    int codePointsCount = 0;
    int *codepoints = LoadCodepoints(data.phonetic.c_str(), &codePointsCount);

    // Load fonts
    Font wordFont = LoadFontEx(FONT_TINY5, WORD_FONT_SIZE, nullptr, 0);
    SetTextureFilter(wordFont.texture, TEXTURE_FILTER_POINT);

    Font phoneticFont = LoadFontEx(FONT_NOTO_SANS, PHONETIC_FONT_SIZE, codepoints, codePointsCount);
    UnloadCodepoints(codepoints);
    SetTextureFilter(phoneticFont.texture, TEXTURE_FILTER_POINT);

    Font posFont = LoadFontEx(FONT_INTER, POS_FONT_SIZE, nullptr, 0);
    Font definitionFont = LoadFontEx(FONT_MERRIWEATHER, DEFINITION_FONT_SIZE, nullptr, 0);

    // Create root frame
    Frame rootFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, BG_HEADER, Padding(0, 0));
    rootFrame.layoutMode = Frame::Layout::Vertical;
    rootFrame.spacing = 0.0f;

    // Create header frame
    auto headFrame = std::make_unique<Frame>(
        Rectangle{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 3},
        BG_HEADER,
        Padding(150, 80));
    headFrame->layoutMode = Frame::Layout::Vertical;

    // Create content frame
    auto tailFrame = std::make_unique<Frame>(
        Rectangle{0, 0, SCREEN_WIDTH, (SCREEN_HEIGHT * 2) / 3},
        BG_CONTENT,
        Padding(100, 80, 0, 80));
    tailFrame->layoutMode = Frame::Layout::Vertical;

    // Create word element
    auto wordElement = std::make_unique<TextElement>(data.word, WORD_FONT_SIZE, TEXT_PRIMARY);
    wordElement->font = wordFont;
    wordElement->useCustomFont = true;
    Vector2 wordSize = MeasureTextEx(wordFont, data.word.c_str(), static_cast<float>(WORD_FONT_SIZE), 1.0f);
    wordElement->bounds.width = wordSize.x;
    wordElement->bounds.height = wordSize.y;

    // Create phonetic element
    auto phoneticElement = std::make_unique<TextElement>(data.phonetic, PHONETIC_FONT_SIZE, TEXT_PRIMARY);
    phoneticElement->font = phoneticFont;
    phoneticElement->useCustomFont = true;
    Vector2 phoneticSize = MeasureTextEx(phoneticFont, data.phonetic.c_str(), static_cast<float>(PHONETIC_FONT_SIZE), 1.0f);
    phoneticElement->bounds.width = phoneticSize.x;
    phoneticElement->bounds.height = phoneticSize.y;

    // Create POS element
    auto posframe = std::make_unique<Frame>(Rectangle{0, 0, 0, 0}, BLANK, Padding(0, 0));
    posframe->layoutMode = Frame::Layout::Horizontal;

    int index = 0;
    for (const auto &posStr : data.posList)
    {
        auto posElement = std::make_unique<TextElement>(posStr, POS_FONT_SIZE, TEXT_PRIMARY);
        posElement->font = posFont;
        posElement->useCustomFont = true;

        Vector2 posSize = MeasureTextEx(posFont, posStr.c_str(), static_cast<float>(POS_FONT_SIZE), 1.0f);
        posElement->bounds.width = posSize.x;
        posElement->bounds.height = posSize.y;

        posframe->AddChild(std::move(posElement));

        if (index < data.posList.size() - 1)
        {
            auto spacer = std::make_unique<spacerElement>(7.5f, posSize.y); // 7.5px horizontal gap
            posframe->AddChild(std::move(spacer));
        }

        index++;
    }

    // Create horizontal line frame for phonetic and POS
    auto lineFrame = std::make_unique<Frame>(Rectangle{0, 0, SCREEN_WIDTH, 0}, BLANK);
    lineFrame->layoutMode = Frame::Layout::Horizontal;

    // Create spacer elements
    auto horizontalGap = std::make_unique<spacerElement>(20.0f, phoneticElement->bounds.height);
    auto verticalGap = std::make_unique<spacerElement>(0.0f, -20.0f);

    // Create definition element

    auto definitionFrame = std::make_unique<Frame>(Rectangle{0, 0, SCREEN_WIDTH, 0}, BLANK);
    definitionFrame->layoutMode = Frame::Layout::Vertical;

	int idx = 0;
    for (const auto& definitionStr : data.definitionList) {
				auto definitionElement = std::make_unique<TextElement>(definitionStr, DEFINITION_FONT_SIZE, TEXT_PRIMARY);
				definitionElement->font = definitionFont;
				definitionElement->useCustomFont = true;

				Vector2 defSize = MeasureTextEx(definitionFont, definitionStr.c_str(), static_cast<float>(DEFINITION_FONT_SIZE), 1.0f);
        definitionElement->bounds.width = defSize.x;
        definitionElement->bounds.height = defSize.y;

        definitionFrame->AddChild(std::move(definitionElement));

        if (idx < data.definitionList.size() - 1)
        {
            auto spacer = std::make_unique<spacerElement>(defSize.x, 20.0f); // 7.5px horizontal gap
            definitionFrame->AddChild(std::move(spacer));
        }

        idx++;
    }

    // Build layout hierarchy
    lineFrame->AddChild(std::move(phoneticElement)); 
    lineFrame->AddChild(std::move(horizontalGap));
    lineFrame->AddChild(std::move(posframe));

    headFrame->AddChild(std::move(wordElement));
    headFrame->AddChild(std::move(verticalGap));
    headFrame->AddChild(std::move(lineFrame));

    tailFrame->AddChild(std::move(definitionFrame));

    rootFrame.AddChild(std::move(headFrame));
    rootFrame.AddChild(std::move(tailFrame));

    // Main loop
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BG_HEADER);
        rootFrame.draw({0, 0});
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

