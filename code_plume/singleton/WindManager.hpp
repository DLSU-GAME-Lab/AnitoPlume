#pragma once
#include "scenes/sources/smoke/wind_structure.hpp"
#include <vector>

class WindManager
{
private:
	std::vector<int> wind_altitudes;
	std::vector<wind_structure> winds;

	float max_altitude;
	float altitude_step;
	int altitude_size;
	float linear_wind_base;

	std::vector<int> deg_angle; // UI wind angles
	bool all_angles; // UI toggle

public:
	static WindManager* getInstance();
	static void initialize();
	static void destroy();
	vcl::vec3 compute_wind_vector(float height);

public:
	std::vector<int>& getWindAlts();
	std::vector<wind_structure>& getWinds();
	std::vector<int>& getDegAngle();
	float getMaxAlt();
	float getAltStep();
	int getAltSize();
	float getLinearWindBase();
	void setLinearWindBase(float fLinearWindBase);

private:
	WindManager();
	WindManager(const WindManager&) {};
	WindManager operator=(const WindManager&) {};
	static WindManager* sharedInstance;
};