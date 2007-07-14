#ifndef BULLET_H
#define BULLET_H

#include "caAnimation.h"
#include "common/entity.h"

class Bullet : public Entity
{
public:
    Bullet();
    virtual void render(int time);
    virtual void step();

    cAni::Animation anim;
};

#endif//BULLET_H