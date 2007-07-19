#ifndef MAP_H
#define MAP_H

#include <hgevector.h>
#include "caAnimation.h"
#include "common/entity.h"

using namespace std;

class Map : public Entity
{
public:
    Map(cAni::AnimResManager &arm);
    virtual ~Map();
    virtual void render(int time);
    virtual void step();

    bool checkCannonPos(float &x, float &y) const;
    bool isInMapRange(float x, float y) const;
    void setOccupied(float x, float y);
    void setShowHighLightClip(bool show)
    {
        showHlc = show;
    }
protected:

    hgeVector pos;
    cAni::Animation bg;
    cAni::Animation hlc;
    bool showHlc;

    struct Node
    {
        Node() : occupied(false)
        {

        }
        bool occupied; // by cannon
    };

    hgeVector mouseHotPoint;
    Node *nodes;
    size_t w;
    size_t h;
};

#endif//MAP_H
