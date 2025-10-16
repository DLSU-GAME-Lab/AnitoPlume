#include "GUIScreen.hpp"

bool GUIScreen::getEnabled() const
{
    return this->enabled;
}

void GUIScreen::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

GUIScreen::GUIScreen(std::string name)
{
    this->name = name;
}

GUIScreen::~GUIScreen()
{

}

std::string GUIScreen::getName() const
{
    return this->name;
}
