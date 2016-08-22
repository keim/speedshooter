//--------------------------------------------------------------------------------
//  SDL OpenGL Core v0.2.0
//
//    Copyright  (C)  2006  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//
//--------------------------------------------------------------------------------
/*
�����G
�yVC6�ŃR���\�[���A�v���P�[�V�����Ƃ��č쐬����ꍇ�z
    1.�u ��ۼު�� �� �ݒ� �� C/C++ �� �ú�� �� ���ސ��� �� �g�p���������ײ���� �v��
       �����[�X�u����گ��(DLL)�v/ �f�o�b�O�u����گ��(DLL�C���ޯ��)�v�ɐݒ�D
    2.�u ��ۼު�� �� �ݒ� �� �ݸ �� �ú�� �� ���߯� �� ��������ײ���� �v�ɁC
       �����[�X�ulibc�v/ �f�o�b�O�ulibcd, msvcrt.lib�v��ǉ��D
       
�y�}�N���z
#define _USE_MIXER
#define _USE_NET
#define _USE_OPENGL
*/

//--------------------------------------------------------------------------------
//  Macro
//--------------------------------------------------------------------------------
#define SDLCORE_WINDOW_TITLE_MAX    256

//--------------------------------------------------------------------------------
//  Library setting for Visual C++
//--------------------------------------------------------------------------------
#pragma comment (lib, "../sdl/winlib/SDL.lib")
#pragma comment (lib, "../sdl/winlib/SDLmain.lib")
#ifdef _USE_MIXER
  #pragma comment (lib, "../sdl/winlib/SDL_mixer.lib")
#endif
#ifdef _USE_NET
  #pragma comment (lib, "../sdl/winlib/SDL_net.lib")
#endif

//--------------------------------------------------------------------------------
//  Include File
//--------------------------------------------------------------------------------
// SDL Header
#include "std_include.h"
#include "../sdl/SDL.h"

#ifdef _USE_MIXER
  #include "../sdl/SDL_mixer.h"
  #include "SDLWave.h"
#endif

#ifdef _USE_NET
  #include "../sdl/SDL_net.h"
#endif

// OpenGL Header
#ifdef _USE_OPENGL
  #include "../sdl/SDL_opengl.h"
  #include "include_opengl.h"
#endif

// SDLCore Header
#include "SDLThread.h"
#include "SDLControlManager.h"

//--------------------------------------------------------------------------------
//  Error Code Enum
//--------------------------------------------------------------------------------
enum {
    _SDLCORE_NO_ERROR = 0,
    _SDLCORE_ERROR_INIT_SDL = -1,
    _SDLCORE_ERROR_INIT_SDLMIXER = -2,
    _SDLCORE_ERROR_INIT_SDLNET = -3,
    _SDLCORE_ERROR_INIT_VIDEOMODE = -4,
};

//--------------------------------------------------------------------------------
//  Global Function
//--------------------------------------------------------------------------------
static bool MainInit(int argc, char *argv[]);
static bool MainLoop();
static bool MainExit();

//--------------------------------------------------------------------------------
//  Reflesh Rate Manager
//--------------------------------------------------------------------------------
namespace _NS_REFLESH_RATE
{
    static float g_fRefleshInterval = 16.666666f;   // �ݒ肳�ꂽ�X�V�Ԋu
    static int   g_iDrawSkipMax = 0;                // Draw()���΂��ő�t���[�����D
    static int   g_iFPSAveragingFrame = 30;         // Reflesh Rate �̌v�Z���ɕ��ς���t���[����
    static float g_fFPSAverage = 60.0f;             // �O��v�Z���� ���� Reflesh Rate
    static bool  g_bSlowAssert = false;             // �ݒ肳�ꂽ Reflesh Rate �𖞂����Ȃ������� true �ɐݒ肳���
    static int   g_iSkipDraw;                       // g_iSkipDraw != 0 �Ȃ�CDraw()���ő� g_iSkipDraw ��X�L�b�v����

    // ���� Reflesh Rate ���v�Z����iSyncFPS()����`��\�ȃ^�C�~���O���ɌĂ΂��j
    static inline void CalcFPS()
    {
    	// Frame���̃J�E���g
        static int iFrameCount = 0;
    	
    	// �O��Ăяo���ꂽ����
        static long lPrevCalcTick = SDL_GetTicks();
    	
    	// ���� Reflesh Rate ���v�Z
        if (++iFrameCount == g_iFPSAveragingFrame) {
            iFrameCount = 0;
            long lCurrentTick = SDL_GetTicks();
            long lInterval = lCurrentTick - lPrevCalcTick;
            lPrevCalcTick = lCurrentTick;
            g_fFPSAverage = (lInterval == 0) ? 0.0f : ((float)g_iFPSAveragingFrame / (float)lInterval * 1000.0f);
        }
    }


    // �ݒ肵�� Reflesh Rate �Ƀ^�C�~���O�����킹��i::MainLoop() ������Ă΂��j
    static inline void SyncFPS()
    {
    	// �҂����Ԃ̎��񎝂��z����
        static float fOddInterval = 0.0f;
    	
    	// �O��Ăяo��������
        static long  lPrevTick = SDL_GetTicks();

    	// ���݂̎���
        long lCurrentTick = SDL_GetTicks();
    	
    	// �҂����Ԃ̌v�Z
        fOddInterval += g_fRefleshInterval - (float)(lCurrentTick - lPrevTick);

        if (fOddInterval > 0) {
            // �҂����ԗL��iDraw()���X�L�b�v���Ȃ��j

        	// Draw()�̓X�L�b�v���Ȃ�
        	g_iSkipDraw = 0;
        	// �҂����Ԍv�Z
            int iOddInterval = (int)fOddInterval;
            // �҂�
            if (iOddInterval != 0) SDL_Delay(iOddInterval);
            // FPS ���v�Z����
        	CalcFPS();
        	// �X���[�͋N���Ă��Ȃ�
            g_bSlowAssert = false;
        } else {
            // �҂����Ԗ����iDraw()���X�L�b�v����j

        	// �X�L�b�v�񐔂̃C���N�������g
        	++g_iSkipDraw;
        	
        	// ��������ȏ�Draw()�̃X�L�b�v�͏o������
            if (g_iSkipDraw > g_iDrawSkipMax) {
            	// Draw()�̓X�L�b�v���Ȃ�
                g_iSkipDraw = 0;
            	// �҂����Ԃ̎��񎝂��z���͖���
                fOddInterval = 0.0f;
            	// FPS ���v�Z����
                CalcFPS();
            	// �X���[����
                g_bSlowAssert = true;
            }
        	
            // Draw()���X�L�b�v����ꍇ�CfOddInterval �͊ہX����Ɏ����z�����
        }
    	
        lPrevTick = lCurrentTick;
    }

    // Draw() �Ăяo���\���H
    static inline bool IsDrawableFrame()
    {
        return (g_iSkipDraw == 0);
    }
}






//--------------------------------------------------------------------------------
//  CSDLCore
//--------------------------------------------------------------------------------
class CSDLCore : public IThread
{
//--------------------------------------------------
// �����o�\����
//--------------------------------------------------
public: 
    // SDL�ݒ�
    struct SDL_INFOMATION
    {
        // SDL �ݒ�
        //------------------------------
        int             m_iScreenWidth;     // �X�N���[����
        int             m_iScreenHeight;    // �X�N���[������
        int             m_iBitPerPixel;     // 1 pixel ������� bit ��
        bool            m_bFullScreen;      // �t���X�N���[��
        bool            m_bHideCursor;      // �}�E�X�J�[�\��
        unsigned long   m_ulSDLInitFlag;    // SDL�������t���O
        SDL_Surface*    m_pIcon;            // �A�C�R���p�T�[�t�B�X�̃|�C���^
        char            m_strTitle[SDLCORE_WINDOW_TITLE_MAX+1]; // �E�B���h�E�^�C�g��

        // SDL Mixer �ݒ�
        //   #define _USE_MIXER �Ŏg�p 
        //------------------------------
        struct tag_Mixer {
            unsigned long   m_ulFreq;       // ���g��
            unsigned short  m_usFormat;     // �t�H�[�}�b�g
            bool            m_bStereo;      // �X�e���I�t���O
            unsigned long   m_ulBufferSize; // �o�b�t�@�T�C�Y
        } Mixer;                            // SDL_Mixer ���

        // SDL Net �ݒ�
        //   #define _USE_NET �Ŏg�p 
        //------------------------------
        struct tag_Net {
        } Net;                              // SDL_Net ���

        // OpenGL �ݒ�
        //   #define _USE_OPENGL �Ŏg�p 
        //------------------------------
        struct tag_OpenGL
        {
            int     m_iRedChannelBit;       // �ԃ`�����l���̃r�b�g��
            int     m_iGreenChannelBit;     // �΃`�����l���̃r�b�g��
            int     m_iBlueChannelBit;      // �`�����l���̃r�b�g��
            int     m_iAlphaChannelBit;     // �A���t�@�`�����l���̃r�b�g��

            int     m_iDepthBufferSize;     // Z �o�b�t�@�̃T�C�Y
            int     m_iStencilBufferSize;   // �X�e���V���o�b�t�@�̃T�C�Y
            bool    m_bDoubleBuffer;        // �_�u���o�b�t�@�t���O

            int     m_iRedAccumBit;         // �ԃA�L���[���o�b�t�@�̃r�b�g��
            int     m_iGreenAccumBit;       // �΃A�L���[���o�b�t�@�̃r�b�g��
            int     m_iBlueAccumBit;        // �A�L���[���o�b�t�@�̃r�b�g��
            int     m_iAlphaAccumBit;       // �A���t�@�A�L���[���o�b�t�@�̃r�b�g��

            int     GetFrameBufferBit()     // �t���[���o�b�t�@�̃r�b�g�����v�Z����
            {
                return ((m_iRedChannelBit + m_iGreenChannelBit + m_iBlueChannelBit + m_iAlphaChannelBit + 7) & 0xf8);
            }

            void    UseAccumBuffer()        // �A�L�������[�V�����o�b�t�@���g��
            {
                m_iRedAccumBit   = m_iRedChannelBit;
                m_iGreenAccumBit = m_iGreenChannelBit;
                m_iBlueAccumBit  = m_iBlueChannelBit;
                m_iAlphaAccumBit = m_iAlphaChannelBit;
            }
        } OpenGL;

    }; 

//--------------------------------------------------
// �����o�ϐ�
//--------------------------------------------------
public:
    // �ݒ�
    struct SDL_INFOMATION       m_SDLInfo;

    // �X���b�h�Ǘ�
    CSDLThreadManager           m_ThreadManager;

    // ���͊Ǘ�
    CSDLControlManager          m_ControlManager;

    // �C���X�^���X�̃|�C���^
    static CSDLCore*            s_pInstance;

private:
    // �G���[�t���O
    int                         m_iErrorCode;           // �G���[�R�[�h
    char                        m_strErrorMessage[256]; // �G���[���b�Z�[�W
    bool                        m_bInitialized;         // �������σt���O

    // MainLoop �𔲂���t���O(�����Ŏg�p)
    bool                        m_bExitMainLoop;

    // �X�N���[���T�[�t�B�X(�����Ŏg�p)
    SDL_Surface*                m_pScreenSurface;

//--------------------------------------------------
// �R���X�g���N�^�C�f�X�g���N�^
//--------------------------------------------------
public:
    CSDLCore();
    ~CSDLCore();

//--------------------------------------------------
// �R�[���o�b�N����鉼�z�֐�
//--------------------------------------------------
public:
    virtual bool InitInstance(int argc, char *argv[]);  // �I�v�V�����������������ꍇ�́C���������C���v�������g����
    virtual bool InitInstance();                        // ��ԍŏ��ɌĂ΂��iSDL�CGL �̏����������O�j
//  virtual bool Init();
//  virtual bool Run();
//  virtual bool Draw();
//  virtual bool Exit();
//  virtual bool InitScene();
//  virtual bool ExitScene();
    virtual bool ExitInstance();                        // ��ԍŌ�ɌĂ΂��iSDL�CGL �̏I������������j
    virtual bool EventHandler(SDL_Event* pEvent);       // �C�x���g�������ɌĂ΂��

//--------------------------------------------------
// SDL ������/�I������
//--------------------------------------------------
public:
    bool InitSDL();
    bool ExitSDL();

//--------------------------------------------------
// ���Q�Ɓi�X�^�e�B�b�N�j
//--------------------------------------------------
public:
	// �X�N���[���c���擾
    static inline int GetScreenWidth()
    {
        return s_pInstance->m_SDLInfo.m_iScreenWidth;
    }

	// �X�N���[�������擾
    static inline int GetScreenHeight()
    {
        return s_pInstance->m_SDLInfo.m_iScreenHeight;
    }

//--------------------------------------------------
// Reflesh Rate �Ǘ��i�X�^�e�B�b�N�j
//--------------------------------------------------
public:
    // Reflesh Rate ��Ԃ�
    static inline float GetFPS()
    {
        return _NS_REFLESH_RATE::g_fFPSAverage;
    }

    // return true; �ŃX���[������
    static inline bool SlowAssert()
    {
        return _NS_REFLESH_RATE::g_bSlowAssert;
    }

    // GetFPS() �ŕԂ���� Reflesh Rate ���v�Z����ۂɁC���ω�����Frame���̐ݒ�
    static inline void SetFPSAveragingFrame(int iFrame)
    {
        _NS_REFLESH_RATE::g_iFPSAveragingFrame = (iFrame < 1) ? 1 : iFrame;
    }

    // Reflesh Rate �̐ݒ�
    static inline void SetFPS(int iFPS)
    {
        // ��ʂ̍X�V�Ԋu[ms]���v�Z
        _NS_REFLESH_RATE::g_fFPSAverage = (float)iFPS;
        _NS_REFLESH_RATE::g_fRefleshInterval = (iFPS == 0) ? (16.666666f) : (1000.0f / (float)iFPS);
    }

    // �ݒ肵�� Reflesh Rate �ɊԂɍ���Ȃ������ꍇ�CDraw()���΂��ő�Frame���̐ݒ�
    static inline void SetDrawSkipLimit(int iSkipFrameMax)
    {
        if (iSkipFrameMax < 0) iSkipFrameMax = 0;
        _NS_REFLESH_RATE::g_iDrawSkipMax = iSkipFrameMax;
    }

    // �O���MeasureInterval() �Ăяo������C����̌Ăяo���܂ł̎��ԊԊu[ms]�𑪒肷��
    // ����Ăяo���� 0(�H��1) ��Ԃ�
    static inline long MeasureInterval()
    {
        static long lPrevCallTime = SDL_GetTicks();
        long lCurrentTime = SDL_GetTicks();
        long lInterval = lCurrentTime - lPrevCallTime;
        lPrevCallTime = lCurrentTime;
        return lInterval;
    }

//--------------------------------------------------
// Controler �Ǘ��i�X�^�e�B�b�N�j
//--------------------------------------------------
public:
    // �w�肳�ꂽ�L�[��������Ă��邩(SDLKey���_�C���N�g�Ɏw��)
    static inline bool IsPressed(SDLKey enSDLKeyCode)
    {
        return (s_pInstance->m_ControlManager.GetKeyboardTable()[enSDLKeyCode] == SDL_PRESSED);
    }

    // KeyConfig �ݒ�
    static inline SDLKey SetAssignKey(SDLKey enNewKey, int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        return s_pInstance->m_ControlManager.SetAssignKey(enNewKey, iCtrlIdx, enAssign);
    }

    // JoyStick�{�^���̊��蓖��
    static inline int SetAssignJSButton(int iJSButton, int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        return s_pInstance->m_ControlManager.SetAssignJSButton(iJSButton, iCtrlIdx, enAssign);
    }

    // KeyConfig �ݒ� �Ǎ���
    static inline bool LoadKeyConfigFile(char* strFile)
    {
        return s_pInstance->m_ControlManager.Load(strFile);
    }

    // KeyConfig �ݒ� ������
    static inline bool SaveKeyConfigFile(char* strFile)
    {
        return s_pInstance->m_ControlManager.Save(strFile);
    }

    // �w�肳�ꂽ�L�[��������Ă��邩(KeyConfig/Joystick�Q��)
    static inline bool IsPressed(int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        return s_pInstance->m_ControlManager.IsPressed(iCtrlIdx, enAssign);
    }

    // �w�肳�ꂽ�L�[�������ꂽ��(KeyConfig/Joystick�Q��)
    static inline bool IsHitted(int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        return s_pInstance->m_ControlManager.IsHitted(iCtrlIdx, enAssign);
    }


//--------------------------------------------------
// Thread �Ǘ��i�X�^�e�B�b�N�j
//--------------------------------------------------
public:
    // �X���b�h�`�F�C���̍Ō�ɃX���b�h��ǉ��D
    static inline void AddThread(IThread* pThread)
    {
        s_pInstance->m_ThreadManager.AddThread(pThread);
    }
    
    // �w��X���b�h�̌��ɃX���b�h��ǉ��D
    static inline void InsertThreadAfter(IThread* pInsertAfter, IThread* pNewThread)
    {
        s_pInstance->m_ThreadManager.InsertThreadAfter(pInsertAfter, pNewThread);
    }


//--------------------------------------------------
// Scene �Ǘ��i�X�^�e�B�b�N�j
//--------------------------------------------------
public:
    // InitScene() ���Ăяo�����ɃV�[�������Z�b�g����
    static inline void ResetScene(int iSceneIndex)
    {
        s_pInstance->m_ThreadManager.ResetScene(iSceneIndex);
    }

    // ���݂�Scene �ԍ���Ԃ�
    static inline int GetScene()
    {
        return s_pInstance->m_ThreadManager.GetScene();
    }

    // Scene �ԍ���ύX����i���t���[���̍ŏ��ɕύX���s����j
    static inline void ChangeScene(int iSceneIndex)
    {
        s_pInstance->m_ThreadManager.ChangeScene(iSceneIndex);
    }


//--------------------------------------------------
// Error �Ǘ��i�X�^�e�B�b�N�j
//--------------------------------------------------
public:
    // Main Loop ���狭���E�o����(Run(),Draw() ���ŗL��)
    static inline void ExitMainLoop(int iErrorCode)
    {
        s_pInstance->m_iErrorCode    = iErrorCode;
        s_pInstance->m_bExitMainLoop = true;
    }

    // Error Code �̎Q��
    static inline int GetErrorCode()
    {
        return s_pInstance->m_iErrorCode;
    }

    // Error Message �̎擾
    static inline char* GetErrorMessage()
    {
        return s_pInstance->m_strErrorMessage;
    }

    // SDL �������ς݂�
    static inline bool IsInitialized()
    {
        return s_pInstance->m_bInitialized;
    }

    // �����E�o�t���O�̎擾�i�����Ŏg�p�j 
    static inline bool _exitflag()
    {
        return s_pInstance->m_bExitMainLoop;
    }


//--------------------------------------------------
// �`��֘A�i�X�^�e�B�b�N�j
//--------------------------------------------------
public:
    // �X�N���[���T�[�t�B�X�̎擾 
    static inline SDL_Surface* GetScreenSurface()
    {
        return s_pInstance->m_pScreenSurface;
    }
    
#ifdef _USE_OPENGL
    // ���������� DepthBuffer ���m�ۂ��Ă��邩
    static inline bool IsUsingDepthBuffer()
    {
        return (s_pInstance->m_SDLInfo.OpenGL.m_iDepthBufferSize > 0);
    }

    // ���������� StencilBuffer ���m�ۂ��Ă��邩
    static inline bool IsUsingStencilBuffer()
    {
        return (s_pInstance->m_SDLInfo.OpenGL.m_iStencilBufferSize > 0);
    }

    // ���������� AlphaChannel ���m�ۂ��Ă��邩
    static inline bool IsUsingAlphaChannel()
    {
        return (s_pInstance->m_SDLInfo.OpenGL.m_iAlphaChannelBit > 0);
    }

#else
    // Screen Format �ɃR���p�`�� SDL_Surface �̍쐬
    static inline SDL_Surface* SDLCreateSurface(int width, int height, bool bSystemMemory=false)
    {
        SDL_PixelFormat *form = GetScreenSurface()->format;
        return SDL_CreateRGBSurface((bSystemMemory) ? (SDL_SWSURFACE) : (SDL_HWSURFACE), width, height, form->BitsPerPixel,
                                    form->Rmask, form->Gmask, form->Bmask, form->Amask);
    }
#endif
};














//--------------------------------------------------------------------------------
//  �G���g���|�C���g
//--------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // Main Instance ���o�^����Ă��Ȃ���΁CError
    if (CSDLCore::s_pInstance == NULL) return -1;

	// main() �Ԓl
	int return_value = 0;
	
    // Main Routine
	if (!MainInit(argc, argv)) return_value = -1;
    else
	if (!MainLoop())           return_value = -1;
	
	if (!MainExit())           return_value = -1;

    return return_value;
}


//--------------------------------------------------
//  Sub Routine
//--------------------------------------------------
// ����������
bool MainInit(int argc, char *argv[])
{
    // CSDLCore::InitInstance() �̌Ăяo��
    if (!CSDLCore::s_pInstance->InitInstance(argc, argv)) return false;

    // SDL����������ĂȂ������珉����
    if (!CSDLCore::s_pInstance->IsInitialized()) {
        if (!CSDLCore::s_pInstance->InitSDL()) return false;
    }

    return true;
}


// ���[�v����
bool MainLoop()
{
    bool        bExitLoop;      // ���[�v�E�o�t���O
    SDL_Event   SDLEventMsg;    // ���b�Z�[�W
    SDLKey      enEscapeKey = CSDLCore::s_pInstance->m_ControlManager.GetAssignKey(0, SKC_ESCAPE); // �����I���L�[�̎擾
    
    // Init() �Ăяo��
    if (!CSDLCore::s_pInstance->m_ThreadManager.Init())  return false;
    // �ŏ��̃V�[���� InitScene() �Ăяo��
    CSDLCore::s_pInstance->m_ThreadManager.InitScene();

    // ���C�����[�v
    while (true) {
        // �C�x���g������
        bExitLoop = false;
        while (SDL_PollEvent(&(SDLEventMsg))) {
            // ���͏����X�V
            CSDLCore::s_pInstance->m_ControlManager.UpdateFlag();
            // �C�x���g������
            if (!CSDLCore::s_pInstance->EventHandler(&(SDLEventMsg)))             bExitLoop = true;
            if (CSDLCore::IsPressed(enEscapeKey) || SDLEventMsg.type == SDL_QUIT) bExitLoop = true;
        }
        if (bExitLoop) break;

        // FPS�ɍ�����
        _NS_REFLESH_RATE::SyncFPS();

        // IThreadManager::RunScene() �Ăяo��
        if (!CSDLCore::s_pInstance->m_ThreadManager.RunScene()) break;

        // Run() �Ăяo��
        if (!CSDLCore::s_pInstance->m_ThreadManager.Run()) break;

        // Draw() �Ăяo��
        if (_NS_REFLESH_RATE::IsDrawableFrame()) {
            if (!CSDLCore::s_pInstance->m_ThreadManager.Draw()) break;

            // Swap Buffers
#ifdef _USE_OPENGL
            SDL_GL_SwapBuffers();
#else
            SDL_Flip(CSDLCore::GetScreenSurface());
#endif
        }

        // CSDLCore::ExitMainLoop() ���Ă΂ꂽ
        if (CSDLCore::_exitflag()) break;
    }

    // �Ō�̃V�[���� ExitScene() �Ăяo��
    CSDLCore::s_pInstance->m_ThreadManager.ExitScene();
    // Exit() �Ăяo��
    if (!CSDLCore::s_pInstance->m_ThreadManager.Exit())  return false;

    return true;
}


// �I������
bool MainExit()
{
	// CSDLCore::ExitSDL() �̌Ăяo��
    if (!CSDLCore::s_pInstance->ExitSDL()) return false;
	
    // CSDLCore::ExitInstance() �̌Ăяo��
	if (!CSDLCore::s_pInstance->ExitInstance()) return false;
    return true;
}













//--------------------------------------------------------------------------------
//  CSDLCore ����
//--------------------------------------------------------------------------------
//--------------------------------------------------
// �C���X�^���X�̃|�C���^
//--------------------------------------------------
CSDLCore*           CSDLCore::s_pInstance = NULL;

//--------------------------------------------------
// �R���X�g���N�^�C�f�X�g���N�^
//--------------------------------------------------
CSDLCore::CSDLCore()
{
    // �����ݒ�
    m_SDLInfo.m_iScreenWidth = 640;
    m_SDLInfo.m_iScreenHeight = 480;
    m_SDLInfo.m_iBitPerPixel = 32;
    m_SDLInfo.m_bFullScreen = false;
    m_SDLInfo.m_bHideCursor = true;
    m_SDLInfo.m_ulSDLInitFlag = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK;
    m_SDLInfo.m_pIcon = NULL;
    strcpy(m_SDLInfo.m_strTitle, "");

#ifdef _USE_MIXER
    m_SDLInfo.Mixer.m_ulFreq = MIX_DEFAULT_FREQUENCY;
    m_SDLInfo.Mixer.m_usFormat = MIX_DEFAULT_FORMAT;
    m_SDLInfo.Mixer.m_bStereo = false;
    m_SDLInfo.Mixer.m_ulBufferSize = 2048;
#endif

#ifdef _USE_OPENGL
    m_SDLInfo.OpenGL.m_iRedChannelBit = 8;
    m_SDLInfo.OpenGL.m_iGreenChannelBit = 8;
    m_SDLInfo.OpenGL.m_iBlueChannelBit = 8;
    m_SDLInfo.OpenGL.m_iAlphaChannelBit = 8;

    m_SDLInfo.OpenGL.m_iDepthBufferSize = 32;
    m_SDLInfo.OpenGL.m_iStencilBufferSize = 0;
    m_SDLInfo.OpenGL.m_bDoubleBuffer = true;

    m_SDLInfo.OpenGL.m_iRedAccumBit = 0;
    m_SDLInfo.OpenGL.m_iGreenAccumBit = 0;
    m_SDLInfo.OpenGL.m_iBlueAccumBit = 0;
    m_SDLInfo.OpenGL.m_iAlphaAccumBit = 0;
#endif

    m_iErrorCode    = _SDLCORE_NO_ERROR;
    m_bExitMainLoop = false;
    m_bInitialized  = false;

    m_pScreenSurface = NULL;

    // �C���X�^���X�̃|�C���^��o�^
    s_pInstance = this;
    // �X���b�h�`�F�C���̐擪�Ɏ�����o�^
    AddThread(this);
}

CSDLCore::~CSDLCore()
{
}


//--------------------------------------------------
//  �R�[���o�b�N����鉼�z�֐�
//--------------------------------------------------
bool CSDLCore::InitInstance(int argc, char *argv[]) {return InitInstance();}
bool CSDLCore::InitInstance() {return true;}
bool CSDLCore::ExitInstance() {return true;}
bool CSDLCore::EventHandler(SDL_Event* pEvent) {return true;}


//--------------------------------------------------
//  SDL ������/�I������
//--------------------------------------------------
bool CSDLCore::InitSDL()
{
    // SDL ������
    if (SDL_Init(m_SDLInfo.m_ulSDLInitFlag) < 0) {
        sprintf(m_strErrorMessage, "Error in Initialize SDL : %s\n", SDL_GetError());
        m_iErrorCode = _SDLCORE_ERROR_INIT_SDL;
        return false;
    }


    // SDL Mixer ������
#ifdef _USE_MIXER
    if (Mix_OpenAudio(m_SDLInfo.Mixer.m_ulFreq,
                      m_SDLInfo.Mixer.m_usFormat,
                      (m_SDLInfo.Mixer.m_bStereo) ? 2 : 1,
                      m_SDLInfo.Mixer.m_ulBufferSize) < 0) {
        sprintf(m_strErrorMessage, "Error in Initialize SDL Mixer : %s\n", SDL_GetError());
        m_iErrorCode = _SDLCORE_ERROR_INIT_SDLMIXER;
        return false;
    }
#endif


    // SDL Net ������
#ifdef _USE_NET
    if (SDLNet_Init() < 0) {
        sprintf(m_strErrorMessage, "Error in Initialize SDL Net : %s\n", SDLNet_GetError());
        m_iErrorCode = _SDLCORE_ERROR_INIT_SDLNET;
        return false;
    }
#endif


    // OpenGL �ݒ�
#ifdef _USE_OPENGL
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     m_SDLInfo.OpenGL.m_iRedChannelBit);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   m_SDLInfo.OpenGL.m_iGreenChannelBit);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    m_SDLInfo.OpenGL.m_iBlueChannelBit);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   m_SDLInfo.OpenGL.m_iAlphaChannelBit);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,  m_SDLInfo.OpenGL.GetFrameBufferBit());

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, (m_SDLInfo.OpenGL.m_bDoubleBuffer) ? 1 : 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   m_SDLInfo.OpenGL.m_iDepthBufferSize);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, m_SDLInfo.OpenGL.m_iStencilBufferSize);

    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,   m_SDLInfo.OpenGL.m_iRedAccumBit);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, m_SDLInfo.OpenGL.m_iGreenAccumBit);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,  m_SDLInfo.OpenGL.m_iBlueAccumBit);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, m_SDLInfo.OpenGL.m_iAlphaAccumBit);

    unsigned long   ulVideoMode = SDL_HWSURFACE | SDL_OPENGL;
#else 

    unsigned long   ulVideoMode = SDL_HWSURFACE | SDL_DOUBLEBUF;
#endif


    // Video Mode ������
    if (m_SDLInfo.m_bFullScreen) ulVideoMode |= SDL_FULLSCREEN;
    m_pScreenSurface = SDL_SetVideoMode(m_SDLInfo.m_iScreenWidth, m_SDLInfo.m_iScreenHeight, 
                                      m_SDLInfo.m_iBitPerPixel, ulVideoMode);
    if (m_pScreenSurface == NULL) {
        sprintf(m_strErrorMessage, "Error in Setting Video Mode : %s\n", SDL_GetError());
        m_iErrorCode = _SDLCORE_ERROR_INIT_VIDEOMODE;
        return false;
    }


    // ���̑�������
    SDL_ShowCursor((m_SDLInfo.m_bHideCursor) ? SDL_DISABLE : SDL_ENABLE);       // Cursor ������
    SDL_WM_SetCaption(m_SDLInfo.m_strTitle, m_SDLInfo.m_strTitle);              // Caption �ݒ�
    if (m_SDLInfo.m_pIcon != NULL) SDL_WM_SetIcon(m_SDLInfo.m_pIcon, NULL);     // Icon �ݒ�
    s_pInstance->m_ControlManager.UpdateJoystickInfo();                         // JoyStick ������
    

    // OpenGL ������
#ifdef _USE_OPENGL
    if (IsUsingDepthBuffer())   glEnable (GL_DEPTH_TEST);
    else                        glDisable(GL_DEPTH_TEST);
    if (IsUsingStencilBuffer()) glEnable (GL_STENCIL_TEST);
    else                        glDisable(GL_STENCIL_TEST);
    if (IsUsingAlphaChannel())  glEnable (GL_BLEND);
    else                        glDisable(GL_BLEND);

    glViewport(0, 0, m_SDLInfo.m_iScreenWidth, m_SDLInfo.m_iScreenHeight);
    GL::ScreenPers(45, (float)m_SDLInfo.m_iScreenWidth, (float)m_SDLInfo.m_iScreenHeight, -100, 1000);
    glLoadIdentity();
#endif

    m_bInitialized = true;
    return true;
}

bool CSDLCore::ExitSDL()
{
    // Show Cursor
    SDL_ShowCursor(SDL_ENABLE);

    // Close Mixer
#ifdef _USE_MIXER
    Mix_CloseAudio();
#endif

    // Close Net
#ifdef _USE_NET
    SDLNet_Quit();
#endif

    // Free Screen Surface
    SDL_FreeSurface(m_pScreenSurface);
    // ShutDown SDL
    SDL_Quit();

    return true;
}

