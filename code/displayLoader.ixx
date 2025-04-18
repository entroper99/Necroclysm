﻿#include <SDL.h>

export module displayLoader;

export import globalVar;

import util;
import constVar;

/*
 실행시킨 디바이스의 해상도에 따라 게임의 해상도를 조정
 기본 개발은 720*720 정사각 해상도에서 진행됩니다.
기기가 가로가 더 길 경우 세로가 720으로 고정되고 가로로 길어집니다.
기기가 세로로 더 길 경우 가로가 720으로 고정되고 세로로 길어집니다.
*/
export void displayLoader()
{
    bool debugSquare = true;

    cameraW = 720;
    cameraH = 720;

    //창을 최소화하면 커스텀스프라이트들이 사라지는 버그가 이 라인들이 있으면 안 일어남.. 왜일까?
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    //////////////////////////////////////////////////////////////////////////////////////////////


    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0) errorBox(L"0번 디스플레이가 존재하지 않는다.");
    else prt(L"0번 디스플레이 (%d Hz) : %d×%d\n", displayMode.refresh_rate, displayMode.w, displayMode.h);

    cameraW = displayMode.w;
    cameraH = displayMode.h;

    window = SDL_CreateWindow("Necroclysm", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, cameraW, cameraH, 0);
    if(option::fullScreen) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    renderer = SDL_CreateRenderer(window, -1, 0);
    setPrimitiveRenderer(renderer);


    if (cameraW > cameraH)
    {
        cameraW = ((float)cameraW / (float)cameraH) * (float)720;
        cameraH = 720;
    }
    else if (cameraH > cameraW)
    {
        cameraH = ((float)cameraH / (float)cameraW) * (float)720;
        cameraW = 720;
    }

    ///////////PC 테스트용 강제 해상도 조정////////
    if(option::fixScreenRatio)
    {
        int resolution = 0;
        switch (resolution)
        {
        default://1:1
            cameraW = 720;
            cameraH = 720;
            break;
        case 1://9:16
            cameraW = 720;
            cameraH = 1280;
            break;
        case 2://4:3
            cameraW = 960;
            cameraH = 720;
            break;
        case 3://1:2
            cameraW = 720;
            cameraH = 1440;
            break;
        case 4://16:9
            cameraW = 1280;
            cameraH = 720;
            break;
        }
    }


    SDL_SetWindowSize(window, cameraW, cameraH);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_RenderSetLogicalSize(renderer, cameraW, cameraH);

    // 이 아래로 HUD 관련 변수
    letterbox = { 0, 0, 630, 140 };
    letterbox.x = (cameraW - letterbox.w) / 2;
    letterbox.y = cameraH - letterbox.h + 6;
    for (int i = 0; i < 7; i++)
    {
        barButton[i] = { cameraW / 2 - 300 + (88 * i), cameraH - 80, 72,72 };
    }
    
    letterboxPopUpButton = { letterbox.x + letterbox.w - 42 + 3, letterbox.y - 36 + 3,29,29 };
}