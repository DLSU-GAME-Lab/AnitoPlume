#pragma once
#include "vector"
#include "unordered_map"
#include "string"
#include "scenes/sources/smoke/Plume.hpp"

enum class SimulatorState { Stopped, Playing, Paused };

class PlumeManager
{
private:
	SimulatorState state;
	vcl::timer_event timer;
	float t_step;
	unsigned int frame_count;

	std::vector<Plume> plumes;
	std::vector<bool> toUpdate;
	std::vector<int> wind_altitudes;
	std::vector<wind_structure> winds;

	float min_altitude;
	float max_altitude;
	float altitude_step;
	int altitude_size;

	std::vector<int> deg_angle; // UI wind angles
	bool all_angles; // UI toggle

public:
	static PlumeManager* getInstance();
	static void initialize();
	static void destroy();

public:
	void createPlume(unsigned int id, std::string ventName, vcl::vec3 ventLoc, EruptionParams eruptParams);
	void setupTransitionValues(int maxSmoke, float transitionSpeed, float transitionDelay);
	void update();

	bool getToUpdate(unsigned int plumeID);
	void setToUpdate(bool toUpdate);
	void setToUpdate(unsigned int plumeID, bool toUpdate);

	void setTimerScale(float scale);
	void playSimulation();
	void pauseSimulation();
	void stopSimulation();
	void reset();

	SimulatorState getState() const;
	std::vector<Plume>& getPlumes();

public:
	void setWindIntensity(unsigned int index, int intensity);
	void setWindAngle(unsigned int index, int angle);
	void setWind(unsigned int index, int intensity, int angle);

	void setLinearWind(float linearWindBase);
	void setAllWindIntensities(int intensity);
	void setAllWindAngles(int angle);
	void setAllWinds(int intensity, int angle);

	vcl::vec3 computeWindVector(float height);
	vcl::vec3 getAverageWindDirection();
	std::vector<int>& getWindAlts();
	std::vector<wind_structure>& getWinds();
	std::vector<int>& getDegAngle();
	float getMaxAlt();
	float getAltStep();
	int getAltSize();

//singleton Stuff
private:
	PlumeManager();
	PlumeManager(const PlumeManager&) {};
	PlumeManager operator=(const PlumeManager&) {};
	static PlumeManager* sharedInstance;
};

