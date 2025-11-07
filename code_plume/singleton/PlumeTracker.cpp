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
    if (windAngle < 0.0f) return locNames;

    float coneRadius = getConeRadius();
    float lowAngle = windAngle - coneRadius;
    float hiAngle = windAngle + coneRadius;

    for (int i = 0; i < this->arcStart.size(); i++)
    {
        float locStart = this->arcStart[i];
        float locEnd = this->arcEnd[i];

        if (locStart > locEnd)
        {
            if (windAngle <= 180) locStart -= 360.0f;
            else if (windAngle > 180) locEnd += 360.0f;
            std::cout << "loc arc1: " << locStart << ", " << locEnd << "\n";
        }

        std::cout << "loc arc1: " << locStart << ", " << locEnd << "\n";

        if ((locStart <= lowAngle || locStart <= hiAngle) &&
            (locEnd >= lowAngle || locEnd >= hiAngle))
        {
            locNames.push_back(this->locationNames[i]);
        }
    }

    return locNames;
}

void PlumeTracker::checkSmokePosition(Plume* plume, unsigned int smokeIndex)
{
    for (int i = 0; i < stepSize; i++)
    {
        int step = i * altStep;
        int alt = plume->smoke_layers[smokeIndex].center.z;
        if (alt >= step - 10 && alt <= step + 10)
            data[plume->getID()].setData(i, plume->smoke_layers[smokeIndex].center, plume->smoke_layers[smokeIndex].r);
    }
    //std::cout << altStep << std::endl;
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
    if (windVector.x == 0 && windVector.y == 0 && windVector.z == 0)
        this->windAngle = -1.0f;
    else this->windAngle = vcl::vector_to_angle(windVector);
}

void PlumeTracker::TrackerData::setData(unsigned int index, vcl::vec3 position, float radius)
{
    //std::cout << "setting data for: " << index << " x: " << position.x << " y: " << position.y << " z: " << position.z << " r: " << radius << std::endl;
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
    this->positions.clear();
    this->radii.clear();
    this->maxRadius = 0.0f;
}
