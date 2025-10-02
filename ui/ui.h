//
// Created by SAGNIK on 30-09-2025.
//

#ifndef UI_H
#define UI_H

#include <memory>
#include <stdexcept>
#include <raylib.h>
#include <utility>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

// ============================================================================
// UTILITY STRUCTURES
// ============================================================================

struct Padding {
    float top = 0, right = 0, bottom = 0, left = 0;

    Padding(float all = 0.0f) : top(all), right(all), bottom(all), left(all) {}
    Padding(float vertical, float horizontal) : top(vertical), right(horizontal), bottom(vertical), left(horizontal) {}
    Padding(float t, float r, float b, float l) : top(t), right(r), bottom(b), left(l) {}

    [[nodiscard]] float totalHorizontal() const { return left + right; }
    [[nodiscard]] float totalVertical() const { return top + bottom; }
};

struct Margin {
    float top = 0, right = 0, bottom = 0, left = 0;

    Margin(float all = 0) : top(all), right(all), bottom(all), left(all) {}
    Margin(float t, float r, float b, float l) : top(t), right(r), bottom(b), left(l) {}

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

struct drawElement {
    Rectangle bounds;

    drawElement() : bounds{ 0, 0, 0, 0 } {}
    explicit drawElement(const Rectangle& rect) : bounds(rect) {}
    virtual ~drawElement() = default;

    virtual void draw(Vector2 parentPos) = 0;
    virtual void updateBounds() {}

    // Helper methods
    [[nodiscard]] Vector2 getSize() const { return { bounds.width, bounds.height }; }
    void setPosition(float x, float y) { bounds.x = x; bounds.y = y; }
    void setSize(float width, float height) { bounds.width = width; bounds.height = height; }
};

// ============================================================================
// TEXT ELEMENT - Optimized for Dictionary Usage
// ============================================================================

struct TextElement : drawElement {
    std::string txt;
    int fontSize{ 20 };  // Changed to int for consistency with raylib
    Color color{ BLACK };
    Vector2 offset{ 0, 0 };
    Font font;
    bool useCustomFont{ false };
    bool useWrapText{ false };
    float wrapLength = 0;

    // Text wrapping properties
    std::vector<std::string> lines;
    float lineSpacing = 5.0f;
    float characterSpacing = 1.0f;

    // Constructors
    TextElement(std::string text, int fs, Color c, Vector2 off = { 0, 0 })
        : txt(std::move(text)), fontSize(fs), color(c), offset(off), font(GetFontDefault()) {
        calculateBounds();
    }

    TextElement(std::string text, int fs, Color c, const Font& f, Vector2 off = { 0, 0 })
        : txt(std::move(text)), fontSize(fs), color(c), offset(off), font(f), useCustomFont(true) {
        calculateBounds();
    }

    // Setter methods for dynamic updates
    void setText(const std::string& newText) {
        if (txt != newText) {
            txt = newText;
            if (useWrapText) {
                wrap_text();
            }
            else {
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
        }
        else {
            calculateBounds();
        }
    }

    // FIXED: Correct text wrapping algorithm
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
            // Build test line WITHOUT modifying currentLine yet
            std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
            float testWidth = measureLineWidth(testLine);

            if (testWidth <= wrapLength || currentLine.empty()) {
                // It fits! Now update currentLine
                currentLine = testLine;
            }
            else {
                // Doesn't fit - save current line and start new one
                if (!currentLine.empty()) {
                    lines.push_back(currentLine);
                    bounds.width = std::max(bounds.width, measureLineWidth(currentLine));
                }

                currentLine = word;

                // Handle single words longer than wrap length
                if (measureLineWidth(currentLine) > wrapLength) {
                    lines.push_back(currentLine);
                    bounds.width = std::max(bounds.width, measureLineWidth(currentLine));
                    currentLine.clear();
                }
            }
        }

        // Add the last line
        if (!currentLine.empty()) {
            lines.push_back(currentLine);
            bounds.width = std::max(bounds.width, measureLineWidth(currentLine));
        }

        // Calculate total height with line spacing
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
// FRAME - Enhanced for Dictionary Layout
// ============================================================================

struct Frame : drawElement {
    Color color{ LIGHTGRAY };
    Padding padding;
    Margin margin;
    Alignment align;

    enum class Layout { Overlay, Vertical, Horizontal };
    Layout layoutMode{ Layout::Overlay };
    float spacing{ 10.0f };

    std::vector<std::unique_ptr<drawElement>> Children;
    Rectangle drawArea;

    Frame(Rectangle rect, Color c = LIGHTGRAY, Padding p = {},
        Margin m = {}, Alignment a = {})
        : drawElement(rect), color(c), padding(p), margin(m), align(a),
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

    // FIXED: Consistent smart pointer interface
    void AddChild(std::unique_ptr<drawElement> child) {
        if (child) {
            Children.push_back(std::move(child));
        }
    }

    // FIXED: Return ownership when removing
    std::unique_ptr<drawElement> removeChild(size_t index) {
        if (index >= Children.size()) {
            return nullptr;
        }

        std::unique_ptr<drawElement> removed = std::move(Children[index]);
        Children.erase(Children.begin() + static_cast<long>(index));
        return removed;
    }

    // FIXED: Remove by pointer - safer version
    std::unique_ptr<drawElement> removeChild(drawElement* child) {
        auto it = std::find_if(Children.begin(), Children.end(),
            [child](const std::unique_ptr<drawElement>& ptr) {
                return ptr.get() == child;
            });

        if (it != Children.end()) {
            std::unique_ptr<drawElement> removed = std::move(*it);
            Children.erase(it);
            return removed;
        }
        return nullptr;
    }

    // Get raw pointer to child (for reading only)
    [[nodiscard]] drawElement* getChild(size_t index) const {
        if (index < Children.size()) {
            return Children[index].get();
        }
        return nullptr;
    }

    [[nodiscard]] size_t getChildCount() const {
        return Children.size();
    }

    void draw(Vector2 parentPos) override {
        // Draw frame background
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
            drawStackedChildren(parentPos);
        }
    }

private:
    // FIXED: Removed const (draw() is non-const)
    void drawOverlayChildren(const Rectangle& frameBounds) {
        for (auto& child : Children) {
            child->draw({
                frameBounds.x + child->bounds.x,
                frameBounds.y + child->bounds.y
                });
        }
    }

    void drawStackedChildren(Vector2 parentPos) {
        Rectangle contentArea = getDrawArea(parentPos);
        float currentX = contentArea.x;
        float currentY = contentArea.y;

        for (size_t i = 0; i < Children.size(); ++i) {
            auto& child = Children[i];

            // Auto-width for text wrapping
            if (child->bounds.width <= 0.0f) {
                child->bounds.width = contentArea.width;
                child->updateBounds();
            }

            Vector2 childPos = calculateChildPosition(child.get(), contentArea, currentX, currentY);
            child->draw(childPos);

            // Update position for next child
            updateStackPosition(child.get(), currentX, currentY, i < Children.size() - 1);
        }
    }

    [[nodiscard]] Vector2 calculateChildPosition(const drawElement* child, const Rectangle& contentArea,
        float stackX, float stackY) const {
        Vector2 pos = { stackX, stackY };

        // Apply alignment based on layout mode
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

    void updateStackPosition(const drawElement* child, float& currentX, float& currentY, bool hasNext) const {
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

// ============================================================================
// SPACER ELEMENT
// ============================================================================

struct spacerElement : public drawElement {
    spacerElement(float w, float h) {
        bounds = { 0, 0, w, h };
    }

    void draw(Vector2 parentPos) override {
        // Spacers are invisible - they just occupy space
        (void)parentPos;
    }

    // FIXED: Return smart pointers
    static std::unique_ptr<spacerElement> createHorizontal(float width) {
        // --- FIX WAS HERE --- Changed 0 to 0.0f
        return std::make_unique<spacerElement>(width, 0.0f);
    }

    static std::unique_ptr<spacerElement> createVertical(float height) {
        // --- FIX WAS HERE --- Changed 0 to 0.0f
        return std::make_unique<spacerElement>(0.0f, height);
    }
};

#endif //DICTIONARY_DESIGN_H