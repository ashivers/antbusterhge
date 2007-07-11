#ifndef __ANIM_H_
#define __ANIM_H_

#include <vector>
#include <cassert>
#include <string>

using std::vector;
using std::string;
#include "curvedani.h"
// #include "point1d.h" it is no need, float is ok
#include "caPoint2d.h"
#include "caPoint3d.h"
#include "caRect.h"
#include "caLocus.h"

/*
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
*/
namespace cAni
{
#pragma warning(push)
#pragma warning(disable:4786)

/// AnimElement
/// @note smallest single part of an animation
struct AnimElement
{
    AnimElement();
    void render(/*System &system, */int time, const Rect *cliprect = NULL) const;
    vector<Locus> locus;
    // int m_LocusStartTime[6]; // elements may have the same locus only differs in the start time
    // string m_name;
protected:
    //int m_time;   // anim length
};

/// AnimData 
/// @note set of AnimElements and the total info of the animation
class AnimData 
{
    /// dtor, faces need to be released
    virtual ~AnimData() // use locus_anim_free() function to free the animdata
    {
        release();
    }
    //friend bool locus_anim_read_text(FILE *text, TextureResource *res, AnimData **ppAnimData);
    //friend bool locus_anim_write_text(FILE *text, const TextureResource *res, const AnimData *ppAnimData);
    //friend void locus_anim_free(AnimData *&pAnimData);
    bool loadFromBitStream(iBitStream *bs);
    bool loadFromXml(const char *filename);
    void release();
public:
    friend class Animation;
    friend class AnimResManager;

    /// render
    /// @param  time    [in] frame time
    /// @param  cliprect [in] clipper rectangle (not implemented yet)
    void render(/*System &system, */int time, const Rect *cliprect = NULL) const;
    bool saveToBitStream(iBitStream *bs) const;
    bool saveToXml(const char *filename) const;

    vector<AnimElement> animElements;
    vector<iCurveDataSet*> datasets;
    vector<iCurveInterpolater*> interpolaters;
    int length;
};

/// Animation
/// @note playing instance of the AnimData
class Animation
{
public:
    typedef vector<const AnimData *>::size_type AnimId;
    enum
    {
        DefaultAnimId = ~0,
    };
    Animation(AnimId animIdCount = 1)
    {
        init(animIdCount);
    }
    void setAnimData(const AnimData *pAnimData, AnimId aniId);
    bool startAnim(int curtime, AnimId aniId = DefaultAnimId);

    bool checkEnd(int curtime) const;
    void setAnimLoop(bool bLoop);
    AnimId getCurAnim() const{return m_CurAnimId;}
   // void setAnimTimer(int time);
    
    void render(/*System &system, */int time, const Rect *cliprect) const;

    void setPos(float x, float y)
    {
        m_Pos.x = x;
        m_Pos.y = y;
    }
    void getPos(float &x, float &y) const
    {
        x = m_Pos.x;
        y = m_Pos.y;
    }
protected:
    void init(AnimId animIdCount);
    const AnimData *getCurAnimData() const
    {
        assert(m_CurAnimId == DefaultAnimId || m_CurAnimId < m_AnimDatas.size());
        return m_CurAnimId == DefaultAnimId ? NULL : m_AnimDatas[m_CurAnimId];
    }

    AnimId m_CurAnimId;
    vector<const AnimData *> m_AnimDatas;

    int m_startTime;    // start time in game timing
    Point2f m_Pos;      // anim position 

    // int m_curTime;  // current time in animation timing
    bool m_play;
    bool m_loop;  // is loop ?
};

/// AnimResManager
/// @note   manage AnimData as resources
class AnimResManager
{
public:
    AnimResManager();
    virtual ~AnimResManager();

    /// getAnimData
    /// @param aniFileName  [in] main file name of ani file, no suffix needed
    const AnimData* getAnimData(const char *aniFileName);
private:
    const AnimData* findAnimData(const char *aniFileName) const;
    const AnimData* loadAnimData(const char *aniFileName);
    struct Pair
    {
        Pair(const AnimData *_pAnimData, const char *_anifilename)
            :pAnimData(_pAnimData),anifilename(_anifilename)
        {
        }
        const AnimData* pAnimData;
        string anifilename;
        bool operator == (const char*name) const
        {
            return anifilename == name;
        }
    };
    vector< Pair > m_AnimDatas;
};

// read text formatted AnimData from FILE
//bool locus_anim_read_text(FILE *text, TextureResource *res, AnimData **ppAnimData);
//bool locus_anim_write_text(FILE *text, const TextureResource *res, const AnimData *ppAnimData);
//void locus_anim_free(AnimData *&pAnimData);
/*
// read binary formatted AnimData from sdl_rwops
bool locus_anim_read(SDL_RWops *sdl_rwops, TextureResource *res, AnimData **ppAnimData);
bool locus_anim_write(SDL_RWops *sdl_rwops, TextureResource *res, const AnimData *pAnimData);
*/

}; // namespace cAni

#endif __ANIM_H_