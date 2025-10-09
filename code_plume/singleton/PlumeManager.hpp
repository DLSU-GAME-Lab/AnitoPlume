#pragma once
#include "vector"
#include "unordered_map"
#include "string"
#include "scenes/sources/smoke/Plume.hpp"

class PlumeManager
{
private:
	std::vector<Plume> vecPlumes;
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
public:
	std::vector<Plume>& getPlumes();
	void setTStep(float fTStep);
//singleton Stuff
private:
	PlumeManager();
	PlumeManager(const PlumeManager&) {};
	PlumeManager operator=(const PlumeManager&) {};
	static PlumeManager* sharedInstance;
};

