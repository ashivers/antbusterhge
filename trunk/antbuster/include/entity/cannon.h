#ifndef CANNON_H
#define CANNON_H

#include <hgevector.h>
#include <caAnimation.h>
#include "common/aimentity.h"

struct BulletData;
struct CannonData;

class BaseCannon : public AimEntity
{
public:
    enum CannonId
    {
        CI_NULL = -1,
        CI_Cannon = 0,
        CI_HeavyCannon1,
        CI_HeavyCannon2,
        CI_HeavyCannon3,
        NumCannonId,
    };
    BaseCannon(cAni::AnimResManager &arm, const CannonData *data);
    virtual void render(int time);
    virtual void step();
    virtual AimType getAimType() const
    {
        return AT_Cannon;
    }
    // 成功升级，则返回升级后的地址，否则返回0。如果成功则需要删除原BaseCannon
    BaseCannon *upgrade(size_t index) const;
    // 降级，同升级
    BaseCannon *degrade() const;
protected:
    friend struct CannonData;
    const CannonData *const data;

    cAni::Animation anim_base;
    cAni::Animation anim_tower;

    hgeVector targetPos;

    float lastFireTime;
};
struct CannonData
{
    BaseCannon::CannonId id;
    BaseCannon::CannonId parent; // -1 表示无祖先
    BaseCannon::CannonId evolution[3]; // 进化的下一级，显示调用的CannonInit内根据parent检验它的正确性。

    string name;
    float freq; // 攻击频率
    size_t bulletId;
    const BulletData &getBulletData() const;
    float range; // 射程

    string ad_base;  // 底座部分（固定部分）
    string ad_tower; // 炮塔部分（可旋转的部分）
    const cAni::AnimData *getAd_base(cAni::AnimResManager &arm) const; 
    const cAni::AnimData *getAd_tower(cAni::AnimResManager &arm) const;
    int price; // 建造费用

    BaseCannon *createInstance(cAni::AnimResManager &arm) const;
    void releaseInstance(BaseCannon *) const;
};

extern CannonData g_cannonData[BaseCannon::NumCannonId];
extern void CannonInit();
#endif