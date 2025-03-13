#pragma once

#include <vector>
#include <array>

class Color {
private:
    std::vector<std::array<float, 4>> colors;
    float speed;
    size_t cIdx;
    float t;
    std::array<float, 4> currentColor;
    std::array<float, 4> nextColor;

public:
    float r, g, b, a;   // Current interpolated color

    Color(std::vector<std::array<float, 4>> colors, float speed);

    void update(float dt);

};
