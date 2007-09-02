#ifndef HGEGUIANIMBUTTON_H
#define HGEGUIANIMBUTTON_H


#include "hgeguictrls.h"
#include "caAnimation.h"

/*
** hgeGUIAnimButton
*/
class hgeGUIAnimButton : public hgeGUIObject
{
public:
    hgeGUIAnimButton(int id, float x, float y, float w, float h);
	virtual			~hgeGUIAnimButton();

    enum AnimId
    {
        ButtonUp,
        ButtonDown,
        TriggedUp,
        TriggedDown,

        NumAnimId,
    };
    void SetAnim(AnimId animId, cAni::AnimData *animData)
    {
        anim.setAnimData(animData, animId);
    }

	void			SetMode(bool _bTrigger) { bTrigger=_bTrigger; }
	void			SetState(bool _bPressed) { bPressed=_bPressed; }
	bool			GetState() const { return bPressed; }

	virtual void	Render();
	virtual bool	MouseLButton(bool bDown);

private:
	bool			bTrigger;
	bool			bPressed;
	bool			bOldState;
    cAni::Animation anim;
};

#endif