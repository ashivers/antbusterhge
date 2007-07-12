#include <cmath>
#include <cassert>

#include "hgefont.h"
#include "hgesprite.h"

#include "game/maingamestate.h"

#include "entity/ant.h"
#include "entity/bullet.h"
#include "entity/cannon.h"

hgeFont *fnt;
MainGameState::~MainGameState()
{
    assert(!hge);
    assert(ants.empty());
    assert(cannons.empty());
}
void MainGameState::OnEnter()
{
    hge = hgeCreate(HGE_VERSION);

    ants.push_back(new Ant);
}

void MainGameState::OnLeave()
{
    for (vector<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        delete *ant;
    }
    for (vector<Cannon *>::iterator cannon = cannons.begin(); cannon != cannons.end(); ++cannon)
    {
        delete *cannon;
    }
    ants.clear();
    cannons.clear();

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
