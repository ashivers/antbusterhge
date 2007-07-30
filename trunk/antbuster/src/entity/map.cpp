#include "entity/map.h"
#include "game/bgdata.h"

const cAni::Rect border(150 + 16, 650 - 16, 50 + 16, 448 - 16);

Map::Map(cAni::AnimResManager &arm) : Entity(arm), bg(2), hlc(2)
{
    bg.setAnimData(this->animResManager.getAnimData("data/bg0.xml"), 0); // 
    bg.setAnimData(this->animResManager.getAnimData("data/bg1.xml"), 1);
    hlc.setAnimData(this->animResManager.getAnimData("data/highlight0.xml"), 0); // 可以安放
    hlc.setAnimData(this->animResManager.getAnimData("data/highlight1.xml"), 1); // 禁止摆放

    HGE *hge = hgeCreate(HGE_VERSION);
    pos = hgeVector(hge->System_GetState(HGE_SCREENWIDTH) / 2, hge->System_GetState(HGE_SCREENHEIGHT) / 2);
    bg.startAnim(int(60 * hge->Timer_GetTime()), 0);
    hge->Release();
    
    w = border.GetWidth() / 16;
    h = border.GetHeight() / 16;
    nodes = new Node[w * h];

    showHlc = false;
    mouseHotPoint = hgeVector(8, 7);
}
Map::~Map()
{
    if (nodes)
    {
        delete [] nodes;
        nodes = 0;
    }
}
void Map::render(int time)
{
    HGE* hge = hgeCreate(HGE_VERSION);
    hge->Gfx_SetTransform(0, 0, (int)pos.x, (int)pos.y, 0, 1, 1);
    bg.render(time, 0);
    if (showHlc)
    {
        float x, y;
        hge->Input_GetMousePos(&x, &y);
        if (isInMapRange(x, y))
        {
            if (checkCannonPos(x, y))
            {
                hlc.startAnim(0, 0);
            }
            else
            {
                hlc.startAnim(0, 1);
            }
            hge->Gfx_SetTransform(0, 0, (int)x, (int)y, 0, 1, 1);
            hlc.render(time, 0);
        }
    }
    hge->Release();
}

void Map::step()
{

}
bool Map::isInMapRange(float x, float y) const
{
    x += mouseHotPoint.x;
    y += mouseHotPoint.y;
    return x >= border.left && x < border.right && y >= border.top && y < border.bottom;
}
bool Map::checkCannonPos(float &x, float &y) const
{
    if (!isInMapRange(x, y))
        return false;

    x += mouseHotPoint.x;
    y += mouseHotPoint.y;
    int bx = (x - border.left) / 16;
    int by = (y - border.top) / 16;
    x = bx * 16 + border.left;
    y = by * 16 + border.top ;
    if (bx >= 0 && bx < BGH_BLOCK_MAX &&
        by >= 0 && by < BGV_BLOCK_MAX &&
        bgData[bx][by] == '1')
    {
        if (bx < w && by < h)
        {
            int xlo = bx > 0 ? bx - 1 : 0, xhi = bx < w - 1 ? bx + 1 : w - 1;
            int ylo = by > 0 ? by - 1 : 0, yhi = by < h - 1 ? by + 1 : h - 1;
            for (int i = xlo; i <= xhi; i++)
                for (int j = ylo; j <= yhi; j++)
                    if (nodes[i + j * w].occupied)
                        return false;
            return true;
        }
    }
    return false;
}

void Map::setOccupied(float x, float y)
{
    int bx = (x - border.left) / 16;
    int by = (y - border.top) / 16;
    if (bx >= 0 && bx < w && by >= 0 && by < h)
    {
        nodes[bx + by * w].occupied = true;
    }
}