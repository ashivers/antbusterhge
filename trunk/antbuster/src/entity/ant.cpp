#include <hge.h>
#include "entity/ant.h"
Ant::Ant()
{
    pos.x = 0;
    pos.y = 0;
    angle = 0;
    hp = 100;
    level = 1;


    anim.setAnimData(, cAni::0);
}

void Ant::render(int time)
{
    HGE* hge = hgeCreate(HGE_VERSION);
    hge->Gfx_SetTransform(pos.x, pos.y, 0, 0, angle, 1, 1);
    anim.render(time, 0);
    hge->Release();
}

void Ant::step()
{

}