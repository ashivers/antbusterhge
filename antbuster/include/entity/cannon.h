#ifndef CANNON_H
#define CANNON_H

#include <hgevector.h>
#include <caAnimation.h>
#include "common/entity.h"

struct BulletData;
struct CannonData;

class BaseCannon : public Entity
{
public:
    enum CannonId
    {
        CI_Cannon,
        CI_HeavyCannon1,
        CI_HeavyCannon2,
        CI_HeavyCannon3,
        NumCannonId,
    };
    BaseCannon(cAni::AnimResManager &arm, const CannonData *data);
    virtual void render(int time);
    virtual void step();
    void setPos(const hgeVector &pos)
    {
        this->pos = pos;
    }
protected:
    friend struct CannonData;
    const CannonData *const data;

    cAni::Animation anim_base;
    cAni::Animation anim_tower;

    hgeVector pos;
    hgeVector targetPos;

    float lastFireTime;
};
struct CannonData
{
    size_t id;
    size_t parent; // size_t(-1) 表示无祖先

    string name;
    float freq; // 攻击频率
    size_t bulletId;
    const BulletData &getBulletData() const;
    float range; // 射程

    string ad_base;  // 底座部分（固定部分）
    string ad_tower; // 炮塔部分（可旋转的部分）
    const cAni::AnimData *getAd_base() const; 
    const cAni::AnimData *getAd_tower() const;
    int price; // 建造费用

    BaseCannon *createInstance(cAni::AnimResManager &arm) const;
    void releaseInstance(BaseCannon *) const;
};

extern CannonData g_cannonData[BaseCannon::NumCannonId];

#endif