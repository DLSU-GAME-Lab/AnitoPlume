#pragma once

#include "vcl/vcl.hpp"
#include <unordered_map>
#include <string>
#include <vector>

class PlumeTracker
{
private:
	std::vector<std::string> locationNames;
	std::vector<float> arcStart;
	std::vector<float> arcEnd;

	std::vector<vcl::vec3> positions;
	std::vector<float> radii;
	float coneRadius;

	const int stepSize = 20;
	const float minAltStep = 100.0f;
	float altStep = 1000.0f;

	float windAngle = 0.0f;
	vcl::vec3 windVector = { 0.0f, 0.0f, 0.0f };

private:
	PlumeTracker();
	~PlumeTracker();
	PlumeTracker(const PlumeTracker&) {};
	PlumeTracker operator=(const PlumeTracker&) {};
	static PlumeTracker* sharedInstance;

public:
	static PlumeTracker* getInstance();
	static void initialize();
	static void destroy();

	void checkSmokePosition(unsigned int i);
	void resetPlumePositions();
	void loadData(std::string filePath);

	void setWindDirection(vcl::vec3 wind_vector);
	void setPlumePositions(unsigned int index, vcl::vec3 position, float radius);

	std::vector<std::string>& getLocationNames();
	std::vector<vcl::vec3>& getPositions();
	std::vector<float>& getRadii();
	float getConeRadius() const;
	
	std::vector<std::string> getIntersectingLocations();
	vcl::vec3 getWindDirection() const;
	float getWindDirectionAngle() const;

};