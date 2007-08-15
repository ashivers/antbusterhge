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
        CI_HeavyCannon1,        CI_HeavyCannon2, CI_HeavyCannon3,
        CI_MissileLauncher1,    CI_MissileLauncher2,
        CI_ImpactCannon1,       CI_ImpactCannon2,
        CI_IceCannon1,          CI_IceCannon2,
        CI_DoubleHeavyCannon1,  CI_DoubleHeavyCannon2,
        CI_SonicPulse1,         CI_SonicPulse2,
        CI_QuickCannon1,        CI_QuickCannon2, CI_QuickCannon3,
        CI_FlameThrower1,
        CI_SniperCanon1,        CI_SniperCanon2,
        CI_Laser1,              CI_Laser2,
        CI_LongRangeCannon1,    CI_LongRangeCannon2,
        CI_EletricCannon1,      CI_EletricCannon2,
        CI_DoubleCannon1,       CI_DoubleCannon2, CI_DoubleCannon3,
        CI_Boomerang,
        CI_DoubleQuickCannon1,  CI_DoubleQuickCannon2,
        CI_MachineGun,
        CI_TripleCannon1,       CI_TripleCannon2,
        CI_PoisonSpray1,        CI_PoisonSpray2,

        NumCannonId,
    };
    BaseCannon(cAni::AnimResManager &arm, const CannonData *data);
    virtual void render(int time);
    virtual void step();
    virtual AimType getAimType() const
    {
        return AT_Cannon;
    }
    // �ɹ��������򷵻�������ĵ�ַ�����򷵻�0������ɹ�����Ҫɾ��ԭBaseCannon
    BaseCannon *upgrade(size_t index) const;
    // ������ͬ����
    BaseCannon *degrade() const;

    inline float getRange() const;
    const CannonData &getData() const
    {
        return *data;
    }
protected:
    virtual void fire(const hgeVector &targetPos);
    friend struct CannonData;
    const CannonData *const data;

    cAni::Animation anim_base;
    cAni::Animation anim_tower;

    hgeVector targetPos;

    float lastFireTime;
};
class DoubleCannon : public BaseCannon
{
public:
    DoubleCannon(cAni::AnimResManager &arm, const CannonData *data) : BaseCannon(arm, data)
    {
    }
protected:
    virtual void fire(const hgeVector &targetPos);
};
class TripleCannon : public BaseCannon
{
public:
    TripleCannon(cAni::AnimResManager &arm, const CannonData *data) : BaseCannon(arm, data)
    {
    }
protected:
    virtual void fire(const hgeVector &targetPos);
};
struct CannonData
{
    BaseCannon::CannonId id;
    BaseCannon::CannonId parent; // -1 ��ʾ������
    BaseCannon::CannonId evolution[3]; // ��������һ������ʾ���õ�CannonInit�ڸ���parent����������ȷ�ԡ�

    string name;
    float freq; // ����Ƶ��
    size_t bulletId;
    const BulletData &getBulletData() const;
    float range; // ���
    int price; // �������

    string ad_base;  // �������֣��̶����֣�
    string ad_tower; // �������֣�����ת�Ĳ��֣�
    string ad_button; // ������ť
    const cAni::AnimData *getAd_base(cAni::AnimResManager &arm) const; 
    const cAni::AnimData *getAd_tower(cAni::AnimResManager &arm) const;
    const cAni::AnimData *getAd_button(cAni::AnimResManager &arm) const;

    BaseCannon *createInstance(cAni::AnimResManager &arm) const;
    void releaseInstance(BaseCannon *) const;
};

inline
float BaseCannon::getRange() const
{
    assert(data);
    return data->range;
}

extern CannonData g_cannonData[BaseCannon::NumCannonId];
extern bool CannonInit();
#endif