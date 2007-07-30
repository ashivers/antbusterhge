#ifndef MAINGAMESTATE_H
#define MAINGAMESTATE_H

#include <list>

#include <hgegui.h>
#include <caAnimation.h>
#include <hgeCurvedAni.h>
#include "common/gamestate.h"
#include "entity/cannon.h"

using namespace std;

class Ant;
class Bullet;
struct BulletData;
class hgeFont;
class hgeSprite;
class Map;

class MainGameState : public GameState
{
public:
    MainGameState() : hge(0), animResManager(0), system(0), picker(0), curPick(0)
    {
        assert(s_Instance == 0);
        s_Instance = this;
    }
    virtual ~MainGameState();

    virtual void OnEnter();
    virtual void OnLeave();
    virtual void OnFrame();
    virtual void OnRender();

    static MainGameState *GetInstance()
    {
        return s_Instance;
    }
    cAni::AnimResManager *GetAnimResManager()
    {
        return animResManager;
    }
    Ant *getTargetAnt(const hgeVector &pos);
    void fire(const hgeVector &pos, Ant &targetAnt, const BulletData &bulletData);
    void fire(const hgeVector &pos, const hgeVector &targetDirection, const BulletData &bulletData);
    void getNearestAnts(vector<Ant *> &hitAnts, const hgeVector &pos, float maxRange);
protected:
    void addCannon(BaseCannon::CannonId cannonid, float x, float y);
    AimEntity *findAimedEntity(float x, float y) const;

    void SetPick(AimEntity *newPick);
    void ShowCannonUi();
    void HideCannonUi();
    void ShowAntUi();
    void HideAntUi();

    HGE *hge;

    list<Ant *> ants;
    vector<BaseCannon *> cannons;
    list<Bullet *> bullets;
    cAni::AnimResManager *animResManager;
    hgeCurvedAniSystem *system;
    HTEXTURE texGui;

    int points;
    int money;
    int curAntLevel;
    float lastSpawnAntTime;

    bool bAddNewCannon;

    Map *map;

    bool mouseLButtonDown;
    cAni::Animation *cake;
    hgeSprite *cursor, *cursorWithCannon;
    hgeGUI *gui;
    hgeFont *font;
    static MainGameState *s_Instance;

    cAni::Animation *picker;
    hgeVector pickerCurPos;

    AimEntity *aimEntityHead;
    AimEntity *curPick;
};

#endif
