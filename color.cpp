#include "Color.h"

#include <algorithm>


Color::Color(std::vector<std::array<float, 4>> colors, float speed)
    : colors(colors), speed(speed), cIdx(0), t(0.0f) {
    // Initialize the current and next colors
    currentColor = colors[cIdx];
    nextColor = colors[(cIdx + 1) % colors.size()];
}

void Color::update(float dt) {
    // Update the interpolation factor
    t += dt * speed;

    // Interpolate between the current and next color
    std::array<float, 4> newColor;
    for (int i = 0; i < 4; ++i) {
        newColor[i] = currentColor[i] + (nextColor[i] - currentColor[i]) * t;
    }

    // Clamp the color values to the range [0, 1] (OpenGL expects colors in this range)
    for (int i = 0; i < 4; ++i) {
        newColor[i] = std::clamp(newColor[i], 0.0f, 1.0f);
    }

    // Check if we have reached the next color
    if (t >= 1.0f) {
        t = 0.0f;
        cIdx = (cIdx + 1) % colors.size();
        currentColor = colors[cIdx];
        nextColor = colors[(cIdx + 1) % colors.size()];
    }

    r = newColor[0];
    g = newColor[1];
    b = newColor[2];
    a = newColor[3];
}
