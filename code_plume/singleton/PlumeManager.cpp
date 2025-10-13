#include "PlumeManager.hpp"
PlumeManager* PlumeManager::sharedInstance = nullptr;

PlumeManager::PlumeManager()
{
	all_angles = false;
	max_altitude = 10000;
	altitude_step = 2000;
	linear_wind_base = 15.f;
	altitude_size = int(max_altitude / altitude_step) + 1;
	for (unsigned int i = 0; i < altitude_size; i++)
	{
		wind_altitudes.push_back(i * altitude_step);
		winds.push_back(wind_structure(0, 0));
		this->deg_angle.push_back(0);
	}
}
void PlumeManager::initialize()
{
	if (sharedInstance == nullptr)
		sharedInstance = new PlumeManager();
}
void PlumeManager::destroy()
{
	delete sharedInstance;
}
PlumeManager* PlumeManager::getInstance()
{
	return sharedInstance;
}

void PlumeManager::createPlume(vcl::vec3 ventLoc)
{
	this->vecPlumes.push_back(Plume(ventLoc));
}
void PlumeManager::setupTransitionValues(int dMaxSmoke, float fTransitionSpeed, float fTransitionDelay)
{
	for (int i = 0; i < this->vecPlumes.size(); i++)
	{
		this->vecPlumes[i].setMaxSmoke(dMaxSmoke);
		this->vecPlumes[i].setTransitionSpeed(fTransitionSpeed);
		this->vecPlumes[i].setTranstionDelay(fTransitionDelay);
		for (int j = 0; j < dMaxSmoke; j++)
		{
			this->vecPlumes[i].getTransitionLifetime().push_back(this->vecPlumes[i].getTransitionDelay() * j);

		}

	}
}
void PlumeManager::removeSmokeLayers()
{
	for (int i = 0; i < this->vecPlumes.size(); i++)
	{
		this->vecPlumes[i].remove_smoke_layers();
	}
}
void PlumeManager::update(unsigned int dFrameCount)
{

	for (int i = 0; i < this->vecPlumes.size(); i++)
	{
		this->vecPlumes[i].update(dFrameCount);
	}
}
void PlumeManager::reset()
{
	for (int i = 0; i < this->vecPlumes.size(); i++)
	{
		this->vecPlumes[i].reset();
	}
}
std::vector<Plume>& PlumeManager::getPlumes()
{
	return this->vecPlumes;
}

void PlumeManager::setTStep(float fTStep)
{
	for (int i = 0; i < this->vecPlumes.size(); i++)
	{
		for (int j = 0; j < this->vecPlumes[i].getTransitionLifetime().size(); j++)
		{
			this->vecPlumes[i].getTransitionLifetime()[j] += fTStep;
		}
		this->vecPlumes[i].set_t_step(fTStep);

	}
}

vcl::vec3 PlumeManager::computeWindVector(float height)
{
	// find altitude interval
	unsigned int low_altitude_idx = 0;
	for (unsigned int i = 0; i < wind_altitudes.size(); i++)
	{
		if (wind_altitudes[i] < height)
		{
			low_altitude_idx = i;
		}
	}

	// compute wind vec by interpolating
	if (low_altitude_idx == wind_altitudes.size() - 1)
	{
		return winds[low_altitude_idx].wind_vector;
	}
	else
	{
		float low_height = (float)wind_altitudes[low_altitude_idx];
		float high_height = (float)wind_altitudes[low_altitude_idx + 1];
		float lambda = (height - low_height) / (high_height - low_height);
		vcl::vec3 interpo_wind = winds[low_altitude_idx].wind_vector + lambda * (winds[low_altitude_idx + 1].wind_vector - winds[low_altitude_idx].wind_vector);
		return interpo_wind;
	}
}

vcl::vec3 PlumeManager::getAverageWindDirection()
{
	vcl::vec3 winds_vec = { 0,0,0 };
	for (int i = 0; i < PlumeManager::getInstance()->getWinds().size(); i++)
	{
		winds_vec += PlumeManager::getInstance()->getWinds()[i].wind_vector;
	}

	float winds_squared_x = winds_vec.x * winds_vec.x;
	float winds_squared_y = winds_vec.y * winds_vec.y;
	float winds_squared_z = winds_vec.z * winds_vec.z;

	float mag = sqrt(winds_squared_x + winds_squared_y + winds_squared_z);
	vcl::vec3 avg_wind_direction = vcl::vec3(winds_vec.x, winds_vec.y, winds_vec.z) / mag;
	return avg_wind_direction;
}

std::vector<int>& PlumeManager::getWindAlts()
{
	return this->wind_altitudes;
}

std::vector<wind_structure>& PlumeManager::getWinds()
{
	return this->winds;
}

std::vector<int>& PlumeManager::getDegAngle()
{
	return this->deg_angle;
}

float PlumeManager::getMaxAlt()
{
	return this->max_altitude;
}

float PlumeManager::getAltStep()
{
	return this->altitude_step;
}

int PlumeManager::getAltSize()
{
	return this->altitude_size;
}

float PlumeManager::getLinearWindBase()
{
	return this->linear_wind_base;
}
void PlumeManager::setLinearWindBase(float fLinearWindBase)
{
	this->linear_wind_base = fLinearWindBase;
}

