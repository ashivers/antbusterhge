#ifndef BULLET_H
#define BULLET_H

#include <hgeVector.h>
#include <caAnimation.h>
#include "common/entity.h"

struct BulletData;
class Ant;
class Bullet : public Entity
{
public:
    enum BulletId
    {
        BI_CannonA, // NOTE they should have some more significative names
        BI_CannonB,
        BI_CannonC,
        BI_CannonD,

        NumBulletId,
    };
    Bullet(cAni::AnimResManager &arm, const BulletData *data);
    virtual void render(int time);
    virtual void step();

    void setPos(const hgeVector &pos)
    {
        this->pos = pos;
    }
    void setTarget(const hgeVector &target)
    {
        this->target = target;
    }
    virtual void setTarget(Ant &ant);
    cAni::Animation anim;

    const BulletData * const data;
    hgeVector pos;
    hgeVector target;
};

class Missile : public Bullet
{
public:
    Missile(cAni::AnimResManager &arm, const BulletData *data) : Bullet(arm, data), targetAnt(0)
    {
    }
    virtual void step();
    virtual void setTarget(Ant &ant);

    Ant *targetAnt; //target ant
};

struct BulletData
{
    Bullet::BulletId bulletId;
    string name;
    float bulletSpeed; // �ӵ��ٶ�
    int damage; // �˺�
    string ad_fire;  // ����
    string ad_move;  // ����
    string ad_explode; // ��ը
    const cAni::AnimData *getAd_fire() const;  // ����
    const cAni::AnimData *getAd_move() const;  // ����
    const cAni::AnimData *getAd_explode() const; // ��ը
    Bullet *createInstance(cAni::AnimResManager &arm) const; // ����һ���µı������ӵ�
    void releaseInstance(Bullet *bullet) const; // �ͷŸ��ӵ�
    void applyDamage(Ant &ant) const;
};

extern BulletData g_bulletData[Bullet::NumBulletId];

#endif//BULLET_H