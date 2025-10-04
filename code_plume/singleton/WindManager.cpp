#include "WindManager.hpp"

using namespace vcl;

WindManager* WindManager::sharedInstance = nullptr;

WindManager::WindManager()
{

}

WindManager::~WindManager()
{

}

WindManager* WindManager::getInstance()
{
	return sharedInstance;
}

void WindManager::initialize()
{
	sharedInstance = new WindManager();
}

void WindManager::destroy()
{
	delete sharedInstance;
}

vcl::vec3 WindManager::compute_wind_vector(float height)
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
        vec3 interpo_wind = winds[low_altitude_idx].wind_vector + lambda * (winds[low_altitude_idx + 1].wind_vector - winds[low_altitude_idx].wind_vector);
        return interpo_wind;
    }
}
