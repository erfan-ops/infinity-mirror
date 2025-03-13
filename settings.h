#pragma once

#include <string>
#include <vector>
#include <array>

// Define a struct to represent your settings
struct Settings {
	float targetFPS;
	std::vector<std::array<float, 4>> colors;
	float colorSpeed;
	std::array<float, 4> bgColor;
	int iterations;
	float scaleMultiplier;
	float borderWidth;
};

// Function to load settings from a JSON file
Settings loadSettings(const std::string& filename);
