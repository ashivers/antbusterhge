#include "entity/cannon.h"
#include "entity/bullet.h"
#include "entity/ant.h"
#include "game/maingamestate.h"
BaseCannon::BaseCannon(cAni::AnimResManager &arm, const CannonData *_data) : AimEntity(arm), data(_data)
{
    assert(data);

    pos;
    targetPos;
    anim_base;
    anim_tower;
    lastFireTime = 0;

    anim_base.setAnimData(data->getAd_base(), 0);
    anim_tower.setAnimData(data->getAd_tower(), 0);
}
void BaseCannon::render(int time)
{
    HGE* hge = hgeCreate(HGE_VERSION);
    hge->Gfx_SetTransform(0, 0, (float)(int)pos.x, (float)(int)pos.y, 0, 1, 1);
    anim_base.render(time, 0);

    hgeVector dir = targetPos - pos;
    hgeVector up(0, -1);
    if (dir.Length() < 0.5f)
    {
        dir = up;
    }
    else
    {
        dir.Normalize();
    }
    float angle = dir.Angle(&up);
    if (dir.x > 0)
        angle = -angle;
    hge->Gfx_SetTransform(0, 0, (float)(int)pos.x, (float)(int)pos.y, angle, 1, 1);
    anim_tower.render(time, 0);
    hge->Release();
}

void BaseCannon::step()
{
    Ant *ant = MainGameState::GetInstance()->getTargetAnt(this->pos);
    if (ant)
    {
        hgeVector tp = ant->getPos();
        HGE* hge = hgeCreate(HGE_VERSION);
        float curtime = hge->Timer_GetTime();
        if ((curtime - lastFireTime) * this->data->freq > 1 && (tp - this->pos).Length() <= this->data->range)
        {
            this->targetPos = tp;
            lastFireTime = curtime;
            hgeVector dir = ant->getPos() - this->pos;
            dir.Normalize();
            dir *= this->data->range;
            MainGameState::GetInstance()->fire(this->pos, dir, this->data->getBulletData());
        }
        hge->Release();
    }
}

const BulletData &CannonData::getBulletData() const
{
    assert(this->bulletId < Bullet::NumBulletId);
    return g_bulletData[this->bulletId];
}

const cAni::AnimData *CannonData::getAd_base() const
{
    return MainGameState::GetInstance()->GetAnimResManager()->getAnimData(this->ad_base.c_str());
}

const cAni::AnimData *CannonData::getAd_tower() const
{
    return MainGameState::GetInstance()->GetAnimResManager()->getAnimData(this->ad_tower.c_str());
}

/*
第一个加农炮的训练费用为30，设加农炮的建造费用为Y，则加农炮的建造费用计算公式为“Yx=Yx-1*1.5n-1”，n为炮塔的序号，直接去除小数，因此每个炮塔的建造费用为：
加农炮1 30 45 67 100 150 225 337 505 757 1135 1702 
*/
CannonData g_cannonData[BaseCannon::NumCannonId] = 
{
    //id                      parent
    //   name             攻击频率      子弹          射程     炮座                   炮塔                    建造费用
    {BaseCannon::CI_Cannon, size_t(-1),
        "Cannon",           3.0f, Bullet::BI_CannonA, 110.0f, "data/cannon_base.xml", "data/cannon_tower.xml", -1},
    {BaseCannon::CI_HeavyCannon1, BaseCannon::CI_Cannon,
        "Heavy Cannon 1",   3.0f, Bullet::BI_CannonB, 120.0f, "data/cannon_base.xml", "data/cannon_tower.xml", 60},
    {BaseCannon::CI_HeavyCannon2, BaseCannon::CI_HeavyCannon1,
        "Heavy Cannon 2",   3.0f, Bullet::BI_CannonC, 120.0f, "data/cannon_base.xml", "data/cannon_tower.xml", 120},
    {BaseCannon::CI_HeavyCannon3, BaseCannon::CI_HeavyCannon2,
        "Heavy Cannon 3",   3.0f, Bullet::BI_CannonD, 120.0f, "data/cannon_base.xml", "data/cannon_tower.xml", 288},
};
BaseCannon *CannonData::createInstance(cAni::AnimResManager &arm) const
{
    BaseCannon *cannon = 0;
    switch(this->id)
    {
    case BaseCannon::CI_Cannon:
    case BaseCannon::CI_HeavyCannon1:
    case BaseCannon::CI_HeavyCannon2:
    case BaseCannon::CI_HeavyCannon3:
        cannon = new BaseCannon(arm, this);
        break;
    /* add new cannon like this
    case BaseCannon::CI_MissleLauncher:
        cannon = new MissileLauncher;
        break;
    */
    default:
        assert(0);
        break;
    }
    return cannon;
}
void CannonData::releaseInstance(BaseCannon *cannon) const
{
    assert(cannon && cannon->data == this);
    delete cannon;
}