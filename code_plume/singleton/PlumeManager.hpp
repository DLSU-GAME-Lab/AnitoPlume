#pragma once
#include "vector"
#include "unordered_map"
#include "string"
#include "scenes/sources/smoke/Plume.hpp"

class PlumeManager
{
private:
	vcl::timer_event timer;
	std::vector<Plume> vecPlumes;
	std::vector<int> wind_altitudes;
	std::vector<wind_structure> winds;

	float max_altitude;
	float altitude_step;
	int altitude_size;
	float linear_wind_base;

	std::vector<int> deg_angle; // UI wind angles
	bool all_angles; // UI toggle

public:
	static PlumeManager* getInstance();
	static void initialize();
	static void destroy();

public:
	void createPlume(vcl::vec3 ventLoc);
	void setupTransitionValues(int maxSmoke, float transitionSpeed, float transitionDelay);
	void removeSmokeLayers();
	void update(unsigned int dFrameCount);
	void reset();
	std::vector<Plume>& getPlumes();
	void setTStep(float fTStep);

public:
	vcl::vec3 computeWindVector(float height);
	vcl::vec3 getAverageWindDirection();
	std::vector<int>& getWindAlts();
	std::vector<wind_structure>& getWinds();
	std::vector<int>& getDegAngle();
	float getMaxAlt();
	float getAltStep();
	int getAltSize();
	float getLinearWindBase();
	void setLinearWindBase(float fLinearWindBase);

//singleton Stuff
private:
	PlumeManager();
	PlumeManager(const PlumeManager&) {};
	PlumeManager operator=(const PlumeManager&) {};
	static PlumeManager* sharedInstance;
};

