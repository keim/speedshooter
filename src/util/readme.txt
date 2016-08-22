
�y�g�����z
    1. CSDLCore
      1.0. CSLDCore �̌p���N���X���쐬���C�O���[�o���ϐ��Ƃ��ăC���X�^���X�����`����D 
           ���� CSLDCore �̌p���N���X�C���X�^���X�̉��z�֐��́Cmain() ����K�X�Ăяo�����D 
      1.1. CSDLCore �p���N���X�ł́C 
           virtual bool CSDLCore::InitInstance(int argc, char *argv[]) or CSDLCore::InitInstance()
           virtual bool CSDLCore::ExitInstance()
           virtual bool CSDLCore::Init()
           virtual bool CSDLCore::Exit()
           virtual bool CSDLCore::Run()
           virtual bool CSDLCore::Draw()
           virtual bool CSDLCore::InitScene()
           virtual bool CSDLCore::ExitScene()
           virtual bool CSDLCore::EventHandler()
           ��8�̉��z�֐�����������D
      1.2. InitInstance() �́C�A�v���N������ɌĂяo�����D 
    2. IThread �p���N���X�ł́C 
       virtual bool IThread::Init()
       virtual bool IThread::Run()
       virtual bool IThread::Draw()
       virtual bool IThread::Exit()
       virtual bool IThread::InitScene()
       virtual bool IThread::ExitScene()
       ��K�X��������D
    3. static CSDLCore::AddThread() �ŁCIThread �p���N���X�̃C���X�^���X��o�^����D
    3. Init()�� MainLoop() �ɓ��钼�O�CExit()�� MainLoop() ����o������ɌĂ΂��D 
    4. Run() / Draw() �́CCSDLCore::SetFPS() �Őݒ肵�� Reflesh Rate ���� MainLoop() ����Ă΂��D
    5. InitScene() / ExitScene() �́CCSDLCore::ChangeScene() ���Ă΂ꂽ�t���[���̎��t���[���̍ŏ��ɌĂ΂��D
    4. �R�}���h���C���������Q�Ƃ���ꍇ�́CInitInstance(int argc, char *argv[]) ����������D
    6. #define _USE_OPENGL  �� OpenGL �Ή��D
    7. #define _USE_MIXER   �� SDL_mixer �Ή��D
    8. #define _USE_NET     �� SDL_net �Ή��D
    9. CSDLCore::ExitMainLoop()�ŁCMainLoop �������E�o�D
   10. CSDLCore::GetErrorCode()�ŁCCSDLCore::ExitMainLoop()�Ŏw�肵���������擾�ł���D
   11. CSDLCore::RunAllThread()�ŁCCSDLCore::AddThread()�o�^����Thread �̒�~/�ĊJ���ł���D

�yIThread �ɂ��āz
    0. IThread �p���N���X�� static CSDLCore::AddThread() �œo�^����ƁCMainLoop()������Ăяo�����悤�ɂȂ�D
    1. Thread �̓o�^�͒ʏ�CCSDLCore::InitInstance() / Init() ���ōs���D
    2. �o�^���C���� iPrior ���������ݒ肵���X���b�h����Ɏ��s����C
       ���l�Ȃ�CAddThread()�������Ɏ��s�����DExit() �֐��͌Ăяo�������t�D
    3. IThread::Alive()   �� On/Off �ݒ�DOff �̏ꍇ�CIThread::Run() / Draw() ���Ăяo����Ȃ��Ȃ�D
    4. IThread::IsAlive() �� On/Off �Q�ƁD

�yCSDLCore/IThread �̃R�[���o�b�N�֐����Ăяo����鏇�ԁz
    virtual bool CSDLCore::InitInstance();  // ��ԍŏ��ɌĂ΂��
    virtual bool CSDLCore::InitSDL();       // �iSDL �̏����������j
    virtual bool CSDLCore::InitGL();        // �iOpenGL �̏����������j
    virtual bool CSDLCore::Init();          // ���C�����[�v�ɓ���O�ɌĂ΂��
    virtual bool IThread ::Init();          // AddThread()�œo�^����IThread::Init()�́C���̃^�C�~���O�ŌĂ΂��
    MainLoop() {                            // �im_SDLInfo.m_iFramePerSecond �Őݒ肵���X�V�Ԋu�Ŗ���Ă΂��j
    virtual bool CSDLCore::EventHandler();  // �i�C�x���g�������ɌĂ΂��j
    virtual bool CSDLCore::Run();           // 
    virtual bool IThread ::Run();           // 
    virtual bool CSDLCore::Draw();          // ��Draw()�͍X�V���ǂ��t���Ȃ��ꍇ�C��΂���鎖������
    virtual bool IThread ::Draw();          //   �� CSDLCore::SetDrawSkipFrame() �Q��
    }
    virtual bool IThread ::Exit();          // AddThread()�œo�^����IThread::Exit()�́C���̃^�C�~���O�ŌĂ΂��
    virtual bool CSDLCore::Exit();          // ���C�����[�v���甲������ɌĂ΂��
    virtual bool CSDLCore::ExitGL();        // �iOpenGL �̏I�������j
    virtual bool CSDLCore::ExitSDL();       // �iSDL �̏I�������j
    virtual bool CSDLCore::ExitInstance();  // ��ԍŌ�ɌĂ΂��

�yOpenGL/SDL �̐ݒ�ɂ��āz
    CSDLCore::InitInstance()���ŁC�����o�ϐ� CSDLCore::m_SDLInfo / CSDLCore::m_GLInfo ��ݒ肷��D
    ���̌�� CSDLCore::SDLInit() / CSDLCore::GLInit() ���Ăяo����Đݒ肪�s����D
    SDLInit() / GLInit() �Őݒ肳��Ȃ��p�����[�^�ɂ��ẮCInit() ���Őݒ���s���D
    SDL, GL �̊֐��́C�ݒ�I����ɌĂяo����� CSDLCore::Init() ������L���DInitInstance() ���ł͌Ăяo���Ȃ��D
    CSDLCore::SDLInit() / CSDLCore::GLInit() �͉��z�֐��̂��߁C
    �I�[�o�[���C�h���Ă����̏������������ōs�������\�D

    m_GLInfo.m_GLSetting.bUseDepthBuffer �ƁCm_GLInfo.m_iDepthBufferSize �́C
    �����Ƃ� false/0 �̏ꍇ�Ɍ���CDepthBuffer �������ƂȂ�iStencil Buffer �����l�j�D
    �ǂ��炩����ł��ݒ肳��Ă���ꍇ�CSDLInit() / GLInit() ���ŁCDepthBuffer �͗L���Ɛݒ肳���D
    �ibUseDepthBuffer = true�Cm_iDepthBufferSize = 0 �̏ꍇ�Cm_iDepthBufferSize = 16 �ɐݒ肳���j

�yReflesh Rate �֌W�ɂ��āz
    static CSDLCore::SetFPS();              // Reflesh Rate ��ݒ肷��D
    static CSDLCore::GetFPS();              // ���ݏ�������Ă��� ���� Reflesh Rate ��Ԃ��D
    static CSDLCore::SlowAssert();          // Reflesh Rate �ɏ������ǂ����Ă��Ȃ��ꍇ�Ctrue��Ԃ��D

    static CSDLCore::MeasureInterval()
        �O�� MeasureInterval() ���Ăяo���ꂽ���_����̌o�ߎ��Ԃ�Ԃ��D
        �ŏ��̈��� 0/1 ��Ԃ��D

    static CSDLCore::SetFPSAveragingFrame()
        GetFPS() �ŕԂ���� Reflesh Rate ���v�Z����ۂɁC���ς���Frame����ݒ肷��D�����l�� 30 �D
    
    static CSDLCore::SetDrawSkipFrame()
        �������Ԃɍ���Ȃ������ꍇ�C�Ԃɍ����܂� Run() �̂ݎ��s���CDraw() ���΂��D
        �ő�� SetDrawSkipFrame() �Őݒ肵���t���[�����܂ŁCDraw() ����΂����
        0 �ɐݒ肷��ƁC�������Ԃɍ���Ȃ��Ă��K�� Draw() ���Ă΂��D�����l�� 0 �D

�y�L�[�{�[�h�֌W�ɂ��āz
    // KeyConfig �ݒ�
    static inline SDLKey SetAssignKey(SDLKey enNewKey, int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
        enNewKey �œo�^����L�[�CiCtrlIdx �ŃR���g���[���ԍ��CenAssign �Ŋ��蓖�Ă�����i�{�^���j�̐ݒ�
    static inline int SetAssignJSButton(int iJSButton, int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
        JoyStick�{�^���̊��蓖�Ă̕ύX�DenAssign �� iJSButton �����蓖�Ă�D

    static inline bool LoadKeyConfigFile(char* strFile)   // KeyConfig �ݒ� �Ǎ���
    static inline bool SaveKeyConfigFile(char* strFile)   // KeyConfig �ݒ� ������

    static inline bool IsPressed(SDLKey enSDLKeyCode)                          // �w�肳�ꂽ�L�[��������Ă��邩(SDLKey���_�C���N�g�Ɏw��)
    static inline bool IsPressed(int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)    // �w�肳�ꂽ�L�[��������Ă��邩(KeyConfig/Joystick�Q��)
    static inline bool IsHitted(int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)     // �w�肳�ꂽ�L�[�������ꂽ��(KeyConfig/Joystick�Q��)
