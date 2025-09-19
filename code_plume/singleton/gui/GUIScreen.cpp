#include "GUIScreen.hpp"

GUIScreen::GUIScreen(std::string name)
{
    this->name = name;
}

GUIScreen::~GUIScreen()
{

}

std::string GUIScreen::getName()
{
    return this->name;
}
