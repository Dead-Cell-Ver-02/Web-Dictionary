//
// Created by SAGNIK on 30-09-2025.
//

#include "screenManager.h"

int main() {
    // Screen dimensions
    const float SCREEN_WIDTH = 1920.0f;
    const float SCREEN_HEIGHT = 1080.0f;

    // Create the screen manager
    screenManager manager(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Initialize (creates window, loads screens)
    manager.initialize();

    // Run the main loop (handles updates, drawing, and screen transitions)
    manager.run();

    // Cleanup is handled automatically by the ScreenManager destructor
    return 0;
}
