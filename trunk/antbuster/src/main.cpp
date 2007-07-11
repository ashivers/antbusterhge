#include "hge.h"
#include "maingamestate.h"
#include "mainmenustate.h"

int APIENTRY WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
{	
    //ÉèÖÃHGEÊôĞÔ		
    HGE *hge = hgeCreate(HGE_VERSION);
    hge->System_SetState(HGE_LOGFILE, "SysInfo.log");
    hge->System_SetState(HGE_INIFILE, "SysINI.ini");
    hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
    hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
    hge->System_SetState(HGE_TITLE, "Antbuster");
    hge->System_SetState(HGE_WINDOWED, true);
    hge->System_SetState(HGE_SCREENWIDTH, 800);
    hge->System_SetState(HGE_SCREENHEIGHT, 600);
    hge->System_SetState(HGE_SCREENBPP, 32);
    hge->System_SetState(HGE_FPS, 60);

#ifdef _DEBUG
#if HGE_VERSION >= 0x170
    hge->System_SetState(HGE_SHOWSPLASH, false);
#else
    hge->System_SetState(hgeIntState(14), 0xFACE0FF);
#endif
#endif

    MainMenuState mms;
    mms.SetName("mainmenu");
    GameStateManager::Instance()->RegisterState(&mms);
    MainGameState mgs;
    mgs.SetName("maingame");
    GameStateManager::Instance()->RegisterState(&mgs);
    GameStateManager::Instance()->RequestState("maingame");

    if (hge->System_Initiate())
    {
        hge->System_Start();
    }

    hge->System_Shutdown();
    hge->Release();
    return 0;
}
