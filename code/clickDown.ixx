﻿export module clickDown;

import std;
import util;
import globalVar;
import GUI;
import wrapVar;

export void clickDown()
{
	click = true;
	dxClickStack = 0;
	dyClickStack = 0;
	dtClickStack = 0;
	dtClickStackStart = getMilliTimer();
	clickStartTime = getMilliTimer();
	deactHold = false;

	switch (option::inputMethod)
	{
		case input::mouse:
			clickDownPoint = { static_cast<int>(getMouseX()), static_cast<int>(getMouseY()) };
			break;
		case input::touch:
			clickDownPoint = { (int)(event.tfinger.x * cameraW), (int)(event.tfinger.y * cameraH) };
			break;
	}
	prevMouseX4Motion = getMouseX();
	prevMouseY4Motion = getMouseY();

	GUI::getActiveGUIList()[GUI::getActiveGUIList().size() - 1]->clickDownGUI();
}