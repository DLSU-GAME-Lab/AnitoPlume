#pragma once

#include <unordered_map>
#include <string>

struct arc { float arcStart, arcEnd; };

class direction_tracker
{
private:
	std::unordered_map<std::string, arc> danger_zone;

public:
	void loadData(std::string filePath);
    arc get_arc(std::string name);
    void show_gui();
};