#include "PlumeManager.hpp"
PlumeManager* PlumeManager::sharedInstance = nullptr;

PlumeManager::PlumeManager()
{
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

