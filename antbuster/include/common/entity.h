#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <hge.h>

using namespace std;

class Entity
{
public:
    Entity(cAni::AnimResManager &arm) : animResManager(arm), active(true)
    {
    }
    virtual void render(int time)
    {
        time;
    }
    virtual void step()
    {

    }
    bool isActive() const
    {
        return active;
    }
protected:
    cAni::AnimResManager &animResManager;

    bool active;
private:
    /// not allowed to assign it
    Entity &operator = (const Entity &o)
    {
        o;
        return *this;
    }
};

#endif