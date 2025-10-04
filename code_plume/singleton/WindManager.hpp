#pragma once
#include "scenes/sources/smoke/wind_structure.hpp"
#include <vector>

class WindManager
{
private:
	std::vector<int> wind_altitudes;
	std::vector<wind_structure> winds;

public:
	static WindManager* getInstance();
	static void initialize();
	static void destroy();
	vcl::vec3 compute_wind_vector(float height);

private:
	WindManager();
	~WindManager();
	WindManager(const WindManager&) {};
	WindManager operator=(const WindManager&) {};
	static WindManager* sharedInstance;
};