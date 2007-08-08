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
    // �ɹ��������򷵻�������ĵ�ַ�����򷵻�0������ɹ�����Ҫɾ��ԭBaseCannon
    BaseCannon *upgrade(size_t index) const;
    // ������ͬ����
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
    BaseCannon::CannonId parent; // -1 ��ʾ������
    BaseCannon::CannonId evolution[3]; // ��������һ������ʾ���õ�CannonInit�ڸ���parent����������ȷ�ԡ�

    string name;
    float freq; // ����Ƶ��
    size_t bulletId;
    const BulletData &getBulletData() const;
    float range; // ���

    string ad_base;  // �������֣��̶����֣�
    string ad_tower; // �������֣�����ת�Ĳ��֣�
    const cAni::AnimData *getAd_base(cAni::AnimResManager &arm) const; 
    const cAni::AnimData *getAd_tower(cAni::AnimResManager &arm) const;
    int price; // �������

    BaseCannon *createInstance(cAni::AnimResManager &arm) const;
    void releaseInstance(BaseCannon *) const;
};

extern CannonData g_cannonData[BaseCannon::NumCannonId];
extern void CannonInit();
#endif