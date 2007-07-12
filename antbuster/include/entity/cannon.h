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
    float freq; // ����Ƶ��
    float bulletSpeed; // �ӵ��ٶ�
    float range; // ���
    int damage; // �˺�

    cAni::Animation anim;
};

#endif