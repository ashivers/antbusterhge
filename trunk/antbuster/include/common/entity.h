#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <hge.h>

using namespace std;

class Entity
{
public:
    Entity(cAni::AnimResManager &arm) : animResManager(arm)
    {
    }
    virtual void render(int time)
    {
        time;
    }
    virtual void step()
    {

    }
protected:
    cAni::AnimResManager &animResManager;

private:
    /// not allowed to assign it
    Entity &operator = (const Entity &o)
    {
        o;
        return *this;
    }
};

#endif