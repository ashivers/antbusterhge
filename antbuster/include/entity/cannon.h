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
    size_t parent; // size_t(-1) ��ʾ������

    string name;
    float freq; // ����Ƶ��
    size_t bulletId;
    const BulletData &getBulletData() const;
    float range; // ���

    string ad_base;  // �������֣��̶����֣�
    string ad_tower; // �������֣�����ת�Ĳ��֣�
    const cAni::AnimData *getAd_base() const; 
    const cAni::AnimData *getAd_tower() const;
    int price; // �������

    BaseCannon *createInstance(cAni::AnimResManager &arm) const;
    void releaseInstance(BaseCannon *) const;
};

extern CannonData g_cannonData[BaseCannon::NumCannonId];

#endif