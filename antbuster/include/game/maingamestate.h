#ifndef MAINGAMESTATE_H
#define MAINGAMESTATE_H

#include <caAnimation.h>
#include <hgeCurvedAni.h>
#include "common/gamestate.h"
class Ant;
class Cannon;
class hgeFont;
class hgeSprite;

class MainGameState : public GameState
{
public:
    MainGameState() : hge(0), animResManager(0), system(0)
    {
    }
    virtual ~MainGameState();

    virtual void OnEnter();
    virtual void OnLeave();
    virtual void OnFrame();
    virtual void OnRender();
protected:
    HGE *hge;

    vector<Ant *> ants;
    vector<Cannon *> cannons;
    cAni::AnimResManager *animResManager;
    hgeCurvedAniSystem *system;
};

#endif
