#include "PlumeTracker.hpp"
#include "PlumeManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

PlumeTracker* PlumeTracker::sharedInstance = nullptr;

PlumeTracker::PlumeTracker()
{
    this->coneRadius = 0.0f;
}

PlumeTracker::~PlumeTracker()
{

}

PlumeTracker* PlumeTracker::getInstance()
{
    return sharedInstance;
}

void PlumeTracker::initialize()
{
    sharedInstance = new PlumeTracker();
}

void PlumeTracker::destroy()
{
    delete sharedInstance;
}

void PlumeTracker::loadData(std::string filePath)
{
    std::ifstream file;
    file.open(filePath);
    if (file.is_open())
    {
        for (std::string line; std::getline(file, line);)
        {
            std::istringstream ss(std::move(line));
            std::vector<std::string> cell;
            for (std::string value; std::getline(ss, value, ',');)
                cell.push_back(std::move(value));

            this->locationNames.push_back(cell[0]);
            this->arcStart.push_back(std::atof(cell[1].c_str()));
            this->arcEnd.push_back(std::atof(cell[2].c_str()));
        }
    }
    else std::cout << "ERROR: File with path " << filePath << " could not be opened.";

    file.close();
}

std::vector<std::string> PlumeTracker::getIntersectingLocations()
{
    std::vector<std::string> locNames;
    float lowAngle = windAngle - coneRadius;
    float hiAngle = windAngle + coneRadius;

    for (int i = 0; i < this->arcStart.size(); i++)
    {
        float arcStart = this->arcStart[i];
        float arcEnd = this->arcEnd[i];

        if (arcStart > arcEnd)
        {
            if (windAngle <= 180) arcStart -=360;
            else if (windAngle > 180) arcEnd += 360;
        }

        if ((arcStart <= lowAngle || arcStart <= hiAngle) &&
            (arcEnd >= lowAngle || arcEnd >= hiAngle))
            locNames.push_back(this->locationNames[i]);
    }

    return locNames;
}

void PlumeTracker::checkSmokePosition(unsigned int i)
{
    std::vector<smoke_layer> smokeLayers = PlumeManager::getInstance()->getPlumes()[0].smoke_layers;
    float maxAltStep = smokeLayers[0].center.z;
    altStep = altStep < minAltStep ? minAltStep : maxAltStep / stepSize;
    for (int j = 0; j < stepSize; j++)
    {
        if (int(smokeLayers[i].center.z) == int(j * altStep) + 1)
        {
            setPlumePositions(j, smokeLayers[i].center, smokeLayers[i].r);
        }
    }
}

void PlumeTracker::setPlumePositions(unsigned int index, vcl::vec3 position, float radius)
{
    if (index == this->positions.size())
    {
        this->positions.push_back(position);
        this->radii.push_back(radius);
        this->coneRadius = radii[radii.size() - 1];
    }
    else if (index < this->positions.size())
    {
        this->positions[index] = position;
        this->radii[index] = radius;
        this->coneRadius = radii[radii.size() - 1];
    }
}

std::vector<std::string>& PlumeTracker::getLocationNames()
{
    return this->locationNames;
}

std::vector<vcl::vec3>& PlumeTracker::getPositions()
{
    return this->positions;
}

std::vector<float>& PlumeTracker::getRadii()
{
    return this->radii;
}

float PlumeTracker::getConeRadius() const
{
    return this->coneRadius;
}

void PlumeTracker::resetPlumePositions()
{
    this->positions.clear();
    this->radii.clear();
    this->coneRadius = 0.0f;
}

vcl::vec3 PlumeTracker::getWindDirection() const
{
    return this->windVector;
}

float PlumeTracker::getWindDirectionAngle() const
{
    return this->windAngle;
}

void PlumeTracker::setWindDirection(vcl::vec3 windVector)
{
    this->windVector = PlumeManager::getInstance()->getAverageWindDirection();
    this->windAngle = vcl::vector_to_angle(windVector);
}
