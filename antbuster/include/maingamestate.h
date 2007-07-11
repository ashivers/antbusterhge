#ifndef MAINGAMESTATE_H
#define MAINGAMESTATE_H

#include "gamestate.h"
class Ant;
class Cannon;
class hgeFont;
class hgeSprite;

class MainGameState : public GameState
{
public:
    MainGameState() : hge(0)
    {
    }
    virtual void OnEnter();
    virtual void OnLeave();
    virtual void OnFrame();
    virtual void OnRender();
protected:
    HGE *hge;

    vector<Ant *> ants;
    vector<Cannon *> cannons;
};

#endif
