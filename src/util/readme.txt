
【使い方】
    1. CSDLCore
      1.0. CSLDCore の継承クラスを作成し，グローバル変数としてインスタンスを一個定義する． 
           この CSLDCore の継承クラスインスタンスの仮想関数は，main() から適宜呼び出される． 
      1.1. CSDLCore 継承クラスでは， 
           virtual bool CSDLCore::InitInstance(int argc, char *argv[]) or CSDLCore::InitInstance()
           virtual bool CSDLCore::ExitInstance()
           virtual bool CSDLCore::Init()
           virtual bool CSDLCore::Exit()
           virtual bool CSDLCore::Run()
           virtual bool CSDLCore::Draw()
           virtual bool CSDLCore::InitScene()
           virtual bool CSDLCore::ExitScene()
           virtual bool CSDLCore::EventHandler()
           の8個の仮想関数を実装する．
      1.2. InitInstance() は，アプリ起動直後に呼び出される． 
    2. IThread 継承クラスでは， 
       virtual bool IThread::Init()
       virtual bool IThread::Run()
       virtual bool IThread::Draw()
       virtual bool IThread::Exit()
       virtual bool IThread::InitScene()
       virtual bool IThread::ExitScene()
       を適宜実装する．
    3. static CSDLCore::AddThread() で，IThread 継承クラスのインスタンスを登録する．
    3. Init()は MainLoop() に入る直前，Exit()は MainLoop() から出た直後に呼ばれる． 
    4. Run() / Draw() は，CSDLCore::SetFPS() で設定した Reflesh Rate 毎に MainLoop() から呼ばれる．
    5. InitScene() / ExitScene() は，CSDLCore::ChangeScene() が呼ばれたフレームの次フレームの最初に呼ばれる．
    4. コマンドライン引数を参照する場合は，InitInstance(int argc, char *argv[]) を実装する．
    6. #define _USE_OPENGL  で OpenGL 対応．
    7. #define _USE_MIXER   で SDL_mixer 対応．
    8. #define _USE_NET     で SDL_net 対応．
    9. CSDLCore::ExitMainLoop()で，MainLoop を強制脱出．
   10. CSDLCore::GetErrorCode()で，CSDLCore::ExitMainLoop()で指定した引数を取得できる．
   11. CSDLCore::RunAllThread()で，CSDLCore::AddThread()登録したThread の停止/再開ができる．

【IThread について】
    0. IThread 継承クラスを static CSDLCore::AddThread() で登録すると，MainLoop()内から呼び出されるようになる．
    1. Thread の登録は通常，CSDLCore::InitInstance() / Init() 内で行う．
    2. 登録時，引数 iPrior を小さく設定したスレッドが先に実行され，
       同値なら，AddThread()した順に実行される．Exit() 関数は呼び出し順が逆．
    3. IThread::Alive()   で On/Off 設定．Off の場合，IThread::Run() / Draw() が呼び出されなくなる．
    4. IThread::IsAlive() で On/Off 参照．

【CSDLCore/IThread のコールバック関数が呼び出される順番】
    virtual bool CSDLCore::InitInstance();  // 一番最初に呼ばれる
    virtual bool CSDLCore::InitSDL();       // （SDL の初期化処理）
    virtual bool CSDLCore::InitGL();        // （OpenGL の初期化処理）
    virtual bool CSDLCore::Init();          // メインループに入る前に呼ばれる
    virtual bool IThread ::Init();          // AddThread()で登録したIThread::Init()は，このタイミングで呼ばれる
    MainLoop() {                            // （m_SDLInfo.m_iFramePerSecond で設定した更新間隔で毎回呼ばれる）
    virtual bool CSDLCore::EventHandler();  // （イベント発生時に呼ばれる）
    virtual bool CSDLCore::Run();           // 
    virtual bool IThread ::Run();           // 
    virtual bool CSDLCore::Draw();          // ※Draw()は更新が追い付かない場合，飛ばされる事がある
    virtual bool IThread ::Draw();          //   ↓ CSDLCore::SetDrawSkipFrame() 参照
    }
    virtual bool IThread ::Exit();          // AddThread()で登録したIThread::Exit()は，このタイミングで呼ばれる
    virtual bool CSDLCore::Exit();          // メインループから抜けた後に呼ばれる
    virtual bool CSDLCore::ExitGL();        // （OpenGL の終了処理）
    virtual bool CSDLCore::ExitSDL();       // （SDL の終了処理）
    virtual bool CSDLCore::ExitInstance();  // 一番最後に呼ばれる

【OpenGL/SDL の設定について】
    CSDLCore::InitInstance()内で，メンバ変数 CSDLCore::m_SDLInfo / CSDLCore::m_GLInfo を設定する．
    その後に CSDLCore::SDLInit() / CSDLCore::GLInit() が呼び出されて設定が行われる．
    SDLInit() / GLInit() で設定されないパラメータについては，Init() 内で設定を行う．
    SDL, GL の関数は，設定終了後に呼び出される CSDLCore::Init() 内から有効．InitInstance() 内では呼び出さない．
    CSDLCore::SDLInit() / CSDLCore::GLInit() は仮想関数のため，
    オーバーライドしてそれらの初期化を自分で行う事も可能．

    m_GLInfo.m_GLSetting.bUseDepthBuffer と，m_GLInfo.m_iDepthBufferSize は，
    両方とも false/0 の場合に限り，DepthBuffer が無効となる（Stencil Buffer も同様）．
    どちらか一方でも設定されている場合，SDLInit() / GLInit() 内で，DepthBuffer は有効と設定される．
    （bUseDepthBuffer = true，m_iDepthBufferSize = 0 の場合，m_iDepthBufferSize = 16 に設定される）

【Reflesh Rate 関係について】
    static CSDLCore::SetFPS();              // Reflesh Rate を設定する．
    static CSDLCore::GetFPS();              // 現在処理されている 平均 Reflesh Rate を返す．
    static CSDLCore::SlowAssert();          // Reflesh Rate に処理が追いついていない場合，trueを返す．

    static CSDLCore::MeasureInterval()
        前に MeasureInterval() が呼び出された時点からの経過時間を返す．
        最初の一回は 0/1 を返す．

    static CSDLCore::SetFPSAveragingFrame()
        GetFPS() で返される Reflesh Rate を計算する際に，平均するFrame数を設定する．初期値は 30 ．
    
    static CSDLCore::SetDrawSkipFrame()
        処理が間に合わなかった場合，間に合うまで Run() のみ実行し，Draw() を飛ばす．
        最大で SetDrawSkipFrame() で設定したフレーム数まで，Draw() が飛ばされる
        0 に設定すると，処理が間に合わなくても必ず Draw() が呼ばれる．初期値は 0 ．

【キーボード関係について】
    // KeyConfig 設定
    static inline SDLKey SetAssignKey(SDLKey enNewKey, int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
        enNewKey で登録するキー，iCtrlIdx でコントローラ番号，enAssign で割り当てる方向（ボタン）の設定
    static inline int SetAssignJSButton(int iJSButton, int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
        JoyStickボタンの割り当ての変更．enAssign に iJSButton を割り当てる．

    static inline bool LoadKeyConfigFile(char* strFile)   // KeyConfig 設定 読込み
    static inline bool SaveKeyConfigFile(char* strFile)   // KeyConfig 設定 書込み

    static inline bool IsPressed(SDLKey enSDLKeyCode)                          // 指定されたキーが押されているか(SDLKeyをダイレクトに指定)
    static inline bool IsPressed(int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)    // 指定されたキーが押されているか(KeyConfig/Joystick参照)
    static inline bool IsHitted(int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)     // 指定されたキーが押されたか(KeyConfig/Joystick参照)
