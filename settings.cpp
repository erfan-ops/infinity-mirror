#include "settings.h"
#include <fstream>
#include <nlohmann/json.hpp>

// Function to load settings from a JSON file
Settings loadSettings(const std::string& filename) {
	std::ifstream file(filename);
	nlohmann::json j;
	file >> j;

	Settings settings;

	settings.targetFPS = j["fps"];
	settings.colors = j["colors"].get<std::vector<std::array<float, 4>>>();
	settings.colorSpeed = j["color-speed"];
	settings.bgColor = j["background-color"].get<std::array<float, 4>>();
	settings.iterations = j["iteration-limit"];
	settings.scaleMultiplier = j["distance-interval-factor"];
	settings.borderWidth = j["border-width"];

	return settings;
}
