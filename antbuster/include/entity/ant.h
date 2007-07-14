#ifndef ANT_H
#define ANT_H

#include <hgevector.h>
#include "caAnimation.h"
#include "common/entity.h"

/* @class Ant
 * @note ant
 */

class Ant : public Entity
{
public:
    Ant(cAni::AnimResManager &arm);
    virtual void render(int time);
    virtual void step();
protected:
    int hp;
    int level;

    cAni::Animation anim;
    cAni::Animation hpAnim;
    hgeVector pos;
    float angle;
    float speed;

    hgeVector dest;
};

#endif//ANT_H