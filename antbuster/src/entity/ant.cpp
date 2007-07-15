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
    hp = 100;
    level = 1;
    speed = 1.0f;
    refCount = 0;

    anim.setAnimData(animResManager.getAnimData("data/ant.xml"), 0);
    hpAnim.setAnimData(animResManager.getAnimData("data/anthp.xml"), 0);
}

void Ant::render(int time)
{
    HGE* hge = hgeCreate(HGE_VERSION);
    hge->Gfx_SetTransform(0, 0, (int)pos.x, (int)pos.y, -angle, 1, 1);
    anim.render(time, 0);
    hge->Gfx_SetTransform(0, 0, (int)pos.x, (int)pos.y, 0, 1, 1);
    hpAnim.render(hp, 0);
    hge->Gfx_SetTransform();
    float alpha = (dest - pos).Length() / 50;
    DWORD color = int(255 * alpha);
    if (color > 255) color = 255;
    color = color<<24 | 0xffffff;
    hge->Gfx_RenderLine(pos.x, pos.y, dest.x, dest.y, color);
    hge->Release();
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
    pos += curdir * speed * (1 - 2 * sqrt(abs(offangle)));
    if (pos.x < border.left)
        pos.x = border.left;
    if (pos.x > border.right)
        pos.x = border.right;
    if (pos.y < border.top)
        pos.y = border.top;
    if (pos.y > border.bottom)
        pos.y = border.bottom;

    if (hp > 0)
        hp--;
    else
        hp = 100;
}