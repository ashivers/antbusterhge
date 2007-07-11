#include <cmath>
#include <cassert>

#include "maingamestate.h"
#include "hgefont.h"
#include "hgesprite.h"

#include "ant.h"
#include "bullet.h"
#include "cannon.h"

hgeFont *fnt;

void MainGameState::OnEnter()
{
    hge = hgeCreate(HGE_VERSION);
}

void MainGameState::OnLeave()
{
    hge->Release();
    hge = 0;
}

void MainGameState::OnFrame()
{
    if (hge->Input_GetKeyState(HGEK_ESCAPE))
    {
        RequestState("mainmenu");
    }

    for (vector<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        (*ant)->step();
    }
    for (vector<Cannon *>::iterator cannon = cannons.begin(); cannon != cannons.end(); ++cannon)
    {
        (*cannon)->step();
    }
}

void MainGameState::OnRender()
{
    hge->Gfx_BeginScene(0);
    hge->Gfx_Clear(0xff000000);//黑色背景
    int time = int(60 * hge->Timer_GetTime());
    for (vector<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        (*ant)->render(time);
    }
    for (vector<Cannon *>::iterator cannon = cannons.begin(); cannon != cannons.end(); ++cannon)
    {
        (*cannon)->render(time);
    }

    // 游戏状态信息
    hge->Gfx_EndScene();
}
