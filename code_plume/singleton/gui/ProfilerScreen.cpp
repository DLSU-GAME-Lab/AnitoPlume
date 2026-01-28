#include "ProfilerScreen.hpp"
#include "singleton/PlumeManager.hpp"
#include <numeric>
#include <cmath>
#include <fstream>

ProfilerScreen::ProfilerScreen() : GUIScreen("Profiler")
{
	this->clearData();
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

	if (particleCount != lastParticleCount)
	{
		this->updateProfilerData(freeSphereCount, subsphereCount);
		lastParticleCount = particleCount;
	}
	

	ImGui::Begin("Profiler", &this->enabled, ImGuiWindowFlags_AlwaysAutoResize);
	
	std::string smokeLayersStr = "Smoke Layers: " + std::to_string(smokeLayersCount);
	std::string particleCountStr = "Particle Count: " + std::to_string(particleCount);
	ImGui::Text(smokeLayersStr.c_str());
	ImGui::Text(particleCountStr.c_str());
	ImGui::Separator();

	unsigned int framesMin = *std::min_element(frameData.begin(), frameData.end());
	unsigned int framesMax = *std::max_element(frameData.begin(), frameData.end());
	unsigned int framesAve = std::accumulate(frameData.begin(), frameData.end(), 0) / frameData.size();

	ImGui::PlotLines("##FPS", frameCount, 100, 0, "Frame Rate (FPS)", 0, FLT_MAX, ImVec2(400, 100));
	std::string framesMinMaxAve =
		"Min: " + std::to_string(framesMin) +
		" | Max: " + std::to_string(framesMax) +
		" | Average: " + std::to_string(framesAve);
	ImGui::Text(framesMinMaxAve.c_str());
	ImGui::Spacing();

	float memoryMax = *std::max_element(memoryData.begin(), memoryData.end());
	float memoryAve = (std::accumulate(memoryData.begin(), memoryData.end(), 0.0f) / memoryData.size());
	
	ImGui::PlotLines("##Memory", memoryUsage, 100, 0, "Particle Memory (MB)", 0, FLT_MAX, ImVec2(400, 100));
	std::string memMinMaxAve =
		"Max: " + std::to_string(memoryMax) +
		" | Average: " + std::to_string(memoryAve);
	ImGui::Text(memMinMaxAve.c_str());
	ImGui::Spacing();

	if (ImGui::Button("Export to CSV")) this->exportToCSV();
	ImGui::SameLine();
	if (ImGui::Button("Clear data")) this->clearData();

	ImGui::End();
}

void ProfilerScreen::updateProfilerData(unsigned int freeSphereCount, unsigned int subsphereCount)
{
	unsigned int particleCount = freeSphereCount + subsphereCount;

	// Shift data to the left
	for (int i = 0; i < 99; i++)
	{
		frameCount[i] = frameCount[i + 1];
		memoryUsage[i] = memoryUsage[i + 1];
	}
	// Add new data at the end
	particleData.push_back(static_cast<float>(particleCount));
	frameCount[99] = std::fmin(120, ImGui::GetIO().Framerate);
	frameData.push_back(frameCount[99]);

	memoryUsage[99] = static_cast<float>(freeSphereCount) * sizeof(free_sphere_params);
	memoryUsage[99] += static_cast<float>(subsphereCount) * sizeof(subsphere_params);
	memoryUsage[99] /= 1048576.0f; // Convert to MB
	memoryData.push_back(memoryUsage[99]);
}

void ProfilerScreen::exportToCSV()
{
	std::fstream csvFile;

	// opens an existing csv file or creates a new file.
	csvFile.open("profiler_data.csv", std::ios::out);

	// Read the input and inster to file.
	csvFile << " particle_count, frames_per_second, particle_memory_mb" << "\n";
	for (int i = 0; i < particleData.size(); i++)
	{
		csvFile << particleData[i] << ", " << frameData[i] << ", " << memoryData[i] << "\n";
	}

	csvFile.close();
}

void ProfilerScreen::clearData()
{
	for (int n = 0; n < 100; n++)
	{
		frameCount[n] = 0;
		memoryUsage[n] = 0;
	}

	particleData.clear();
	frameData.clear();
	memoryData.clear();

	unsigned int freeSphereCount = PlumeManager::getInstance()->getFreeSphereCount();
	unsigned int subsphereCount = PlumeManager::getInstance()->getSubsphereCount();
	this->updateProfilerData(freeSphereCount, subsphereCount);
}
