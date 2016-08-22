//--------------------------------------------------------------------------------
//  SDL OpenGL Core v0.2.0
//
//    Copyright  (C)  2006  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//
//--------------------------------------------------------------------------------
/*
メモ；
【VC6でコンソールアプリケーションとして作成する場合】
    1.「 ﾌﾟﾛｼﾞｪｸﾄ → 設定 → C/C++ → ｶﾃｺﾞﾘ → ｺｰﾄﾞ生成 → 使用するﾗﾝﾀｲﾑﾗｲﾌﾞﾗﾘ 」を
       リリース「ﾏﾙﾁｽﾚｯﾄﾞ(DLL)」/ デバッグ「ﾏﾙﾁｽﾚｯﾄﾞ(DLL，ﾃﾞﾊﾞｯｸﾞ)」に設定．
    2.「 ﾌﾟﾛｼﾞｪｸﾄ → 設定 → ﾘﾝｸ → ｶﾃｺﾞﾘ → ｲﾝﾌﾟｯﾄ → 無視するﾗｲﾌﾞﾗﾘ 」に，
       リリース「libc」/ デバッグ「libcd, msvcrt.lib」を追加．
       
【マクロ】
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
    static float g_fRefleshInterval = 16.666666f;   // 設定された更新間隔
    static int   g_iDrawSkipMax = 0;                // Draw()を飛ばす最大フレーム数．
    static int   g_iFPSAveragingFrame = 30;         // Reflesh Rate の計算時に平均するフレーム数
    static float g_fFPSAverage = 60.0f;             // 前回計算した 平均 Reflesh Rate
    static bool  g_bSlowAssert = false;             // 設定された Reflesh Rate を満たせなかったら true に設定される
    static int   g_iSkipDraw;                       // g_iSkipDraw != 0 なら，Draw()を最大 g_iSkipDraw 回スキップする

    // 平均 Reflesh Rate を計算する（SyncFPS()から描画可能なタイミング毎に呼ばれる）
    static inline void CalcFPS()
    {
    	// Frame数のカウント
        static int iFrameCount = 0;
    	
    	// 前回呼び出された時間
        static long lPrevCalcTick = SDL_GetTicks();
    	
    	// 平均 Reflesh Rate を計算
        if (++iFrameCount == g_iFPSAveragingFrame) {
            iFrameCount = 0;
            long lCurrentTick = SDL_GetTicks();
            long lInterval = lCurrentTick - lPrevCalcTick;
            lPrevCalcTick = lCurrentTick;
            g_fFPSAverage = (lInterval == 0) ? 0.0f : ((float)g_iFPSAveragingFrame / (float)lInterval * 1000.0f);
        }
    }


    // 設定した Reflesh Rate にタイミングを合わせる（::MainLoop() 内から呼ばれる）
    static inline void SyncFPS()
    {
    	// 待ち時間の次回持ち越し分
        static float fOddInterval = 0.0f;
    	
    	// 前回呼び出した時間
        static long  lPrevTick = SDL_GetTicks();

    	// 現在の時間
        long lCurrentTick = SDL_GetTicks();
    	
    	// 待ち時間の計算
        fOddInterval += g_fRefleshInterval - (float)(lCurrentTick - lPrevTick);

        if (fOddInterval > 0) {
            // 待ち時間有り（Draw()をスキップしない）

        	// Draw()はスキップしない
        	g_iSkipDraw = 0;
        	// 待ち時間計算
            int iOddInterval = (int)fOddInterval;
            // 待つ
            if (iOddInterval != 0) SDL_Delay(iOddInterval);
            // FPS を計算する
        	CalcFPS();
        	// スローは起きていない
            g_bSlowAssert = false;
        } else {
            // 待ち時間無し（Draw()をスキップする）

        	// スキップ回数のインクリメント
        	++g_iSkipDraw;
        	
        	// もうこれ以上Draw()のスキップは出来無い
            if (g_iSkipDraw > g_iDrawSkipMax) {
            	// Draw()はスキップしない
                g_iSkipDraw = 0;
            	// 待ち時間の次回持ち越しは無し
                fOddInterval = 0.0f;
            	// FPS を計算する
                CalcFPS();
            	// スロー発生
                g_bSlowAssert = true;
            }
        	
            // Draw()をスキップする場合，fOddInterval は丸々次回に持ち越される
        }
    	
        lPrevTick = lCurrentTick;
    }

    // Draw() 呼び出し可能か？
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
// メンバ構造体
//--------------------------------------------------
public: 
    // SDL設定
    struct SDL_INFOMATION
    {
        // SDL 設定
        //------------------------------
        int             m_iScreenWidth;     // スクリーン幅
        int             m_iScreenHeight;    // スクリーン高さ
        int             m_iBitPerPixel;     // 1 pixel あたりの bit 数
        bool            m_bFullScreen;      // フルスクリーン
        bool            m_bHideCursor;      // マウスカーソル
        unsigned long   m_ulSDLInitFlag;    // SDL初期化フラグ
        SDL_Surface*    m_pIcon;            // アイコン用サーフィスのポインタ
        char            m_strTitle[SDLCORE_WINDOW_TITLE_MAX+1]; // ウィンドウタイトル

        // SDL Mixer 設定
        //   #define _USE_MIXER で使用 
        //------------------------------
        struct tag_Mixer {
            unsigned long   m_ulFreq;       // 周波数
            unsigned short  m_usFormat;     // フォーマット
            bool            m_bStereo;      // ステレオフラグ
            unsigned long   m_ulBufferSize; // バッファサイズ
        } Mixer;                            // SDL_Mixer 情報

        // SDL Net 設定
        //   #define _USE_NET で使用 
        //------------------------------
        struct tag_Net {
        } Net;                              // SDL_Net 情報

        // OpenGL 設定
        //   #define _USE_OPENGL で使用 
        //------------------------------
        struct tag_OpenGL
        {
            int     m_iRedChannelBit;       // 赤チャンネルのビット数
            int     m_iGreenChannelBit;     // 緑チャンネルのビット数
            int     m_iBlueChannelBit;      // 青チャンネルのビット数
            int     m_iAlphaChannelBit;     // アルファチャンネルのビット数

            int     m_iDepthBufferSize;     // Z バッファのサイズ
            int     m_iStencilBufferSize;   // ステンシルバッファのサイズ
            bool    m_bDoubleBuffer;        // ダブルバッファフラグ

            int     m_iRedAccumBit;         // 赤アキュームバッファのビット数
            int     m_iGreenAccumBit;       // 緑アキュームバッファのビット数
            int     m_iBlueAccumBit;        // 青アキュームバッファのビット数
            int     m_iAlphaAccumBit;       // アルファアキュームバッファのビット数

            int     GetFrameBufferBit()     // フレームバッファのビット数を計算する
            {
                return ((m_iRedChannelBit + m_iGreenChannelBit + m_iBlueChannelBit + m_iAlphaChannelBit + 7) & 0xf8);
            }

            void    UseAccumBuffer()        // アキュムレーションバッファを使う
            {
                m_iRedAccumBit   = m_iRedChannelBit;
                m_iGreenAccumBit = m_iGreenChannelBit;
                m_iBlueAccumBit  = m_iBlueChannelBit;
                m_iAlphaAccumBit = m_iAlphaChannelBit;
            }
        } OpenGL;

    }; 

//--------------------------------------------------
// メンバ変数
//--------------------------------------------------
public:
    // 設定
    struct SDL_INFOMATION       m_SDLInfo;

    // スレッド管理
    CSDLThreadManager           m_ThreadManager;

    // 入力管理
    CSDLControlManager          m_ControlManager;

    // インスタンスのポインタ
    static CSDLCore*            s_pInstance;

private:
    // エラーフラグ
    int                         m_iErrorCode;           // エラーコード
    char                        m_strErrorMessage[256]; // エラーメッセージ
    bool                        m_bInitialized;         // 初期化済フラグ

    // MainLoop を抜けるフラグ(内部で使用)
    bool                        m_bExitMainLoop;

    // スクリーンサーフィス(内部で使用)
    SDL_Surface*                m_pScreenSurface;

//--------------------------------------------------
// コンストラクタ，デストラクタ
//--------------------------------------------------
public:
    CSDLCore();
    ~CSDLCore();

//--------------------------------------------------
// コールバックされる仮想関数
//--------------------------------------------------
public:
    virtual bool InitInstance(int argc, char *argv[]);  // オプションを処理したい場合は，こっちをインプリメントする
    virtual bool InitInstance();                        // 一番最初に呼ばれる（SDL，GL の初期化よりも前）
//  virtual bool Init();
//  virtual bool Run();
//  virtual bool Draw();
//  virtual bool Exit();
//  virtual bool InitScene();
//  virtual bool ExitScene();
    virtual bool ExitInstance();                        // 一番最後に呼ばれる（SDL，GL の終了処理よりも後）
    virtual bool EventHandler(SDL_Event* pEvent);       // イベント発生時に呼ばれる

//--------------------------------------------------
// SDL 初期化/終了処理
//--------------------------------------------------
public:
    bool InitSDL();
    bool ExitSDL();

//--------------------------------------------------
// 情報参照（スタティック）
//--------------------------------------------------
public:
	// スクリーン縦幅取得
    static inline int GetScreenWidth()
    {
        return s_pInstance->m_SDLInfo.m_iScreenWidth;
    }

	// スクリーン横幅取得
    static inline int GetScreenHeight()
    {
        return s_pInstance->m_SDLInfo.m_iScreenHeight;
    }

//--------------------------------------------------
// Reflesh Rate 管理（スタティック）
//--------------------------------------------------
public:
    // Reflesh Rate を返す
    static inline float GetFPS()
    {
        return _NS_REFLESH_RATE::g_fFPSAverage;
    }

    // return true; でスロー発生中
    static inline bool SlowAssert()
    {
        return _NS_REFLESH_RATE::g_bSlowAssert;
    }

    // GetFPS() で返される Reflesh Rate を計算する際に，平均化するFrame数の設定
    static inline void SetFPSAveragingFrame(int iFrame)
    {
        _NS_REFLESH_RATE::g_iFPSAveragingFrame = (iFrame < 1) ? 1 : iFrame;
    }

    // Reflesh Rate の設定
    static inline void SetFPS(int iFPS)
    {
        // 画面の更新間隔[ms]を計算
        _NS_REFLESH_RATE::g_fFPSAverage = (float)iFPS;
        _NS_REFLESH_RATE::g_fRefleshInterval = (iFPS == 0) ? (16.666666f) : (1000.0f / (float)iFPS);
    }

    // 設定した Reflesh Rate に間に合わなかった場合，Draw()を飛ばす最大Frame数の設定
    static inline void SetDrawSkipLimit(int iSkipFrameMax)
    {
        if (iSkipFrameMax < 0) iSkipFrameMax = 0;
        _NS_REFLESH_RATE::g_iDrawSkipMax = iSkipFrameMax;
    }

    // 前回のMeasureInterval() 呼び出しから，今回の呼び出しまでの時間間隔[ms]を測定する
    // 初回呼び出しは 0(稀に1) を返す
    static inline long MeasureInterval()
    {
        static long lPrevCallTime = SDL_GetTicks();
        long lCurrentTime = SDL_GetTicks();
        long lInterval = lCurrentTime - lPrevCallTime;
        lPrevCallTime = lCurrentTime;
        return lInterval;
    }

//--------------------------------------------------
// Controler 管理（スタティック）
//--------------------------------------------------
public:
    // 指定されたキーが押されているか(SDLKeyをダイレクトに指定)
    static inline bool IsPressed(SDLKey enSDLKeyCode)
    {
        return (s_pInstance->m_ControlManager.GetKeyboardTable()[enSDLKeyCode] == SDL_PRESSED);
    }

    // KeyConfig 設定
    static inline SDLKey SetAssignKey(SDLKey enNewKey, int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        return s_pInstance->m_ControlManager.SetAssignKey(enNewKey, iCtrlIdx, enAssign);
    }

    // JoyStickボタンの割り当て
    static inline int SetAssignJSButton(int iJSButton, int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        return s_pInstance->m_ControlManager.SetAssignJSButton(iJSButton, iCtrlIdx, enAssign);
    }

    // KeyConfig 設定 読込み
    static inline bool LoadKeyConfigFile(char* strFile)
    {
        return s_pInstance->m_ControlManager.Load(strFile);
    }

    // KeyConfig 設定 書込み
    static inline bool SaveKeyConfigFile(char* strFile)
    {
        return s_pInstance->m_ControlManager.Save(strFile);
    }

    // 指定されたキーが押されているか(KeyConfig/Joystick参照)
    static inline bool IsPressed(int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        return s_pInstance->m_ControlManager.IsPressed(iCtrlIdx, enAssign);
    }

    // 指定されたキーが押されたか(KeyConfig/Joystick参照)
    static inline bool IsHitted(int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        return s_pInstance->m_ControlManager.IsHitted(iCtrlIdx, enAssign);
    }


//--------------------------------------------------
// Thread 管理（スタティック）
//--------------------------------------------------
public:
    // スレッドチェインの最後にスレッドを追加．
    static inline void AddThread(IThread* pThread)
    {
        s_pInstance->m_ThreadManager.AddThread(pThread);
    }
    
    // 指定スレッドの後ろにスレッドを追加．
    static inline void InsertThreadAfter(IThread* pInsertAfter, IThread* pNewThread)
    {
        s_pInstance->m_ThreadManager.InsertThreadAfter(pInsertAfter, pNewThread);
    }


//--------------------------------------------------
// Scene 管理（スタティック）
//--------------------------------------------------
public:
    // InitScene() を呼び出さずにシーンをリセットする
    static inline void ResetScene(int iSceneIndex)
    {
        s_pInstance->m_ThreadManager.ResetScene(iSceneIndex);
    }

    // 現在のScene 番号を返す
    static inline int GetScene()
    {
        return s_pInstance->m_ThreadManager.GetScene();
    }

    // Scene 番号を変更する（次フレームの最初に変更が行われる）
    static inline void ChangeScene(int iSceneIndex)
    {
        s_pInstance->m_ThreadManager.ChangeScene(iSceneIndex);
    }


//--------------------------------------------------
// Error 管理（スタティック）
//--------------------------------------------------
public:
    // Main Loop から強制脱出する(Run(),Draw() 内で有効)
    static inline void ExitMainLoop(int iErrorCode)
    {
        s_pInstance->m_iErrorCode    = iErrorCode;
        s_pInstance->m_bExitMainLoop = true;
    }

    // Error Code の参照
    static inline int GetErrorCode()
    {
        return s_pInstance->m_iErrorCode;
    }

    // Error Message の取得
    static inline char* GetErrorMessage()
    {
        return s_pInstance->m_strErrorMessage;
    }

    // SDL 初期化済みか
    static inline bool IsInitialized()
    {
        return s_pInstance->m_bInitialized;
    }

    // 強制脱出フラグの取得（内部で使用） 
    static inline bool _exitflag()
    {
        return s_pInstance->m_bExitMainLoop;
    }


//--------------------------------------------------
// 描画関連（スタティック）
//--------------------------------------------------
public:
    // スクリーンサーフィスの取得 
    static inline SDL_Surface* GetScreenSurface()
    {
        return s_pInstance->m_pScreenSurface;
    }
    
#ifdef _USE_OPENGL
    // 初期化時に DepthBuffer を確保しているか
    static inline bool IsUsingDepthBuffer()
    {
        return (s_pInstance->m_SDLInfo.OpenGL.m_iDepthBufferSize > 0);
    }

    // 初期化時に StencilBuffer を確保しているか
    static inline bool IsUsingStencilBuffer()
    {
        return (s_pInstance->m_SDLInfo.OpenGL.m_iStencilBufferSize > 0);
    }

    // 初期化時に AlphaChannel を確保しているか
    static inline bool IsUsingAlphaChannel()
    {
        return (s_pInstance->m_SDLInfo.OpenGL.m_iAlphaChannelBit > 0);
    }

#else
    // Screen Format にコンパチな SDL_Surface の作成
    static inline SDL_Surface* SDLCreateSurface(int width, int height, bool bSystemMemory=false)
    {
        SDL_PixelFormat *form = GetScreenSurface()->format;
        return SDL_CreateRGBSurface((bSystemMemory) ? (SDL_SWSURFACE) : (SDL_HWSURFACE), width, height, form->BitsPerPixel,
                                    form->Rmask, form->Gmask, form->Bmask, form->Amask);
    }
#endif
};














//--------------------------------------------------------------------------------
//  エントリポイント
//--------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // Main Instance が登録されていなければ，Error
    if (CSDLCore::s_pInstance == NULL) return -1;

	// main() 返値
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
// 初期化処理
bool MainInit(int argc, char *argv[])
{
    // CSDLCore::InitInstance() の呼び出し
    if (!CSDLCore::s_pInstance->InitInstance(argc, argv)) return false;

    // SDL初期化されてなかったら初期化
    if (!CSDLCore::s_pInstance->IsInitialized()) {
        if (!CSDLCore::s_pInstance->InitSDL()) return false;
    }

    return true;
}


// ループ処理
bool MainLoop()
{
    bool        bExitLoop;      // ループ脱出フラグ
    SDL_Event   SDLEventMsg;    // メッセージ
    SDLKey      enEscapeKey = CSDLCore::s_pInstance->m_ControlManager.GetAssignKey(0, SKC_ESCAPE); // 強制終了キーの取得
    
    // Init() 呼び出し
    if (!CSDLCore::s_pInstance->m_ThreadManager.Init())  return false;
    // 最初のシーンの InitScene() 呼び出し
    CSDLCore::s_pInstance->m_ThreadManager.InitScene();

    // メインループ
    while (true) {
        // イベントを処理
        bExitLoop = false;
        while (SDL_PollEvent(&(SDLEventMsg))) {
            // 入力情報を更新
            CSDLCore::s_pInstance->m_ControlManager.UpdateFlag();
            // イベントを処理
            if (!CSDLCore::s_pInstance->EventHandler(&(SDLEventMsg)))             bExitLoop = true;
            if (CSDLCore::IsPressed(enEscapeKey) || SDLEventMsg.type == SDL_QUIT) bExitLoop = true;
        }
        if (bExitLoop) break;

        // FPSに合せる
        _NS_REFLESH_RATE::SyncFPS();

        // IThreadManager::RunScene() 呼び出し
        if (!CSDLCore::s_pInstance->m_ThreadManager.RunScene()) break;

        // Run() 呼び出し
        if (!CSDLCore::s_pInstance->m_ThreadManager.Run()) break;

        // Draw() 呼び出し
        if (_NS_REFLESH_RATE::IsDrawableFrame()) {
            if (!CSDLCore::s_pInstance->m_ThreadManager.Draw()) break;

            // Swap Buffers
#ifdef _USE_OPENGL
            SDL_GL_SwapBuffers();
#else
            SDL_Flip(CSDLCore::GetScreenSurface());
#endif
        }

        // CSDLCore::ExitMainLoop() が呼ばれた
        if (CSDLCore::_exitflag()) break;
    }

    // 最後のシーンの ExitScene() 呼び出し
    CSDLCore::s_pInstance->m_ThreadManager.ExitScene();
    // Exit() 呼び出し
    if (!CSDLCore::s_pInstance->m_ThreadManager.Exit())  return false;

    return true;
}


// 終了処理
bool MainExit()
{
	// CSDLCore::ExitSDL() の呼び出し
    if (!CSDLCore::s_pInstance->ExitSDL()) return false;
	
    // CSDLCore::ExitInstance() の呼び出し
	if (!CSDLCore::s_pInstance->ExitInstance()) return false;
    return true;
}













//--------------------------------------------------------------------------------
//  CSDLCore 実装
//--------------------------------------------------------------------------------
//--------------------------------------------------
// インスタンスのポインタ
//--------------------------------------------------
CSDLCore*           CSDLCore::s_pInstance = NULL;

//--------------------------------------------------
// コンストラクタ，デストラクタ
//--------------------------------------------------
CSDLCore::CSDLCore()
{
    // 初期設定
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

    // インスタンスのポインタを登録
    s_pInstance = this;
    // スレッドチェインの先頭に自分を登録
    AddThread(this);
}

CSDLCore::~CSDLCore()
{
}


//--------------------------------------------------
//  コールバックされる仮想関数
//--------------------------------------------------
bool CSDLCore::InitInstance(int argc, char *argv[]) {return InitInstance();}
bool CSDLCore::InitInstance() {return true;}
bool CSDLCore::ExitInstance() {return true;}
bool CSDLCore::EventHandler(SDL_Event* pEvent) {return true;}


//--------------------------------------------------
//  SDL 初期化/終了処理
//--------------------------------------------------
bool CSDLCore::InitSDL()
{
    // SDL 初期化
    if (SDL_Init(m_SDLInfo.m_ulSDLInitFlag) < 0) {
        sprintf(m_strErrorMessage, "Error in Initialize SDL : %s\n", SDL_GetError());
        m_iErrorCode = _SDLCORE_ERROR_INIT_SDL;
        return false;
    }


    // SDL Mixer 初期化
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


    // SDL Net 初期化
#ifdef _USE_NET
    if (SDLNet_Init() < 0) {
        sprintf(m_strErrorMessage, "Error in Initialize SDL Net : %s\n", SDLNet_GetError());
        m_iErrorCode = _SDLCORE_ERROR_INIT_SDLNET;
        return false;
    }
#endif


    // OpenGL 設定
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


    // Video Mode 初期化
    if (m_SDLInfo.m_bFullScreen) ulVideoMode |= SDL_FULLSCREEN;
    m_pScreenSurface = SDL_SetVideoMode(m_SDLInfo.m_iScreenWidth, m_SDLInfo.m_iScreenHeight, 
                                      m_SDLInfo.m_iBitPerPixel, ulVideoMode);
    if (m_pScreenSurface == NULL) {
        sprintf(m_strErrorMessage, "Error in Setting Video Mode : %s\n", SDL_GetError());
        m_iErrorCode = _SDLCORE_ERROR_INIT_VIDEOMODE;
        return false;
    }


    // その他初期化
    SDL_ShowCursor((m_SDLInfo.m_bHideCursor) ? SDL_DISABLE : SDL_ENABLE);       // Cursor 初期化
    SDL_WM_SetCaption(m_SDLInfo.m_strTitle, m_SDLInfo.m_strTitle);              // Caption 設定
    if (m_SDLInfo.m_pIcon != NULL) SDL_WM_SetIcon(m_SDLInfo.m_pIcon, NULL);     // Icon 設定
    s_pInstance->m_ControlManager.UpdateJoystickInfo();                         // JoyStick 初期化
    

    // OpenGL 初期化
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

