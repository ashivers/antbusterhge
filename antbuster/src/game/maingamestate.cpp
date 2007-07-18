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

#ifndef SAFE_DELETE
#define SAFE_DELETE(a) if (!a);else {delete a; a = 0;}
#endif
enum GUI_ID
{
    GID_BtnAddCannon = 1,
    GID_BtnMute,
    GID_BtnPause,
    NumGUIId,
};
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

    curAntLevel = 1;
    points = 0;
    money = 200;

    lastSpawnAntTime = hge->Timer_GetTime();

    mouseLButtonDown = hge->Input_GetKeyState(HGEK_LBUTTON);
    bAddNewCannon = false;

    font = new hgeFont("data/font.fnt");
    font->SetColor(ARGB(255, 0, 255, 0));
    
    gui = new hgeGUI;
    // gui->AddCtrl(new hgeGUIButton(1, 100, 520, 10, 10,));
    this->texGui = hge->Texture_Load("data/ui.png");
    cursor = new hgeSprite(this->texGui, 0, 120, 32, 32);
    cursorWithCannon = new hgeSprite(this->texGui, 32, 120, 32, 32);
    cursorWithCannon->SetHotSpot(8, 7);
    gui->AddCtrl(new hgeGUIButton(GID_BtnAddCannon, 170, 465, 50, 51, this->texGui, 0, 0));
    gui->AddCtrl(new hgeGUIButton(GID_BtnMute, 544, 503, 27, 28, this->texGui, 0, 52));
    gui->AddCtrl(new hgeGUIButton(GID_BtnPause, 514, 503, 27, 28, this->texGui, 0, 85));
    ((hgeGUIButton*)gui->GetCtrl(GID_BtnMute))->SetMode(true);
    ((hgeGUIButton*)gui->GetCtrl(GID_BtnPause))->SetMode(true);

    gui->SetCursor(cursor);
    gui->Enter();

    bg = new cAni::Animation(2);
    bg->setAnimData(this->animResManager->getAnimData("data/bg0.xml"), 0); // 
    bg->setAnimData(this->animResManager->getAnimData("data/bg1.xml"), 1);
    bg->startAnim(int(60 * hge->Timer_GetTime()), 0);

    cake = new cAni::Animation;
    cake->setAnimData(this->animResManager->getAnimData("data/cake.xml"), 0);
}

void MainGameState::OnLeave()
{
    SAFE_DELETE(bg);
    SAFE_DELETE(cake);
    SAFE_DELETE(gui);
    SAFE_DELETE(cursor);
    SAFE_DELETE(cursorWithCannon);
    SAFE_DELETE(font);
    if (this->texGui)
    {
        hge->Texture_Free(this->texGui);
        this->texGui = 0;
    }
    for (list<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
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
    SAFE_DELETE(animResManager);
    SAFE_DELETE(system);

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
        if (!mouseLButtonDown && bAddNewCannon)
        {
            float x, y;
            hge->Input_GetMousePos(&x, &y);
            const cAni::Rect border(150 + 16, 650 - 16, 50 + 16, 448 - 16);
            if (x > border.left && x < border.right && y > border.top && y < border.bottom)
                addCannon(BaseCannon::CI_Cannon, x, y);
            bAddNewCannon = false;
            gui->SetCursor(cursor);
        }
        mouseLButtonDown = true;
    }
    else
        mouseLButtonDown = false;

    switch(gui->Update(hge->Timer_GetDelta()))
    {
    case GID_BtnAddCannon:
        bAddNewCannon = true;
        gui->SetCursor(cursorWithCannon);
        break;
    case GID_BtnPause:
        break;
    case GID_BtnMute:
        break;
    }
    for (list<Ant *>::iterator ant = ants.begin(); ant != ants.end(); )
    {
        (*ant)->step();
        if (!(*ant)->isActive())
        {
			points += (*ant)->getLevel();
			money += (*ant)->getLevel();

            delete *ant;
            ant = ants.erase(ant);
        }
        else
            ++ant;
    }
    if (ants.size() < 6)
    {
        if (hge->Timer_GetTime() - lastSpawnAntTime > 1)
        {
            lastSpawnAntTime = hge->Timer_GetTime();
            Ant *ant = new Ant(*animResManager);
            ant->initAnt(hgeVector(184, 83), this->curAntLevel++);
            ants.push_back(ant);
        }
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
    hge->Gfx_Clear(ARGB(255, 0x22, 0xbb, 0x33));//黑色背景
    int time = int(60 * hge->Timer_GetTime());
    hge->Gfx_SetTransform(0, 0, 400, 300, 0, 1, 1);
    bg->render(time, 0);
    hge->Gfx_SetTransform(0, 0, 620, 417, 0, 1, 1);
    cake->render(time / 10, 0);
    for (list<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
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
    
    font->printf(620 - 78, 480, HGETEXT_CENTER, "%d", this->points);
    font->printf(610, 480, HGETEXT_CENTER, "%d", this->money);
    font->printf(640 - 14, 505, HGETEXT_CENTER, "%d", this->curAntLevel);

    // 游戏状态信息
    hge->Gfx_EndScene();
}

Ant *MainGameState::getTargetAnt(const hgeVector &pos)
{
    Ant *bestAnt = 0;
    float distance = 1e10;
    for (list<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
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

void MainGameState::fire(const hgeVector &pos, const hgeVector &targetDirection, const BulletData &bulletData)
{
    assert(this->animResManager);
    Bullet *bullet = bulletData.createInstance(*this->animResManager);
    bullet->setPos(pos);
    bullet->setTarget(pos + targetDirection);
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
    for (list<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        float d = ((*ant)->getPos() - pos).Length();
        if (d < maxRange)
        {
            hitAnts.push_back(*ant);
        }
    }
    sort(hitAnts.begin(), hitAnts.end(), CmpNearestFrom(pos));
}