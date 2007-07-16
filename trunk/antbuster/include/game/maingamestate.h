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

class MainGameState : public GameState
{
public:
    MainGameState() : hge(0), animResManager(0), system(0)
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

    HGE *hge;

    list<Ant *> ants;
    vector<BaseCannon *> cannons;
    list<Bullet *> bullets;
    cAni::AnimResManager *animResManager;
    hgeCurvedAniSystem *system;

    int curAntLevel;
    float lastSpawnAntTime;

    bool mouseLButtonDown;
    cAni::Animation *bg;
    cAni::Animation *cake;
    hgeSprite *cursor;
    hgeGUI *gui;
    static MainGameState *s_Instance;
};

#endif
