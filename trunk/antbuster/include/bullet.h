#ifndef BULLET_H
#define BULLET_H

#include "caAnimation.h"

class Bullet
{
public:
    Bullet();
    virtual void render(int time);
    virtual void step();

    cAni::Animation anim;
};

#endif//BULLET_H