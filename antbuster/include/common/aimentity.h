#ifndef AIMENTITY_H
#define AIMENTITY_H

#include "common/entity.h"

class AimEntity : public Entity
{
public:
    AimEntity(cAni::AnimResManager &arm) : Entity(arm)
    {
    }
    virtual void renderInfo(int time)
    {
        time;
    }
protected:
};

#endif