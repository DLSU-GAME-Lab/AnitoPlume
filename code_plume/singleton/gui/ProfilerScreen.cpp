#include "ProfilerScreen.hpp"
#include "singleton/PlumeManager.hpp"
#include <numeric>
#include <cmath>

ProfilerScreen::ProfilerScreen() : GUIScreen("Profiler")
{
	for (int n = 0; n < 100; n++)
	{
		frameCount[n] = 0;
		memoryUsage[n] = 0;
	}

	unsigned int freeSphereCount = PlumeManager::getInstance()->getFreeSphereCount();
	unsigned int subsphereCount = PlumeManager::getInstance()->getSubsphereCount();
	this->updateProfilerData(freeSphereCount, subsphereCount);
}

ProfilerScreen::~ProfilerScreen()
{

}

void ProfilerScreen::drawGUI()
{
	// Update profiler data only if particle count has changed
	unsigned int smokeLayersCount = PlumeManager::getInstance()->getSmokeLayersCount();
	unsigned int freeSphereCount = PlumeManager::getInstance()->getFreeSphereCount();
	unsigned int subsphereCount = PlumeManager::getInstance()->getSubsphereCount();
	unsigned int particleCount = freeSphereCount + subsphereCount;
	this->updateProfilerData(freeSphereCount, subsphereCount);

	ImGui::Begin("Profiler", &this->enabled, ImGuiWindowFlags_AlwaysAutoResize);
	
	std::string smokeLayersStr = "Smoke Layers: " + std::to_string(smokeLayersCount);
	std::string particleCountStr = "Particle Count: " + std::to_string(particleCount);
	ImGui::Text(smokeLayersStr.c_str());
	ImGui::Text(particleCountStr.c_str());
	ImGui::Separator();

	ImGui::PlotLines("##FPS", frameCount, 100, 0, "Frame Rate (FPS)", 0, FLT_MAX, ImVec2(400, 100));
	std::string framesMinMaxAve = "Min: " + std::to_string((int)(*std::min_element(frameData.begin(), frameData.end()))) +
		" | Max: " + std::to_string((int)(*std::max_element(frameData.begin(), frameData.end()))) +
		" | Average: " + std::to_string(std::accumulate(frameData.begin(), frameData.end(), 0) / frameData.size());
	ImGui::Text(framesMinMaxAve.c_str());
	ImGui::Spacing();

	ImGui::PlotLines("##Memory", memoryUsage, 100, 0, "Memory (MB)", 0, FLT_MAX, ImVec2(400, 100));
	std::string memMinMaxAve = "Max: " + std::to_string((*std::max_element(memoryData.begin(), memoryData.end()) / 1048576.0f)) +
		" | Average: " + std::to_string((std::accumulate(memoryData.begin(), memoryData.end(), 0) / memoryData.size()) / 1048576.0f);
	ImGui::Text(memMinMaxAve.c_str());

	ImGui::End();
}

void ProfilerScreen::updateProfilerData(unsigned int freeSphereCount, unsigned int subsphereCount)
{
	unsigned int particleCount = freeSphereCount + subsphereCount;
	if (particleCount != lastParticleCount)
	{
		// Shift data to the left
		for (int i = 0; i < 99; i++)
		{
			frameCount[i] = frameCount[i + 1];
			memoryUsage[i] = memoryUsage[i + 1];
		}
		// Add new data at the end
		frameCount[99] = std::fmin(120, ImGui::GetIO().Framerate);
		frameData.push_back(frameCount[99]);

		memoryUsage[99] = static_cast<float>(freeSphereCount) * sizeof(free_sphere_params);
		memoryUsage[99] += static_cast<float>(subsphereCount) * sizeof(subsphere_params);
		memoryData.push_back(memoryUsage[99]);

		lastParticleCount = particleCount;
	}

}
