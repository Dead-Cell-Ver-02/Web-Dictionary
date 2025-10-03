#ifndef SCREEN_H
#define SCREEN_H

// Base screen class
class Screen {
public:
    virtual ~Screen() = default;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void handleInput() {}
};

#endif // SCREEN_H
