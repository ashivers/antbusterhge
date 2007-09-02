#include "common/ui/hgeGUIAnimButton.h"

/*
** hgeGUIAnimButton
*/

hgeGUIAnimButton::hgeGUIAnimButton(int _id, float x, float y, float w, float h) :
    anim(NumAnimId)
{
	id = _id;
	bStatic = false;
	bVisible = true;
	bEnabled = true;
	rect.Set(x, y, x + w, y + h);

	bPressed = false;
	bTrigger = false;
}

hgeGUIAnimButton::~hgeGUIAnimButton()
{
}

void hgeGUIAnimButton::Render()
{
    HGE *hge = hgeCreate(HGE_VERSION);
    hge->Gfx_SetTransform(rect.x1, rect.y1);
    anim.render();
    hge->Release();
}

bool hgeGUIAnimButton::MouseLButton(bool bDown)
{
	if (bDown)
	{
		bOldState = bPressed;
        bPressed = true;
        anim.startAnim(0, bTrigger && bOldState ? TriggedDown : ButtonDown);
		return false;
	}
	else
	{
		if (bTrigger)
        {
            bPressed = !bOldState;
            anim.startAnim(0, bPressed ? TriggedDown : TriggedUp);
        }
		else 
        {
            bPressed = false;
            anim.startAnim(0, ButtonUp);
        }
		return true; 
	}
}