#include <cmath>
#include <cassert>
#include <cctype>

#include <hgefont.h>
#include <hgesprite.h>
#include <hgeguictrls.h>

#include "game/maingamestate.h"
#include "common/entity.h"
#include "entity/ant.h"
#include "entity/bullet.h"
#include "entity/cannon.h"

MainGameState *MainGameState::s_Instance = 0;

MainGameState::~MainGameState()
{
    assert(!cursor);
    assert(!gui);
    assert(s_Instance == this);
    assert(!animResManager && !system);
    assert(!hge);
    assert(ants.empty());
    assert(cannons.empty());
    s_Instance = 0;
}

void MainGameState::OnEnter()
{
    hge = hgeCreate(HGE_VERSION);
    assert(!animResManager && !system);
    animResManager = new cAni::AnimResManager;
    system = new hgeCurvedAniSystem;
    for (int i = 0; i < 10; i++)
        ants.push_back(new Ant(*animResManager));

    mouseLButtonDown = hge->Input_GetKeyState(HGEK_LBUTTON);

    gui = new hgeGUI;
    // gui->AddCtrl(new hgeGUIButton(1, 100, 520, 10, 10,));
    HTEXTURE tex = hge->Texture_Load("data/cursor.png");
    cursor = new hgeSprite(tex, 0, 0, 32, 32);
    gui->SetCursor(cursor);
    gui->Enter();

    bg = new cAni::Animation(2);
    bg->setAnimData(this->animResManager->getAnimData("data/bg0.xml"), 0); // 
    bg->setAnimData(this->animResManager->getAnimData("data/bg1.xml"), 1);
    bg->startAnim(int(60 * hge->Timer_GetTime()), 0);
}

void MainGameState::OnLeave()
{
    if (bg)
    {
        delete bg;
        bg = 0;
    }
    if (gui)
    {
        delete gui;
        gui = 0;
    }
    if (cursor)
    {
        HTEXTURE tex = cursor->GetTexture();
        delete cursor;
        hge->Texture_Free(tex);
        cursor = 0;
    }
    for (vector<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        delete *ant;
    }
    for (vector<BaseCannon *>::iterator cannon = cannons.begin(); cannon != cannons.end(); ++cannon)
    {
        delete *cannon;
    }
    for (list<Bullet *>::iterator bullet = bullets.begin(); bullet != bullets.end(); ++bullet)
    {
        delete *bullet;
    }
    ants.clear();
    cannons.clear();
    bullets.clear();
    if (animResManager)
    {
        delete animResManager;
        animResManager = 0;
    }
    if (system)
    {
        delete system;
        system = 0;
    }

    hge->Release();
    hge = 0;
}

void MainGameState::addCannon(BaseCannon::CannonId cannonid, float x, float y)
{
    assert(cannonid >= 0 && cannonid < BaseCannon::NumCannonId);
    BaseCannon *cannon = g_cannonData[cannonid].createInstance(*animResManager);
    cannon->setPos(hgeVector(x, y));
    cannons.push_back(cannon);
}

void MainGameState::OnFrame()
{
    if (hge->Input_GetKeyState(HGEK_ESCAPE))
    {
        // RequestState("mainmenu");
        RequestState("exit");
    }

    if (hge->Input_GetKeyState(HGEK_LBUTTON))
    {
        if (!mouseLButtonDown)
        {
            float x, y;
            hge->Input_GetMousePos(&x, &y);
            addCannon(BaseCannon::CI_Cannon, x, y);
        }
        mouseLButtonDown = true;
    }
    else
        mouseLButtonDown = false;

    gui->Update(hge->Timer_GetDelta());
    for (vector<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        (*ant)->step();
    }
    for (vector<BaseCannon *>::iterator cannon = cannons.begin(); cannon != cannons.end(); ++cannon)
    {
        (*cannon)->step();
    }
    for (list<Bullet *>::iterator bullet = bullets.begin(); bullet != bullets.end(); )
    {
        (*bullet)->step();
        if (!(*bullet)->isActive())
        {
            (*bullet)->data->releaseInstance(*bullet);
            bullet = bullets.erase(bullet);
        }
        else
            ++bullet;
    }
}

void MainGameState::OnRender()
{
    hge->Gfx_BeginScene(0);
    hge->Gfx_Clear(0xff22bb33);//黑色背景
    int time = int(60 * hge->Timer_GetTime());
    hge->Gfx_SetTransform(0, 0, 400, 300, 0, 1, 1);
    bg->render(time, 0);
    for (vector<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        (*ant)->render(time);
    }
    for (vector<BaseCannon *>::iterator cannon = cannons.begin(); cannon != cannons.end(); ++cannon)
    {
        (*cannon)->render(time);
    }
    for (list<Bullet *>::iterator bullet = bullets.begin(); bullet != bullets.end(); ++bullet)
    {
        (*bullet)->render(time);
    }
    hge->Gfx_SetTransform();
    gui->Render();

    // 游戏状态信息
    hge->Gfx_EndScene();
}

Ant *MainGameState::getTargetAnt(const hgeVector &pos)
{
    Ant *bestAnt = 0;
    float distance = 1e10;
    for (vector<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        float d = ((*ant)->getPos() - pos).Length();
        if (d < distance)
        {
            distance = d;
            bestAnt = *ant;
        }
    }
    return bestAnt;
}

void MainGameState::fire(const hgeVector &pos, Ant &targetAnt, const BulletData &bulletData)
{
    assert(this->animResManager);
    Bullet *bullet = bulletData.createInstance(*this->animResManager);
    bullet->setPos(pos);
    bullet->setTarget(targetAnt);
    this->bullets.push_back(bullet);
}
struct CmpNearestFrom
{
    hgeVector pos;
    CmpNearestFrom(const hgeVector &_pos) : pos(_pos)
    {
    }
    bool operator () (Ant *const a, Ant *const b) const
    {
        return (a->getPos() - pos).Length() < (b->getPos() - pos).Length();
    }
};
void MainGameState::getNearestAnts(vector<Ant *> &hitAnts, const hgeVector &pos, float maxRange)
{
    for (vector<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        float d = ((*ant)->getPos() - pos).Length();
        if (d < maxRange)
        {
            hitAnts.push_back(*ant);
        }
    }
    sort(ants.begin(), ants.end(), CmpNearestFrom(pos));
}