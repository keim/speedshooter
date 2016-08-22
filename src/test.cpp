//----------------------------
// Hard cording tuning
//------------------------------
#define _LEVELUP_INTERVAL   600     // Level up ����Frame��
#define _CARD_MOVE_SPEED    40      // �J�[�h�̈ړ��ɂ�����Frame��
#define _CHAIN_INTERVAL     70      // �A�����肳���Frame��
#define _LEVEL_COUNT_ACCEL  60      // �J�[�h�ꖇ�������т�LevelUp����������Frame��
#define _DCLICK_INTERVAL    12      // double click �F���t���[���� 

// �X�N���[���X�s�[�h�̌v�Z���@
#define _SCROLL_SPEED(lev)  ((((lev)%10)+((lev)/10)*2) + 10) * 0.0001f

//------------------------------
// Include flags
//------------------------------
#define  _USE_OPENGL
#define  _USE_MIXER

//------------------------------
// Include files
//------------------------------
#include "util/SDLCore.h"

#include "Manager.h"
#include "ResourceManager.h"
#include "ScoreManager.h"
#include "CardManager.h"
#include "ScreenManager.h"

//--------------------------------------------------------------------------------
// �{�� 
//--------------------------------------------------------------------------------
class CMainApp : public CSDLCore
{
public:
    bool InitInstance(int argc, char *argv[])
    {
        // Set window status
        m_SDLInfo.m_iScreenWidth  = 1024;
        m_SDLInfo.m_iScreenHeight = 768;
        m_SDLInfo.m_bHideCursor   = true;
        m_SDLInfo.m_bFullScreen   = true;
        SetFPS(60);
        SetDrawSkipLimit(5);

        // Read argments
        if (argc > 1) {
            for (int i=1; i<argc; i++) {
                if (strcmp(argv[i], "-window") == 0) {
                    m_SDLInfo.m_bFullScreen = false;
                } else 
                if (strcmp(argv[i], "-res") == 0 && i+2<argc) {
                    m_SDLInfo.m_iScreenWidth  = atoi(argv[++i]);
                    m_SDLInfo.m_iScreenHeight = atoi(argv[++i]);
                }
            }
        }

        // Register managers
        AddThread(&RESOURCE);
        AddThread(&SCORE);
        AddThread(&CARD);
        AddThread(&SCREEN);

        // �L�[�Z�b�e�B���O 
        SetAssignKey(SDLK_F12,  0, SKC_ESCAPE);
        SetAssignKey(SDLK_UP,   0, SKC_YDOWN);
        SetAssignKey(SDLK_DOWN, 0, SKC_YUP);
        SetAssignKey(SDLK_z,    0, SKC_BTN0);
        SetAssignKey(SDLK_x,    0, SKC_BTN1);
        SetAssignKey(SDLK_c,    0, SKC_BTN2);
        SetAssignKey(SDLK_v,    0, SKC_BTN3);
        SetAssignKey(SDLK_LSHIFT, 0, SKC_BTN4);
        SetAssignKey(SDLK_ESCAPE, 0, SKC_BTN5);

        // Randomize
        time_t timer;
        time(&timer);
        srandMT(timer);

        return true;
    }

    bool Init()
    {
        GL::ScreenPers(45, 640, 480, -480, 600);
        GL::SetFog(1, 250, 400, 0);

        glLineWidth(2);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_FOG);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        GL::SetAlphaBlend();

        return true;
    }

    bool Run()
    {
        return true;
    }

    bool Draw()
    {
        float fls = 0.5f - fabsf(CARD.GetScreenFlash() - 0.5f);
        glClearColor(fls*0.1f, fls*0.2f, fls*0.4f, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return true;
    }

    bool Exit()
    {
        return true;
    }

    bool ExitInstance()
    {
        return true;
    }

    bool EventHandler(SDL_Event* pEvent)
    {
        SDL_MouseMotionEvent* pMot;
        SDL_MouseButtonEvent* pBot;
        switch(pEvent->type)
        {
        case SDL_MOUSEMOTION:
            if (RESOURCE.IsMouseMode()) { 
                pMot = ((SDL_MouseMotionEvent*)pEvent);
                CARD.MousePosition(pMot->x, pMot->y);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            pBot = ((SDL_MouseButtonEvent*)pEvent);
            SCORE.MouseBottunPressed(pBot->button);
            break;
        case SDL_MOUSEBUTTONUP:
            pBot = ((SDL_MouseButtonEvent*)pEvent);
            SCORE.MouseBottunReleased(pBot->button);
            break;
        }
        
        return true;
    }
};


// �C���X�^���X
CMainApp theApp;
