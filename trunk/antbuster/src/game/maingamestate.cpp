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
#include "entity/map.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(a) if (!a);else {delete a; a = 0;}
#endif

enum GUI_ID
{
    /// GUI main
    GID_BtnAddCannon = 1,
    GID_BtnMute,
    GID_BtnPause,
    GID_TxtPoints,
    GID_TxtPointsValue,
    GID_TxtMoney,
    GID_TxtMoneyValue,
    GID_TxtLevel,

    /// GUI Cannon
    GID_BtnCannonUpgradeA,
    GID_BtnCannonUpgradeB,
    GID_BtnCannonUpgradeC,
    GID_BtnCannonDegrade,
    GID_BtnCannonCancel,
    GID_TxtCannonName,
    // cannon info ...

    /// GUI Ant
    GID_BtnAntCancel,
    GID_TxtAntLevel,
    // ant info ...

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
    CannonInit();
    animResManager = new cAni::AnimResManager;
    system = new hgeCurvedAniSystem;

    curAntLevel = 1;
    points = 0;
    money = 200;

    lastSpawnAntTime = hge->Timer_GetTime();

    mouseLButtonDown = hge->Input_GetKeyState(HGEK_LBUTTON);
    bAddNewCannon = false;

    aimEntityHead = new AimEntity(*animResManager);

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

    // add cannon ui
    gui->AddCtrl(new hgeGUIButton(GID_BtnCannonUpgradeA, 236, 481, 40, 40, texGui, 0, 177));
    gui->AddCtrl(new hgeGUIButton(GID_BtnCannonUpgradeB, 236 + 40, 481, 40, 40, texGui, 0, 177));
    gui->AddCtrl(new hgeGUIButton(GID_BtnCannonUpgradeC, 236 + 80, 481, 40, 40, texGui, 0, 177));
    gui->AddCtrl(new hgeGUIButton(GID_BtnCannonDegrade, 236 + 120, 481, 40, 40, texGui, 0, 177));
    gui->AddCtrl(new hgeGUIButton(GID_BtnCannonCancel, 236 + 243 , 481 + 24, 32, 32, texGui, 0, 177));
    this->HideCannonUi();

    // add ant ui
    gui->AddCtrl(new hgeGUIButton(GID_BtnAntCancel, 236 + 243 , 481 + 24, 32, 32, texGui, 0, 177));
    this->HideAntUi();


    gui->SetCursor(cursor);
    gui->Enter();


    cake = new cAni::Animation;
    cake->setAnimData(this->animResManager->getAnimData("data/cake.xml"), 0);

    picker = new cAni::Animation(2);
    picker->setAnimData(this->animResManager->getAnimData("data/pickant.xml"), 0);
    picker->setAnimData(this->animResManager->getAnimData("data/pickcannon.xml"), 1);
    
    range = new cAni::Animation;
    range->setAnimData(this->animResManager->getAnimData("data/range.xml"), 0);

    upgradeButton[0] = new cAni::Animation;
    upgradeButton[1] = new cAni::Animation;
    upgradeButton[2] = new cAni::Animation;

    this->map = new Map(*this->animResManager);
}

void MainGameState::OnLeave()
{
    SAFE_DELETE(map);
    SAFE_DELETE(cake);
    SAFE_DELETE(picker);
    SAFE_DELETE(range);
    SAFE_DELETE(upgradeButton[0]);
    SAFE_DELETE(upgradeButton[1]);
    SAFE_DELETE(upgradeButton[2]);
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
    assert(!aimEntityHead->getNext());
    SAFE_DELETE(aimEntityHead);
    hge->Release();
    hge = 0;
}

void MainGameState::addCannon(BaseCannon::CannonId cannonid, float x, float y)
{
    assert(cannonid >= 0 && cannonid < BaseCannon::NumCannonId);
    BaseCannon *cannon = g_cannonData[cannonid].createInstance(*animResManager);
    cannon->setPos(hgeVector(x, y));
    map->setOccupied(x, y);
    cannons.push_back(cannon);
    assert(aimEntityHead);
    aimEntityHead->insertAfter(*cannon);
}

AimEntity *MainGameState::findAimedEntity(float x, float y) const
{
    hgeVector ap(x, y);
    assert(aimEntityHead);
    float minLen = 10;
    AimEntity *minAe = 0;
    for (AimEntity *ae = aimEntityHead->getNext(); ae; ae = ae->getNext())
    {
        float len = (ae->getPos() - ap).Length();
        if (len < minLen)
        {
            minLen = len;
            minAe = ae;
        }
    }
    return minAe;
}

void MainGameState::SetPick(AimEntity *newPick)
{
    if (this->curPick)
    {
        switch(this->curPick->getAimType())
        {
        case AimEntity::AT_Ant:
            this->HideAntUi();
            break;
        case AimEntity::AT_Cannon:
            this->HideCannonUi();
            break;
        }
    }
    this->curPick = newPick;
    if (newPick == 0)
        return;
    switch(this->curPick->getAimType())
    {
    case AimEntity::AT_Ant:
        this->picker->startAnim(-1, 0);
        this->ShowAntUi();
        break;
    case AimEntity::AT_Cannon:
        this->picker->startAnim(-1, 1);
        this->ShowCannonUi();
        break;
    default:
        assert(0);
    }
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
            if (bAddNewCannon)
            {
                if (map->checkCannonPos(x, y))
                    addCannon(BaseCannon::CI_Cannon, x, y);

                bAddNewCannon = false;
                gui->SetCursor(cursor);
                map->setShowHighLightClip(false);
            }
            else
            {
                AimEntity *newPick = this->findAimedEntity(x, y);
                if (newPick)
                {
                    SetPick(newPick);
                }
            }
        }
        mouseLButtonDown = true;
    }
    else
        mouseLButtonDown = false;

    if (curPick)
    {
        pickerCurPos += (curPick->getPos() - pickerCurPos) * 0.1f;
    }

    int id = gui->Update(hge->Timer_GetDelta());
    switch(id)
    {
    case GID_BtnAddCannon:
        bAddNewCannon = true;
        gui->SetCursor(cursorWithCannon);
        map->setShowHighLightClip(true);
        break;
    case GID_BtnPause:
        break;
    case GID_BtnMute:
        break;
    case GID_BtnCannonUpgradeA:
    case GID_BtnCannonUpgradeB:
    case GID_BtnCannonUpgradeC:
        {
            assert(curPick);
            assert(curPick->getAimType() == AimEntity::AT_Cannon);
            BaseCannon *cannon = (BaseCannon *)curPick;
            BaseCannon *newCannon = cannon->upgrade(id - GID_BtnCannonUpgradeA);
            if (newCannon)
            {
                replace(this->cannons.begin(), this->cannons.end(), cannon, newCannon);
                SetPick(newCannon);
                newCannon->setPos(cannon->getPos());
                assert(aimEntityHead);
                aimEntityHead->insertAfter(*newCannon);
                delete cannon;
            }
        }
        break;
    case GID_BtnCannonDegrade:
        {
            assert(curPick);
            assert(curPick->getAimType() == AimEntity::AT_Cannon);
            BaseCannon *cannon = (BaseCannon *)curPick;
            BaseCannon *newCannon = cannon->degrade();
            if (newCannon)
            {
                replace(this->cannons.begin(), this->cannons.end(), cannon, newCannon);
                SetPick(newCannon);
                newCannon->setPos(cannon->getPos());
                assert(aimEntityHead);
                aimEntityHead->insertAfter(*newCannon);
            }
            else
            {
                this->cannons.erase(remove(this->cannons.begin(), this->cannons.end(), cannon), this->cannons.end());
                SetPick(0);
                map->clearOccupied(cannon->getPos().x, cannon->getPos().y);
            }
            delete cannon;
        }
        break;

    case GID_BtnAntCancel:
    case GID_BtnCannonCancel:
        SetPick(0);
        break;
    }
    for (list<Ant *>::iterator ant = ants.begin(); ant != ants.end();)
    {
        (*ant)->step();
        if (!(*ant)->isActive())
        {
            points += (*ant)->getLevel();
            money += (*ant)->getLevel();

            if (this->curPick == *ant)
            {
                SetPick(0);
            }
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
            assert(aimEntityHead);
            aimEntityHead->insertAfter(*ant);
        }
    }
    for (vector<BaseCannon *>::iterator cannon = cannons.begin(); cannon != cannons.end(); ++cannon)
    {
        (*cannon)->step();
    }
    for (list<Bullet *>::iterator bullet = bullets.begin(); bullet != bullets.end();)
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

void MainGameState::ShowCannonUi()
{
    assert(gui);
    assert(curPick && curPick->getAimType() == AimEntity::AT_Cannon);
    BaseCannon *cannon = (BaseCannon *) curPick;
    const CannonData &data = cannon->getData();

    for (int i = 0; i < 3; i++)
    {
        BaseCannon::CannonId cid = data.evolution[i];
        if (cid != BaseCannon::CI_NULL)
        {
            assert(cid < BaseCannon::NumCannonId);
            assert(upgradeButton[i]);
            upgradeButton[i]->setAnimData(g_cannonData[cid].getAd_button(*this->animResManager), 0);
            gui->ShowCtrl(GID_BtnCannonUpgradeA + i, true);
            gui->EnableCtrl(GID_BtnCannonUpgradeA + i, true);
        }
    }
    gui->ShowCtrl(GID_BtnCannonDegrade, true);
    gui->EnableCtrl(GID_BtnCannonDegrade, true);

    gui->ShowCtrl(GID_BtnCannonCancel, true);
    gui->EnableCtrl(GID_BtnCannonCancel, true);
}

void MainGameState::HideCannonUi()
{
    assert(gui);

    gui->ShowCtrl(GID_BtnCannonUpgradeA, false);
    gui->ShowCtrl(GID_BtnCannonUpgradeB, false);
    gui->ShowCtrl(GID_BtnCannonUpgradeC, false);
    gui->ShowCtrl(GID_BtnCannonDegrade, false);
    gui->ShowCtrl(GID_BtnCannonCancel, false);
    gui->EnableCtrl(GID_BtnCannonUpgradeA, false);
    gui->EnableCtrl(GID_BtnCannonUpgradeB, false);
    gui->EnableCtrl(GID_BtnCannonUpgradeC, false);
    gui->EnableCtrl(GID_BtnCannonDegrade, false);
    gui->EnableCtrl(GID_BtnCannonCancel, false);
}

void MainGameState::ShowAntUi()
{
    assert(gui);
    assert(curPick && curPick->getAimType() == AimEntity::AT_Ant);

    gui->ShowCtrl(GID_BtnAntCancel, true);
    gui->EnableCtrl(GID_BtnAntCancel, true);
}

void MainGameState::HideAntUi()
{
    assert(gui);

    gui->ShowCtrl(GID_BtnAntCancel, false);
    gui->EnableCtrl(GID_BtnAntCancel, false);
}

void MainGameState::OnRender()
{
    hge->Gfx_BeginScene(0);
    hge->Gfx_Clear(ARGB(255, 0x22, 0xbb, 0x33));//黑色背景
    int time = int(60 * hge->Timer_GetTime());
    map->render(time);
    hge->Gfx_SetTransform(0, 0, 620, 417, 0, 1, 1);
    cake->render(time / 10, 0);

    // draw range before ants and cannons
    if (this->curPick && this->curPick->getAimType() == AimEntity::AT_Cannon)
    {
        BaseCannon *cannon = (BaseCannon *) this->curPick;
        hge->Gfx_SetTransform(0, 0, cannon->getPos().x, cannon->getPos().y, 0, 1, 1);
        this->range->render(cannon->getRange(), 0);
    }
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
    if (this->curPick)
    {
        hge->Gfx_SetTransform(0, 0, pickerCurPos.x, pickerCurPos.y, 0, 1, 1);
        this->picker->render(time, 0);
    }
    hge->Gfx_SetTransform();
    gui->Render();

    for (int i = 0; i < 3; i++)
    {
        hgeGUIButton *btn = (hgeGUIButton *)gui->GetCtrl(GID_BtnCannonUpgradeA + i);
        if (btn->bVisible)
        {
            hge->Gfx_SetTransform(0, 0, (btn->rect.x1 + btn->rect.x2) * 0.5f, (btn->rect.y1 + btn->rect.y2) * 0.5f, 0, 1, 1);
            this->upgradeButton[i]->render(time, 0);
        }
    }
    
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
