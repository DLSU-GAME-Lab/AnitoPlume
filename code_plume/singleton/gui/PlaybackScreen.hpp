#pragma once
#include "GUIScreen.hpp"

class PlaybackScreen : public GUIScreen
{
private:
	float timer_scale;
	unsigned int pauseIcon;
	unsigned int playIcon;
	unsigned int stopIcon;

public:
	float getTimerScale() const;
	void setTimerScale(float timer_scale);

protected:
	PlaybackScreen();
	~PlaybackScreen();

	virtual void drawGUI() override;

	friend class GUIManager;
};