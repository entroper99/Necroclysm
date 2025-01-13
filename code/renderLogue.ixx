﻿#include <SDL.h>

export module renderLog;

import std;
import util;
import constVar;
import drawText;
import globalVar;
import log;

export __int64 renderLog(SDL_Renderer* renderer)
{
	__int64 timeStampStart = getNanoTimer();//시간 측정 시작

	std::deque<std::wstring> logStrDeque = getLogStrDeque();
	std::deque<int> logTimerDeque = getLogTimerDeque();

	setFontSize(13);
	setSolidText();
	int textHeight = queryTextHeight(L"가", true);

	for (int i = 0; i < maxLogLine; i++)
	{
		if (logTimerDeque[i] > 0)
		{
			//prt(L"그림자 문자는 %ws 이다.\n", excludeControlChar(logStrDeque[i]).c_str());
			//prt(L"일반 문자는 %ws 이다.\n", logStrDeque[i].c_str());
			drawText(L"#000000" + eraseColorCodeText(logStrDeque[i]), 5 + 2, cameraH - 158 - textHeight * i + 2);
			drawText(logStrDeque[i], 5, cameraH - 158 - textHeight * i);
			if (stopLog == false) 
			{ 
				minusLogTimerDeque(i);
			}
		}
	}
	disableSolidText();

	return (getNanoTimer() - timeStampStart);
}