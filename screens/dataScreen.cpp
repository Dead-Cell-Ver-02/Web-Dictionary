#include "dataScreen.h"
#include <iostream>

// Font sizes
constexpr int WORD_FONT_SIZE = 128;
constexpr int PHONETIC_FONT_SIZE = 48;
constexpr int POS_FONT_SIZE = 48;
constexpr int DEFINITION_FONT_SIZE = 24;

// Font paths
static const char *FONT_TINY5 = "D:/fonts/Inter,Source_Code_Pro,Tiny5/Tiny5/Tiny5-regular.ttf";
static const char *FONT_NOTO_SANS = "D:/fonts/Noto_Sans/static/NotoSans-SemiBold.ttf";
static const char *FONT_INTER = "D:/fonts/Inter,Source_Code_Pro,Tiny5/Inter/static/Inter_18pt-BoldItalic.ttf";
static const char *FONT_MERRIWEATHER = "D:/fonts/Merriweather/static/Merriweather_24pt-Regular.ttf";

// Dark red color scheme
constexpr Color BG_HEADER = Color{45, 20, 20, 255};
constexpr Color BG_CONTENT = Color{35, 15, 15, 255};
constexpr Color TEXT_PRIMARY = Color{240, 200, 200, 255};
constexpr Color TEXT_ACCENT = Color{220, 120, 120, 255};

dataScreen::dataScreen(float screenWidth, float screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight), shouldGoBack(false) {}

void dataScreen::onEnter() { shouldGoBack = false; }
void dataScreen::onExit() {
    rootFrame.reset();
    unloadFonts();
}

void dataScreen::loadWord(const std::string& word) {
    currentWordData = fetchWordData(word);
    loadFonts(currentWordData);
    buildUI(currentWordData);
}

void dataScreen::loadFonts(const WordData& data) {
    int codePointsCount = 0;
    int *codepoints = LoadCodepoints(data.phonetic.c_str(), &codePointsCount);

    // Load fonts
    wordFont = LoadFontEx(FONT_TINY5, WORD_FONT_SIZE, nullptr, 0);
    SetTextureFilter(wordFont.texture, TEXTURE_FILTER_POINT);

    phoneticFont = LoadFontEx(FONT_NOTO_SANS, PHONETIC_FONT_SIZE, codepoints, codePointsCount);
    UnloadCodepoints(codepoints);
    SetTextureFilter(phoneticFont.texture, TEXTURE_FILTER_POINT);

    posFont = LoadFontEx(FONT_INTER, POS_FONT_SIZE, nullptr, 0);
    definitionFont = LoadFontEx(FONT_MERRIWEATHER, DEFINITION_FONT_SIZE, nullptr, 0);
}

void dataScreen::unloadFonts() {
    UnloadFont(wordFont);
    UnloadFont(phoneticFont);
    UnloadFont(posFont);
    UnloadFont(definitionFont);
}

void dataScreen::buildUI(const WordData& data) {
    // root frame
    rootFrame = std::make_unique<Frame>(
        Rectangle{0, 0, screenWidth, screenHeight},
        BLANK,
        Padding(0.0f)
    );
    rootFrame->layoutMode = Frame::Layout::Vertical;
    rootFrame->spacing = 0.0f;

    auto topBar = std::make_unique<Frame>(
        Rectangle{0, 0, screenWidth, 80},
        BLANK,
        Padding(20.0f)
    );
    topBar->layoutMode = Frame::Layout::Horizontal;
    topBar->align = Alignment{Alignment::Horizontal::Left, Alignment::Vertical::Center};

    auto backButton = ButtonElement::createAutoSize("< Back", 24, Padding(10.0f, 20.0f),
        [this]() {
            shouldGoBack = true;
            std::cout << "prev screen \n";
        });

    backButton->font = posFont;
    backButton->useCustomFont = true;
    backButton->style.normalColor = Color{70, 35, 35, 255};
    backButton->style.hoverColor = Color{90, 45, 45, 255};
    backButton->style.pressedColor = Color{50, 25, 25, 255};
    backButton->style.textNormalColor = TEXT_PRIMARY;
    backButton->style.textHoverColor = WHITE;
    backButtonPtr = backButton.get();

    topBar->AddChild(std::move(backButton));

    auto headFrame = std::make_unique<Frame>(
        Rectangle{0, 0, screenWidth, screenHeight / 3 - 40},
        BG_HEADER,
        Padding(100.0f, 80.0f)
    );
    headFrame->layoutMode = Frame::Layout::Vertical;

    auto tailFrame = std::make_unique<Frame>(
        Rectangle{0, 0, screenWidth, (screenHeight * 2) / 3},
        BG_CONTENT,
        Padding(100.0f, 80.0f, 0.0f, 80.0f)
    );
    tailFrame->layoutMode = Frame::Layout::Vertical;

    auto wordElement = std::make_unique<TextElement>(data.word, WORD_FONT_SIZE, TEXT_PRIMARY);
    wordElement->font = wordFont;
    wordElement->useCustomFont = true;
    Vector2 wordSize = MeasureTextEx(wordFont, data.word.c_str(), static_cast<float>(WORD_FONT_SIZE), 1.0f);
    wordElement->bounds.width = wordSize.x;
    wordElement->bounds.height = wordSize.y;

    auto phoneticElement = std::make_unique<TextElement>(data.phonetic, PHONETIC_FONT_SIZE, TEXT_PRIMARY);
    phoneticElement->font = phoneticFont;
    phoneticElement->useCustomFont = true;
    Vector2 phoneticSize = MeasureTextEx(phoneticFont, data.phonetic.c_str(), static_cast<float>(PHONETIC_FONT_SIZE), 1.0f);
    phoneticElement->bounds.width = phoneticSize.x;
    phoneticElement->bounds.height = phoneticSize.y;

    auto posframe = std::make_unique<Frame>(Rectangle{0, 0, 0, 0}, BLANK, Padding(0.0f));
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

        if (index < data.posList.size() - 1) {
            auto spacer = SpacerElement::createHorizontal(7.5f);
            posframe->AddChild(std::move(spacer));
        }

        index++;
    }

    auto lineFrame = std::make_unique<Frame>(Rectangle{0, 0, screenWidth, 0}, BLANK, Padding(0.0f));
    lineFrame->layoutMode = Frame::Layout::Horizontal;

    auto horizontalGap = SpacerElement::createHorizontal(20.0f);
    auto verticalGap = SpacerElement::createVertical(20.0f);

    auto definitionFrame = std::make_unique<Frame>(Rectangle{0, 0, screenWidth, 0}, BLANK, Padding(0.0f));
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

        if (idx < data.definitionList.size() - 1) {
            auto spacer = SpacerElement::createVertical(20.0f);
            definitionFrame->AddChild(std::move(spacer));
        }

        idx++;
    }

    lineFrame->AddChild(std::move(phoneticElement));
    lineFrame->AddChild(std::move(horizontalGap));
    lineFrame->AddChild(std::move(posframe));

    headFrame->AddChild(std::move(wordElement));
    headFrame->AddChild(std::move(verticalGap));
    headFrame->AddChild(std::move(lineFrame));

    tailFrame->AddChild(std::move(definitionFrame));

    rootFrame->AddChild(std::move(topBar));
    rootFrame->AddChild(std::move(headFrame));
    rootFrame->AddChild(std::move(tailFrame));

}

void dataScreen::update() {
    rootFrame->update({0, 0});
}

void dataScreen::draw() {
    rootFrame->draw({0, 0});
}
