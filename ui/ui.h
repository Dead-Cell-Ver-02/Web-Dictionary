//
// Created by SAGNIK on 30-09-2025.
//

#ifndef UI_H
#define UI_H

#include <algorithm>
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <raylib.h>

// ============================================================================
// UTILITY STRUCTURES
// ============================================================================

struct Padding {
    float top = 0, right = 0, bottom = 0, left = 0;

    Padding(float all = 0.0f) : top(all), right(all), bottom(all), left(all) {}
    Padding(float vertical, float horizontal) 
        : top(vertical), right(horizontal), bottom(vertical), left(horizontal) {}
    Padding(float t, float r, float b, float l) 
        : top(t), right(r), bottom(b), left(l) {}

    [[nodiscard]] float totalHorizontal() const { return left + right; }
    [[nodiscard]] float totalVertical() const { return top + bottom; }
};

struct Margin {
    float top = 0, right = 0, bottom = 0, left = 0;

    Margin(float all = 0.0f) : top(all), right(all), bottom(all), left(all) {}
    Margin(float t, float r, float b, float l) 
        : top(t), right(r), bottom(b), left(l) {}

    [[nodiscard]] float totalHorizontal() const { return left + right; }
    [[nodiscard]] float totalVertical() const { return top + bottom; }
};

struct Alignment {
    enum class Horizontal { Left, Center, Right, Stretch };
    enum class Vertical { Top, Center, Bottom, Stretch };

    Horizontal hAlign;
    Vertical vAlign;

    Alignment() : hAlign(Horizontal::Left), vAlign(Vertical::Top) {}
    Alignment(Horizontal h, Vertical v) : hAlign(h), vAlign(v) {}
};

// ============================================================================
// BASE DRAWABLE ELEMENT
// ============================================================================

struct DrawElement {
    Rectangle bounds;

    DrawElement() : bounds{0, 0, 0, 0} {}
    explicit DrawElement(const Rectangle& rect) : bounds(rect) {}
    virtual ~DrawElement() = default;

    virtual void draw(Vector2 parentPos) = 0;
    virtual void update(Vector2 parentPos) { (void)parentPos; }
    virtual void updateBounds() {}

    // Helper methods
    [[nodiscard]] Vector2 getSize() const { return {bounds.width, bounds.height}; }
    void setPosition(float x, float y) { bounds.x = x; bounds.y = y; }
    void setSize(float width, float height) { bounds.width = width; bounds.height = height; }
};

// ============================================================================
// TEXT ELEMENT - Optimized for Dictionary Usage
// ============================================================================

struct TextElement : DrawElement {
    std::string txt;
    int fontSize{20};
    Color color{BLACK};
    Vector2 offset{0, 0};
    Font font;
    bool useCustomFont{false};
    bool useWrapText{false};
    float wrapLength = 0.0f;

    // Text wrapping properties
    std::vector<std::string> lines;
    float lineSpacing = 5.0f;
    float characterSpacing = 1.0f;

    // Constructors
    TextElement(std::string text, int fs, Color c, Vector2 off = {0, 0})
        : txt(std::move(text)), fontSize(fs), color(c), offset(off), font(GetFontDefault()) {
        calculateBounds();
    }

    TextElement(std::string text, int fs, Color c, const Font& f, Vector2 off = {0, 0})
        : txt(std::move(text)), fontSize(fs), color(c), offset(off), font(f), useCustomFont(true) {
        calculateBounds();
    }

    void setText(const std::string& newText) {
        if (txt != newText) {
            txt = newText;
            if (useWrapText) {
                wrap_text();
            } else {
                calculateBounds();
            }
        }
    }

    void setColor(Color newColor) {
        color = newColor;
    }

    void setFont(const Font& newFont) {
        font = newFont;
        useCustomFont = true;
        if (useWrapText) {
            wrap_text();
        } else {
            calculateBounds();
        }
    }

    void wrap_text() {
        if (!useWrapText || wrapLength <= 0) return;

        lines.clear();
        std::istringstream words(txt);
        std::string word;
        std::string currentLine;
        bounds.width = 0;

        auto measureLineWidth = [this](const std::string& line) -> float {
            if (useCustomFont) {
                return MeasureTextEx(font, line.c_str(), static_cast<float>(fontSize), characterSpacing).x;
            }
            return static_cast<float>(MeasureText(line.c_str(), fontSize));
        };

        while (words >> word) {
            std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
            float testWidth = measureLineWidth(testLine);

            if (testWidth <= wrapLength || currentLine.empty()) {
                currentLine = testLine;
            }
            else {
                if (!currentLine.empty()) {
                    lines.push_back(currentLine);
                    bounds.width = std::max(bounds.width, measureLineWidth(currentLine));
                }

                currentLine = word;

                if (measureLineWidth(currentLine) > wrapLength) {
                    lines.push_back(currentLine);
                    bounds.width = std::max(bounds.width, measureLineWidth(currentLine));
                    currentLine.clear();
                }
            }
        }

        if (!currentLine.empty()) {
            lines.push_back(currentLine);
            bounds.width = std::max(bounds.width, measureLineWidth(currentLine));
        }

        if (!lines.empty()) {
            bounds.height = static_cast<float>(lines.size()) * static_cast<float>(fontSize) +
                (static_cast<float>(lines.size()) - 1.0f) * lineSpacing;
        }
        else {
            bounds.height = static_cast<float>(fontSize);
        }
    }

    void draw(Vector2 parentPos) override {
        Vector2 drawPos = {
            parentPos.x + bounds.x + offset.x,
            parentPos.y + bounds.y + offset.y
        };

        if (useWrapText && !lines.empty()) {
            for (size_t i = 0; i < lines.size(); i++) {
                Vector2 linePos = {
                    drawPos.x,
                    drawPos.y + static_cast<float>(i) * (static_cast<float>(fontSize) + lineSpacing)
                };

                if (useCustomFont) {
                    DrawTextEx(font, lines[i].c_str(), linePos,
                        static_cast<float>(fontSize), characterSpacing, color);
                }
                else {
                    DrawText(lines[i].c_str(),
                        static_cast<int>(linePos.x),
                        static_cast<int>(linePos.y),
                        fontSize,
                        color);
                }
            }
        }
        else {
            if (useCustomFont) {
                DrawTextEx(font, txt.c_str(), drawPos,
                    static_cast<float>(fontSize), characterSpacing, color);
            }
            else {
                DrawText(txt.c_str(),
                    static_cast<int>(drawPos.x),
                    static_cast<int>(drawPos.y),
                    fontSize,
                    color);
            }
        }
    }

    void updateBounds() override {
        if (useWrapText && wrapLength > 0) {
            wrap_text();
        }
        else {
            calculateBounds();
        }
    }

private:
    void calculateBounds() {
        if (useCustomFont) {
            Vector2 size = MeasureTextEx(font, txt.c_str(),
                static_cast<float>(fontSize), characterSpacing);
            bounds.width = size.x;
            bounds.height = size.y;
        }
        else {
            bounds.width = static_cast<float>(MeasureText(txt.c_str(), fontSize));
            bounds.height = static_cast<float>(fontSize);
        }
    }
};

// ============================================================================
// BUTTON ELEMENT
// ============================================================================

struct ButtonElement : DrawElement {
    enum class State {
        Normal,
        Hovered,
        Pressed,
        Disabled
    };

    struct Style {
        Color normalColor{LIGHTGRAY};
        Color hoverColor{GRAY};
        Color pressedColor{DARKGRAY};
        Color disabledColor{Color{200, 200, 200, 255}};
        
        Color textNormalColor{BLACK};
        Color textHoverColor{BLACK};
        Color textPressedColor{WHITE};
        Color textDisabledColor{DARKGRAY};
        
        Color borderColor{DARKGRAY};
        float borderThickness = 2.0f;
        float cornerRadius = 5.0f;
        
        Padding padding{10.0f, 20.0f};
    };

    std::string label;
    int fontSize{20};
    Font font;
    bool useCustomFont{false};
    
    Style style;
    State currentState{State::Normal};
    bool isEnabled{true};
    
    std::function<void()> onClick;
    
    // Internal state
    Rectangle absoluteBounds;
    Vector2 textOffset{0, 0};
    bool wasPressed{false};

    // Constructors
    ButtonElement(std::string text, Rectangle rect, std::function<void()> callback = nullptr)
        : DrawElement(rect), label(std::move(text)), font(GetFontDefault()), onClick(std::move(callback)) {
        calculateTextOffset();
    }

    ButtonElement(std::string text, float width, float height, std::function<void()> callback = nullptr)
        : DrawElement(Rectangle{0, 0, width, height}), label(std::move(text)), 
          font(GetFontDefault()), onClick(std::move(callback)) {
        calculateTextOffset();
    }

    // Auto-size button based on text
    static std::unique_ptr<ButtonElement> createAutoSize(const std::string& text, int fontSize = 20, 
                                                         const Padding& padding = Padding(10.0f, 20.0f),
                                                         std::function<void()> callback = nullptr) {
        float textWidth = static_cast<float>(MeasureText(text.c_str(), fontSize));
        float width = textWidth + padding.totalHorizontal();
        float height = static_cast<float>(fontSize) + padding.totalVertical();
        
        auto btn = std::make_unique<ButtonElement>(text, width, height, std::move(callback));
        btn->fontSize = fontSize;
        btn->style.padding = padding;
        btn->calculateTextOffset();
        return btn;
    }

    void setLabel(const std::string& newLabel) {
        label = newLabel;
        calculateTextOffset();
    }

    void setEnabled(bool enabled) {
        isEnabled = enabled;
        if (!enabled) {
            currentState = State::Disabled;
        }
        else if (currentState == State::Disabled) {
            currentState = State::Normal;
        }
    }

    void setCallback(std::function<void()> callback) {
        onClick = std::move(callback);
    }

    void update(Vector2 parentPos) override {
        if (!isEnabled) {
            currentState = State::Disabled;
            wasPressed = false;
            return;
        }

        absoluteBounds = {
            parentPos.x + bounds.x,
            parentPos.y + bounds.y,
            bounds.width,
            bounds.height
        };

        Vector2 mousePos = GetMousePosition();
        bool isHovered = CheckCollisionPointRec(mousePos, absoluteBounds);
        bool isMousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        bool isMouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        bool isMouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

        if (isHovered && isMousePressed) {
            wasPressed = true;
            currentState = State::Pressed;
        }
        else if (isMouseReleased) {
            if (wasPressed && isHovered && onClick) {
                onClick();
            }
            wasPressed = false;
            currentState = isHovered ? State::Hovered : State::Normal;
        }
        else if (wasPressed && isMouseDown) {
            currentState = State::Pressed;
        }
        else if (isHovered && !isMouseDown) {
            currentState = State::Hovered;
        }
        else {
            currentState = State::Normal;
        }
    }

    void draw(Vector2 parentPos) override {
        Rectangle drawRect = {
            parentPos.x + bounds.x,
            parentPos.y + bounds.y,
            bounds.width,
            bounds.height
        };

        Color bgColor = getBackgroundColor();
        DrawRectangleRounded(drawRect, style.cornerRadius / bounds.height, 8, bgColor);

        if (style.borderThickness > 0) {
            DrawRectangleRoundedLines(drawRect, style.cornerRadius / bounds.height, 8, style.borderColor);
        }

        Color textColor = getTextColor();
        Vector2 textPos = {
            drawRect.x + textOffset.x,
            drawRect.y + textOffset.y
        };

        if (useCustomFont) {
            DrawTextEx(font, label.c_str(), textPos, static_cast<float>(fontSize), 1.0f, textColor);
        }
        else {
            DrawText(label.c_str(), static_cast<int>(textPos.x), static_cast<int>(textPos.y), 
                    fontSize, textColor);
        }
    }

private:
    void calculateTextOffset() {
        float textWidth = useCustomFont ? 
            MeasureTextEx(font, label.c_str(), static_cast<float>(fontSize), 1.0f).x :
            static_cast<float>(MeasureText(label.c_str(), fontSize));
        
        textOffset.x = (bounds.width - textWidth) * 0.5f;
        textOffset.y = (bounds.height - static_cast<float>(fontSize)) * 0.5f;
    }

    Color getBackgroundColor() const {
        switch (currentState) {
            case State::Hovered: return style.hoverColor;
            case State::Pressed: return style.pressedColor;
            case State::Disabled: return style.disabledColor;
            default: return style.normalColor;
        }
    }

    Color getTextColor() const {
        switch (currentState) {
            case State::Hovered: return style.textHoverColor;
            case State::Pressed: return style.textPressedColor;
            case State::Disabled: return style.textDisabledColor;
            default: return style.textNormalColor;
        }
    }
};

// ============================================================================
// FRAME - Enhanced for Dictionary Layout
// ============================================================================

struct Frame : DrawElement {
    Color color{LIGHTGRAY};
    Padding padding;
    Margin margin;
    Alignment align;

    enum class Layout { Overlay, Vertical, Horizontal };
    Layout layoutMode{Layout::Overlay};
    float spacing{10.0f};

    std::vector<std::unique_ptr<DrawElement>> Children;
    Rectangle drawArea;

    Frame(Rectangle rect, Color c = LIGHTGRAY, Padding p = {},
          Margin m = {}, Alignment a = {})
        : DrawElement(rect), color(c), padding(p), margin(m), align(a),
          drawArea{
              rect.x + p.left,
              rect.y + p.top,
              rect.width - p.totalHorizontal(),
              rect.height - p.totalVertical()
          } {
    }

    ~Frame() override = default;

    Rectangle getDrawArea(Vector2 parentPos) {
        drawArea = {
            parentPos.x + bounds.x + padding.left,
            parentPos.y + bounds.y + padding.top,
            bounds.width - padding.totalHorizontal(),
            bounds.height - padding.totalVertical()
        };
        return drawArea;
    }

    void AddChild(std::unique_ptr<DrawElement> child) {
        if (child) {
            Children.push_back(std::move(child));
        }
    }

    // Remove and transfer ownership - caller MUST take ownership
    // If you want to just delete the child, use deleteChild() instead
    [[nodiscard("Ownership must be taken or element will be destroyed")]]
    std::unique_ptr<DrawElement> removeChild(size_t index) {
        if (index >= Children.size()) {
            return nullptr;
        }

        std::unique_ptr<DrawElement> removed = std::move(Children[index]);
        Children.erase(Children.begin() + static_cast<long>(index));
        return removed;
    }

    [[nodiscard("Ownership must be taken or element will be destroyed")]]
    std::unique_ptr<DrawElement> removeChild(DrawElement* child) {
        auto it = std::find_if(Children.begin(), Children.end(),
            [child](const std::unique_ptr<DrawElement>& ptr) {
                return ptr.get() == child;
            });

        if (it != Children.end()) {
            std::unique_ptr<DrawElement> removed = std::move(*it);
            Children.erase(it);
            return removed;
        }
        return nullptr;
    }

    // Delete child immediately (clearer intent when you just want to remove)
    void deleteChild(size_t index) {
        if (index < Children.size()) {
            Children.erase(Children.begin() + static_cast<long>(index));
        }
    }

    void deleteChild(DrawElement* child) {
        auto it = std::find_if(Children.begin(), Children.end(),
            [child](const std::unique_ptr<DrawElement>& ptr) {
                return ptr.get() == child;
            });
        
        if (it != Children.end()) {
            Children.erase(it);
        }
    }

    void clearChildren() {
        Children.clear();
    }

    [[nodiscard]] DrawElement* getChild(size_t index) const {
        if (index < Children.size()) {
            return Children[index].get();
        }
        return nullptr;
    }

    [[nodiscard]] size_t getChildCount() const {
        return Children.size();
    }

    void update(Vector2 parentPos) override {
        Rectangle frameBounds = {
            parentPos.x + bounds.x + margin.left,
            parentPos.y + bounds.y + margin.top,
            bounds.width - margin.totalHorizontal(),
            bounds.height - margin.totalVertical()
        };

        if (layoutMode == Layout::Overlay) {
            for (auto& child : Children) {
                child->update({
                    frameBounds.x + child->bounds.x,
                    frameBounds.y + child->bounds.y
                });
            }
        }
        else {
            processStackedChildren(parentPos, true);
        }
    }

    void draw(Vector2 parentPos) override {
        Rectangle frameBounds = {
            parentPos.x + bounds.x + margin.left,
            parentPos.y + bounds.y + margin.top,
            bounds.width - margin.totalHorizontal(),
            bounds.height - margin.totalVertical()
        };
        DrawRectangleRec(frameBounds, color);

        if (Children.empty()) return;

        if (layoutMode == Layout::Overlay) {
            drawOverlayChildren(frameBounds);
        }
        else {
            processStackedChildren(parentPos, false);
        }
    }

private:
    void drawOverlayChildren(const Rectangle& frameBounds) {
        for (auto& child : Children) {
            child->draw({
                frameBounds.x + child->bounds.x,
                frameBounds.y + child->bounds.y
            });
        }
    }

    void processStackedChildren(Vector2 parentPos, bool isUpdate) {
        Rectangle contentArea = getDrawArea(parentPos);
        float currentX = contentArea.x;
        float currentY = contentArea.y;

        for (size_t i = 0; i < Children.size(); ++i) {
            auto& child = Children[i];

            if (child->bounds.width <= 0.0f) {
                child->bounds.width = contentArea.width;
                child->updateBounds();
            }

            Vector2 childPos = calculateChildPosition(child.get(), contentArea, currentX, currentY);
            isUpdate ? child->update(childPos) : child->draw(childPos);

            updateStackPosition(child.get(), currentX, currentY, i < Children.size() - 1);
        }
    }

    [[nodiscard]] Vector2 calculateChildPosition(const DrawElement* child, const Rectangle& contentArea,
        float stackX, float stackY) const {
        Vector2 pos = { stackX, stackY };

        if (layoutMode == Layout::Vertical) {
            switch (align.hAlign) {
            case Alignment::Horizontal::Center:
                pos.x = contentArea.x + (contentArea.width - child->bounds.width) * 0.5f;
                break;
            case Alignment::Horizontal::Right:
                pos.x = contentArea.x + contentArea.width - child->bounds.width;
                break;
            default:
                break;
            }
        }
        else if (layoutMode == Layout::Horizontal) {
            switch (align.vAlign) {
            case Alignment::Vertical::Center:
                pos.y = contentArea.y + (contentArea.height - child->bounds.height) * 0.5f;
                break;
            case Alignment::Vertical::Bottom:
                pos.y = contentArea.y + contentArea.height - child->bounds.height;
                break;
            default:
                break;
            }
        }

        return pos;
    }

    void updateStackPosition(const DrawElement* child, float& currentX, float& currentY, bool hasNext) const {
        if (layoutMode == Layout::Vertical) {
            currentY += child->bounds.height;
            if (hasNext) currentY += spacing;
        }
        else if (layoutMode == Layout::Horizontal) {
            currentX += child->bounds.width;
            if (hasNext) currentX += spacing;
        }
    }
};

// ============================================================================
// SPACER ELEMENT
// ============================================================================

struct SpacerElement : public DrawElement {
    SpacerElement(float w, float h) {
        bounds = {0, 0, w, h};
    }

    void draw(Vector2 parentPos) override {
        (void)parentPos;
    }

    static std::unique_ptr<SpacerElement> createHorizontal(float width) {
        return std::make_unique<SpacerElement>(width, 0.0f);
    }

    static std::unique_ptr<SpacerElement> createVertical(float height) {
        return std::make_unique<SpacerElement>(0.0f, height);
    }
};

// Backwards compatibility typedef
using spacerElement = SpacerElement;

#endif //UI_H
