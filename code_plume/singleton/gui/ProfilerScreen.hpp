#pragma once
#include "GUIScreen.hpp"
#include <vector>

class ProfilerScreen : public GUIScreen
{
private:
	float frameCount[100] = {};
	float memoryUsage[100] = {};
	unsigned int lastParticleCount = 0;

	std::vector<float> particleData;
	std::vector<float> frameData;
	std::vector<float> memoryData;

protected:
	ProfilerScreen();
	~ProfilerScreen();

	virtual void drawGUI() override;

	void updateProfilerData(unsigned int freeSphereCount, unsigned int subsphereCount);
	void exportToCSV();
	void clearData();

	friend class GUIManager;
};