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

    anim_base.setAnimData(data->getAd_base(this->animResManager), 0);
    anim_tower.setAnimData(data->getAd_tower(this->animResManager), 0);
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

const cAni::AnimData *CannonData::getAd_base(cAni::AnimResManager &arm) const
{
    return arm.getAnimData(this->ad_base.c_str());
}

const cAni::AnimData *CannonData::getAd_tower(cAni::AnimResManager &arm) const
{
    return arm.getAnimData(this->ad_tower.c_str());
}

const cAni::AnimData *CannonData::getAd_button(cAni::AnimResManager &arm) const
{
    return arm.getAnimData(this->ad_button.c_str());
}

/*
��һ����ũ�ڵ�ѵ������Ϊ30�����ũ�ڵĽ������ΪY�����ũ�ڵĽ�����ü��㹫ʽΪ��Yx=Yx-1*1.5n-1����nΪ��������ţ�ֱ��ȥ��С�������ÿ�������Ľ������Ϊ��
��ũ��1 30 45 67 100 150 225 337 505 757 1135 1702 
*/
CannonData g_cannonData[BaseCannon::NumCannonId] = 
{
    //id                          parent                       evolution[3]
    //   name             ����Ƶ��      �ӵ�          ���     �������
    //   ����
    //   ����
    //   ������ť
    {
        BaseCannon::CI_Cannon,       BaseCannon::CI_NULL,         {BaseCannon::CI_HeavyCannon1, BaseCannon::CI_QuickCannon1, BaseCannon::CI_DoubleCannon1,},
        "Cannon",           3.0f, Bullet::BI_CannonA, 110.0f, -1,
        "data/cannon/base/3.xml",
        "data/cannon/tower/10.xml",
        "data/cannon/button/0.xml",
    },
    {
        BaseCannon::CI_HeavyCannon1, BaseCannon::CI_Cannon,       {BaseCannon::CI_HeavyCannon2, BaseCannon::CI_ImpactCannon1, BaseCannon::CI_DoubleHeavyCannon1,},
        "Heavy Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 60,
        "data/cannon/base/4.xml",
        "data/cannon/tower/12.xml",
        "data/cannon/button/0.xml",
    },
    {
        BaseCannon::CI_HeavyCannon2, BaseCannon::CI_HeavyCannon1, {BaseCannon::CI_HeavyCannon3, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Heavy Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 120,
        "data/cannon/base/7.xml",
        "data/cannon/tower/2.xml",
        "data/cannon/button/5.xml",
    },
    {
        BaseCannon::CI_HeavyCannon3, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Heavy Cannon 3",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_MissileLauncher1, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_MissileLauncher2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Missile Launcher 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_MissileLauncher2, BaseCannon::CI_MissileLauncher1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Missile Launcher 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_ImpactCannon1, BaseCannon::CI_HeavyCannon1, {BaseCannon::CI_ImpactCannon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Impact Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/8.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/6.xml",
    },
    {
        BaseCannon::CI_ImpactCannon2, BaseCannon::CI_ImpactCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Impact Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_IceCannon1, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_IceCannon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Ice Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_IceCannon2, BaseCannon::CI_IceCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Ice Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_DoubleHeavyCannon1, BaseCannon::CI_HeavyCannon1, {BaseCannon::CI_DoubleHeavyCannon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Double Heavy Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/9.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/7.xml",
    },
    {
        BaseCannon::CI_DoubleHeavyCannon2, BaseCannon::CI_DoubleHeavyCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Double Heavy Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_SonicPulse1, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_SonicPulse2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Sonic Pulse 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_SonicPulse2, BaseCannon::CI_SonicPulse1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Sonic Pulse 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_QuickCannon1, BaseCannon::CI_Cannon, {BaseCannon::CI_QuickCannon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Quick Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/5.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/1.xml",
    },
    {
        BaseCannon::CI_QuickCannon2, BaseCannon::CI_QuickCannon1, {BaseCannon::CI_QuickCannon3, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Quick Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_QuickCannon3, BaseCannon::CI_QuickCannon2, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Quick Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_FlameThrower1, BaseCannon::CI_ImpactCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Flame Thrower 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_SniperCanon1, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_SniperCanon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Sniper Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_SniperCanon2, BaseCannon::CI_SniperCanon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Sniper Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_Laser1, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_Laser2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Laser 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_Laser2, BaseCannon::CI_Laser1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Laser 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },

    {
        BaseCannon::CI_LongRangeCannon1, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_LongRangeCannon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Long Range Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_LongRangeCannon2, BaseCannon::CI_LongRangeCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Long Range Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },

    {
        BaseCannon::CI_EletricCannon1, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_EletricCannon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Eletric Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_EletricCannon2, BaseCannon::CI_EletricCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Eletric Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },

    {
        BaseCannon::CI_DoubleCannon1, BaseCannon::CI_Cannon, {BaseCannon::CI_DoubleCannon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Double Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/6.xml",
        "data/cannon/tower/14.xml",
        "data/cannon/button/2.xml",
    },
    {
        BaseCannon::CI_DoubleCannon2, BaseCannon::CI_DoubleCannon1, {BaseCannon::CI_DoubleCannon3, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Double Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_DoubleCannon3, BaseCannon::CI_DoubleCannon2, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Double Cannon 3",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_Boomerang, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Boomerang",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_DoubleQuickCannon1, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_DoubleQuickCannon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Double Quick Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_DoubleQuickCannon2, BaseCannon::CI_DoubleQuickCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Double Quick Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_MachineGun, BaseCannon::CI_ImpactCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Machine Gun",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_TripleCannon1, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_TripleCannon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Triple Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_TripleCannon2, BaseCannon::CI_TripleCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Triple Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_PoisonSpray1, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_PoisonSpray2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Poison Spray 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
    {
        BaseCannon::CI_PoisonSpray2, BaseCannon::CI_PoisonSpray1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Poison Spray 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3.xml",
        "data/cannon/tower/3.xml",
        "data/cannon/button/3.xml",
    },
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
    case BaseCannon::CI_MissileLauncher1:
    case BaseCannon::CI_MissileLauncher2:
    case BaseCannon::CI_ImpactCannon1:
    case BaseCannon::CI_ImpactCannon2:
    case BaseCannon::CI_IceCannon1:
    case BaseCannon::CI_IceCannon2:
    case BaseCannon::CI_DoubleHeavyCannon1:
    case BaseCannon::CI_DoubleHeavyCannon2:
    case BaseCannon::CI_SonicPulse1:
    case BaseCannon::CI_SonicPulse2:
    case BaseCannon::CI_QuickCannon1:
    case BaseCannon::CI_QuickCannon2:
    case BaseCannon::CI_QuickCannon3:
    case BaseCannon::CI_FlameThrower1:
    case BaseCannon::CI_SniperCanon1:
    case BaseCannon::CI_SniperCanon2:
    case BaseCannon::CI_Laser1:
    case BaseCannon::CI_Laser2:
    case BaseCannon::CI_LongRangeCannon1:
    case BaseCannon::CI_LongRangeCannon2:
    case BaseCannon::CI_EletricCannon1:
    case BaseCannon::CI_EletricCannon2:
    case BaseCannon::CI_DoubleCannon1:
    case BaseCannon::CI_DoubleCannon2:
    case BaseCannon::CI_DoubleCannon3:
    case BaseCannon::CI_Boomerang:
    case BaseCannon::CI_DoubleQuickCannon1:
    case BaseCannon::CI_DoubleQuickCannon2:
    case BaseCannon::CI_MachineGun:
    case BaseCannon::CI_TripleCannon1:
    case BaseCannon::CI_TripleCannon2:
    case BaseCannon::CI_PoisonSpray1:
    case BaseCannon::CI_PoisonSpray2:
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