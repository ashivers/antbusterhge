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

    const hgeVector &getPos() const
    {
        return pos;
    }
    int getHp() const
    {
        return hp;
    }
    int refCount; // do not delete the ant if refCount is not 0, this is used by missile like bullets

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

/*
1、蚂蚁的速度是1.2，负重后速度降为1.0，冰冻后最低速度0.3左右。

2、第一个蚂蚁的血量为4，设蚂蚁的血量为Y，则蚂蚁的血量计算公式为“Yx=4*1.1x-1”。 
3、杀死蚂蚁后获得的积分和金钱与蚂蚁的级别相同。

4、屏幕上最多同时会有6只蚂蚁。

5、蚂蚁的AI比看起来的低，经常无视蛋糕四处乱跑。

6、蚂蚁抗上蛋糕的时候会恢复一半的血。
*/

#endif//ANT_H