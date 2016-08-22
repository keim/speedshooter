#include "SDLTextScreen.h"

class debug : public CSDLTextScreen
{
private:
    static debug* me;

public:
    debug()
    {
        me = this;
    }
    
//--------------------------------------------------
// ëÄçÏ 
//--------------------------------------------------
public:
    inline static void cls()
    {
        me->Cls();
    }
    
    inline static void locate(int x, int y)
    {
        me->Locate(x, y);
    }
    
    inline static void printf(const char* strForm, ...)
    {
        va_list arg;
        va_start(arg, strForm);
        me->VPrintf(strForm, arg);
        va_end(arg);
        me->LineFeed();
    }

//--------------------------------------------------
// é¿çs
//--------------------------------------------------
public:
    bool Init()
    {
        if (!LoadFont("system_font.bmp", 0xff00ff, 8, 10, 32)) return false;
        SetScreen(0, 0, CSDLCore::GetScreenWidth() / 6, CSDLCore::GetScreenHeight() / 10);
        return true;
    }
    
    bool Draw()
    {
        if (IsEmpty()) return true;
        
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glColor4f(1, 1, 1, 1);

        return CSDLTextScreen::Draw();
    }
};

debug* debug::me;
