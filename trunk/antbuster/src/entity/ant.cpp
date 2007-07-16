#include <hge.h>
#include "caRect.h"
#include "entity/ant.h"

const cAni::Rect border(150 + 16, 650 - 16, 50 + 16, 448 - 16);
const float dlen = 20;
const int dMaxlen = 50;

Ant::Ant(cAni::AnimResManager &arm) : Entity(arm)
{
    dest.x = pos.x = float(rand() % border.GetWidth() + border.left);
    dest.y = pos.y = float(rand() % border.GetHeight() + border.top);
    angle = (float)rand() / RAND_MAX * 3.1415926f;
    level = 1;
    hp = this->getMaxHp();
    refCount = 0;
    memset(damageEffect, 0, sizeof(damageEffect));
    anim.setAnimData(animResManager.getAnimData("data/ant.xml"), 0);
    hpAnim.setAnimData(animResManager.getAnimData("data/anthp.xml"), 0);
}

void Ant::render(int time)
{
    HGE* hge = hgeCreate(HGE_VERSION);
    hge->Gfx_SetTransform(0, 0, (int)pos.x, (int)pos.y, -angle, 1, 1);
    anim.render(time, 0);
    hge->Gfx_SetTransform(0, 0, (int)pos.x, (int)pos.y, 0, 1, 1);
    hpAnim.render(100.0f * hp / this->getMaxHp(), 0);
    hge->Gfx_SetTransform();
    float alpha = (dest - pos).Length() / 50;
    DWORD color = int(255 * alpha);
    if (color > 255) color = 255;
    color = color<<24 | 0xffffff;
    hge->Gfx_RenderLine(pos.x, pos.y, dest.x, dest.y, color);
    hge->Release();
}

void Ant::applyDamage(DamageType damageType, int damage)
{
    assert(damageType >= 0 && damageType < NumDamageType);
    assert(damage > 0);
    this->damageEffect[damageType] += damage;
}
void Ant::applyDamageEffect()
{
    hp -= damageEffect[DT_Normal]; // 普通攻击
    damageEffect[DT_Normal] = 0;   // 立即生效

    hp -= damageEffect[DT_Impact]; // 冲击
    damageEffect[DT_Impact] /= 2;  // 有衰减的效果

    if (damageEffect[DT_Frozen] > 0) // 冰冻
        hp--, damageEffect[DT_Frozen]--; 

    if (damageEffect[DT_Fire] > 0) // 火焰
        hp--, damageEffect[DT_Fire]--; 

    if (damageEffect[DT_Poison] > 0) // 中毒
        hp--, damageEffect[DT_Poison]--; 

    if (hp < 0)
    {
        hp = 0;
        active = false;
    }
}

float Ant::getSpeed() const
{
    float speed = 1.2f;
    // if (cake) speed = 1.0f;
    speed -= 0.1f * damageEffect[DT_Frozen];
    if (speed < 0.3f)
        speed = 0.3f;
    return speed;
}

void Ant::step()
{
    if ((dest - pos).Length() < dlen)
    {
        int dx = rand() % (dMaxlen * 2 + 1) - dMaxlen;
        int dy = rand() % (dMaxlen * 2 + 1) - dMaxlen;
        if (dx > 0) dx += (int)dlen; else dx -= (int)dlen;
        if (dy > 0) dy += (int)dlen; else dy -= (int)dlen;
        dest = hgeVector(pos.x + dx, pos.y + dy);
        if (dest.x < border.left)
            dest.x = border.left;
        if (dest.x > border.right)
            dest.x = border.right;
        if (dest.y < border.top)
            dest.y = border.top;
        if (dest.y > border.bottom)
            dest.y = border.bottom;
    }

    hgeVector curdir(0, -1);
    curdir.Rotate(angle);
    hgeVector destdir = *(dest - pos).Normalize();
    float offangle = curdir.x * destdir.y - curdir.y * destdir.x;
    angle += offangle * 0.05f;
    pos += curdir * getSpeed() * (1 - 2 * sqrt(abs(offangle)));
    if (pos.x < border.left)
        pos.x = border.left;
    if (pos.x > border.right)
        pos.x = border.right;
    if (pos.y < border.top)
        pos.y = border.top;
    if (pos.y > border.bottom)
        pos.y = border.bottom;

    applyDamageEffect();
}