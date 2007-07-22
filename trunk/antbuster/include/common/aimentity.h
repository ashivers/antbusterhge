#ifndef AIMENTITY_H
#define AIMENTITY_H

#include "common/entity.h"

template<class T>
class LinkList
{
public:
    LinkList() : prev(0), next(0)
    {
    }
    virtual ~LinkList()
    {
        disconnect();
    }
    void disconnect()
    {
        if (prev)
            prev->next = next, prev = 0;
        if (next)
            next->prev = prev, next = 0;
    }
    void insertAfter(T &n)
    {
        n.disconnect();
        n.prev = this;
        n.next = next;
        if (next)
            next->prev = &n;
        next = &n;
    }
    T *getPrev()
    {
        return prev;
    }
    T *getNext()
    {
        return next;
    }
protected:
    T *prev;
    T *next;
};

class AimEntity : public Entity, public LinkList<AimEntity>
{
public:
    AimEntity(cAni::AnimResManager &arm) : Entity(arm)
    {
    }
    virtual void renderInfo(int time)
    {
        time;
    }
    const hgeVector &getPos() const
    {
        return pos;
    }
    void setPos(const hgeVector &pos)
    {
        this->pos = pos;
    }
protected:

    hgeVector pos;
};

#endif