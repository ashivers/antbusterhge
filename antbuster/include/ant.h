#ifndef ANT_H
#define ANT_H

#include <hgevector.h>
#include "caAnimation.h"

class Ant
{
public:
    Ant();
    virtual void render(int time);
    virtual void step();
    int hp;
    int level;

    cAni::Animation anim;
    hgeVector pos;
    float angle;
};

#endif//ANT_H