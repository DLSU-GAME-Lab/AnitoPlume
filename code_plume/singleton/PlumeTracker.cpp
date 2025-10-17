#include "PlumeTracker.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

PlumeTracker* PlumeTracker::sharedInstance = nullptr;

PlumeTracker::PlumeTracker()
{
    
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

void PlumeTracker::addTrackerData()
{
    this->data.push_back(TrackerData());
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
    float coneRadius = getConeRadius();
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

void PlumeTracker::checkSmokePosition(unsigned int plumeID, float maxAltitude, vcl::vec3 center, float radius)
{
    altStep = altStep < minAltStep ? minAltStep : maxAltitude / stepSize;
    for (int i = 0; i < stepSize; i++)
    {
        if (int(center.z) == int(i * altStep) + 1) data[plumeID].setData(i, center, radius);
    }
}

unsigned int PlumeTracker::getDataCount()
{
    return data.size();
}

std::vector<std::string>& PlumeTracker::getLocationNames()
{
    return this->locationNames;
}

std::vector<vcl::vec3>& PlumeTracker::getPositions(unsigned int plumeID)
{
    return data[plumeID].positions;
}

std::vector<float>& PlumeTracker::getRadii(unsigned int plumeID)
{
    return data[plumeID].radii;
}

float PlumeTracker::getConeRadius() const
{
    float coneRadius = 0.0f;
    for (int i = 0; i < data.size(); i++) coneRadius += data[i].maxRadius;
    return coneRadius;
}

void PlumeTracker::resetPlumePositions()
{
    for (int i = 0; i < data.size(); i++) data[i].reset();
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
    this->windVector = windVector;
    this->windAngle = vcl::vector_to_angle(windVector);
}

void PlumeTracker::TrackerData::setData(unsigned int index, vcl::vec3 position, float radius)
{
    if (index == this->positions.size())
    {
        this->positions.push_back(position);
        this->radii.push_back(radius);
        this->maxRadius = radii[radii.size() - 1];
    }
    else if (index < this->positions.size())
    {
        this->positions[index] = position;
        this->radii[index] = radius;
        this->maxRadius = radii[radii.size() - 1];
    }
}

void PlumeTracker::TrackerData::reset()
{

}
