#ifndef CANNON_H
#define CANNON_H

#include "caAnimation.h"

class Cannon
{
public:
    Cannon();
    virtual void render(int time);
    virtual void step();
protected:
    float freq; // 攻击频率
    float bulletSpeed; // 子弹速度
    float range; // 射程
    int damage; // 伤害

    cAni::Animation anim;
};

#endif