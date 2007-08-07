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
BaseCannon *BaseCannon::upgrade(size_t index) const
{
    assert(index < 3);
    BaseCannon::CannonId new_cid = data->evolution[index];
    if (new_cid == BaseCannon::CI_NULL)
        return 0;

    return g_cannonData[new_cid].createInstance(this->animResManager);
}

BaseCannon *BaseCannon::degrade() const
{
    BaseCannon::CannonId new_cid = data->parent;
    if (new_cid == BaseCannon::CI_NULL)
        return 0;

    return g_cannonData[new_cid].createInstance(this->animResManager);
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
��һ����ũ�ڵ�ѵ������Ϊ30�����ũ�ڵĽ������ΪY�����ũ�ڵĽ�����ü��㹫ʽΪ��Yx=Yx-1*1.5n-1����nΪ��������ţ�ֱ��ȥ��С�������ÿ�������Ľ������Ϊ��
��ũ��1 30 45 67 100 150 225 337 505 757 1135 1702 
*/
CannonData g_cannonData[BaseCannon::NumCannonId] = 
{
    //id                          parent                       evolution[3]
    //   name             ����Ƶ��      �ӵ�          ���     ����                   ����                    �������
    {BaseCannon::CI_Cannon,       BaseCannon::CI_NULL,         {BaseCannon::CI_HeavyCannon1, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Cannon",           3.0f, Bullet::BI_CannonA, 110.0f, "data/cannon/base/0.xml", "data/cannon/tower/0.xml", -1},
    {BaseCannon::CI_HeavyCannon1, BaseCannon::CI_Cannon,       {BaseCannon::CI_HeavyCannon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Heavy Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, "data/cannon/base/1.xml", "data/cannon/tower/1.xml", 60},
    {BaseCannon::CI_HeavyCannon2, BaseCannon::CI_HeavyCannon1, {BaseCannon::CI_HeavyCannon3, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Heavy Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, "data/cannon/base/2.xml", "data/cannon/tower/2.xml", 120},
    {BaseCannon::CI_HeavyCannon3, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_NULL,         BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Heavy Cannon 3",   3.0f, Bullet::BI_CannonA, 120.0f, "data/cannon/base/3.xml", "data/cannon/tower/3.xml", 288},
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
void CannonInit()
{
    for (int i = 0; i < BaseCannon::NumCannonId; i++)
    {
        const CannonData &cd = g_cannonData[i];
        assert(cd.id == i);
        for (int j = 0; j < 3; j++)
        {
            BaseCannon::CannonId e = cd.evolution[j];
            if (e == BaseCannon::CI_NULL)
                continue;
            assert(e < BaseCannon::NumCannonId);
            assert(g_cannonData[e].parent == cd.id);
        }
    }
}