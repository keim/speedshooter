//--------------------------------------------------------------------------------
//  CSDLCore Thread Class
//    Copyright  (C)  2006  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// class IThread
//   Thread �̊�{�N���X�D
//   ���z�֐� Init()/Run()/Draw()/Exit()/InitScene()/ExitScene() ���I�[�o�[���C�h���āC
//   SDLCore::AddThread() �Ń|�C���^��o�^����ƁC
//   MainLoop ����Ăяo�����悤�ɂȂ�D
//--------------------------------------------------------------------------------
class IThread
{
//--------------------------------------------------
//  �t�����h�N���X
//--------------------------------------------------
friend class CSDLThreadManager;
//--------------------------------------------------
//  �����o�ϐ�
//--------------------------------------------------
private:
    IThread*    m_pNextChainedThread;   // �o�������X�g�\��
    IThread*    m_pPrevChainedThread;
    bool        m_bIsAlive;             // �X���b�h��On/Off

//--------------------------------------------------
//  �R���X�g���N�^
//--------------------------------------------------
public:
	// �R���X�g���N�^
    IThread()
    {
        m_pNextChainedThread = NULL;
        m_pPrevChainedThread = NULL;
        m_bIsAlive = true;
    }

    // �R���X�g���N�^�i�X���b�h��On/Off�w��j
    IThread(bool bAlive)
    {
        m_pNextChainedThread = NULL;
        m_pPrevChainedThread = NULL;
        m_bIsAlive = bAlive;
    }

//--------------------------------------------------
//  �X���b�h����
//--------------------------------------------------
public:
    // �X���b�h�̐����ݒ�
    inline void Alive(bool bAlive)
    {
        m_bIsAlive = bAlive;
    }

    // �X���b�h�̐����Q��
    inline bool IsAlive()
    {
        return m_bIsAlive;
    }

//--------------------------------------------------
//  ���z�֐�
//--------------------------------------------------
public:
    virtual bool Init(){return true;}
    virtual bool Run(){return true;}
    virtual bool Draw(){return true;}
    virtual bool Exit(){return true;}
	virtual bool InitScene(){return true;}
	virtual bool ExitScene(){return true;}
};





//--------------------------------------------------------------------------------
// class CSDLThreadManager
//   ������ IThread �����X�g�Ƃ��ĕێ����C�����������s���D
//   CSDLCore �������o�ϐ��Ƃ��Ď��̂������Ă���D
//--------------------------------------------------------------------------------
class CSDLThreadManager : public IThread
{
//--------------------------------------------------
//  �����o�ϐ�
//--------------------------------------------------
private:
    // �X���b�h���X�g�̍ŏ��ƍŌ�
    IThread*    m_pFirstThread;
    IThread*    m_pLastThread;

	// �V�[���Ǘ�
    int         m_iSceneIndex;          // ���݂̃V�[���ԍ�
    int         m_iNextSceneIndex;      // ���t���[���Ő؂�ւ��V�[���ԍ�
    bool        m_bSceneChanged;        // ���t���[���ŃV�[�����؂�ւ�邩�H

//--------------------------------------------------
//  �R���X�g���N�^
//--------------------------------------------------
public:
    CSDLThreadManager()
    {
        m_pFirstThread = NULL;
        m_pLastThread = NULL;
        m_iSceneIndex     = 0;
        m_iNextSceneIndex = 0;
        m_bSceneChanged   = false;
    }

//--------------------------------------------------
//  �X���b�h����
//--------------------------------------------------
public:
    // ���s���X�g�ɃX���b�h��ǉ�
    void AddThread(IThread* pThread)
    {
        if (m_pFirstThread == NULL) {
            m_pLastThread = m_pFirstThread = pThread;
        } else {
            InsertThreadAfter(m_pLastThread, pThread);
        }
    }

    // ptrInsertAfter �̌�� newptr ��ǉ��CptrInsertAfter == NULL �Ȃ�擪�ɒǉ�
    void InsertThreadAfter(IThread* ptrInsertAfter, IThread* newptr)
    {
        if (ptrInsertAfter == NULL) {
            newptr->m_pNextChainedThread = m_pFirstThread;
            newptr->m_pPrevChainedThread = NULL;
            m_pFirstThread = newptr;
        } else {
            newptr->m_pNextChainedThread = ptrInsertAfter->m_pNextChainedThread;
            newptr->m_pPrevChainedThread = ptrInsertAfter;
            if (ptrInsertAfter->m_pNextChainedThread != NULL) {
                ptrInsertAfter->m_pNextChainedThread->m_pPrevChainedThread = newptr;
            } else {
                m_pLastThread = newptr;
            }
            ptrInsertAfter->m_pNextChainedThread = newptr;
        }
    }

//--------------------------------------------------
//  �X���b�h�Q��
//--------------------------------------------------
private:
    IThread* GetFirstThread()
    {
        return m_pFirstThread;
    }

    IThread* GetLastThread()
    {
        return m_pLastThread;
    }

    IThread* GetNextThread(IThread* ptr)
    {
        return ptr->m_pNextChainedThread;
    }

    IThread* GetPrevThread(IThread* ptr)
    {
        return ptr->m_pPrevChainedThread;
    }

//--------------------------------------------------
//  �V�[������
//--------------------------------------------------
public:
    // CSDLCore::ResetScene() ����Ă΂��
    inline void ResetScene(int iSceneIndex)
    {
        m_iSceneIndex     = iSceneIndex;
        m_iNextSceneIndex = iSceneIndex;
        m_bSceneChanged   = false;
    }

    // CSDLCore::GetScene() ����Ă΂��
    inline int GetScene()
    {
        return m_iSceneIndex;
    }

    // CSDLCore::ChangeScene() ����Ă΂��
    inline void ChangeScene(int iNextSceneIndex)
    {
        m_iNextSceneIndex = iNextSceneIndex;
        m_bSceneChanged = true;
    }

    // MainLoop() ����CIThread::Run()�̒��O�ɌĂ΂��
    inline bool RunScene()
    {
        if (m_bSceneChanged) {
            m_bSceneChanged = false;
        	if (!ExitScene()) return false;
            m_iSceneIndex = m_iNextSceneIndex;
            if (!InitScene()) return false;
        }
    	return true;
    }

//--------------------------------------------------
//  �X���b�h���s
//--------------------------------------------------
public:
    // �S�q�X���b�h��Init()���s
    bool Init()
    {
        for (IThread *ptr = GetFirstThread(); ptr != NULL; ptr = GetNextThread(ptr)) {
            if (!ptr->Init()) return false;
        }
        return true;
    }

    // �����Ă���q�X���b�h��Run()���s
    bool Run()
    {
        if (IsAlive()) {
            for (IThread *ptr = GetFirstThread(); ptr != NULL; ptr = GetNextThread(ptr)) {
                if (ptr->IsAlive()) {
                    if (!ptr->Run()) return false;
                }
            }
        }
        return true;
    }

    // �����Ă���q�X���b�h��Draw()���s
    bool Draw()
    {
        if (IsAlive()) {
            for (IThread *ptr = GetFirstThread(); ptr != NULL; ptr = GetNextThread(ptr)) {
                if (ptr->IsAlive()) {
                    if (!ptr->Draw()) return false;
                }
            }
        }
        return true;
    }

    // �S�q�X���b�h��Exit()���t���Ŏ��s
    bool Exit()
    {
        for (IThread *ptr = GetLastThread(); ptr != NULL; ptr = GetPrevThread(ptr)) {
            if (!ptr->Exit()) return false;
        }
        return true;
    }

    // �����Ă���q�X���b�h��InitScene()���s
    bool InitScene()
    {
        if (IsAlive()) {
            for (IThread *ptr = GetFirstThread(); ptr != NULL; ptr = GetNextThread(ptr)) {
                if (ptr->IsAlive()) {
                    if (!ptr->InitScene()) return false;
                }
            }
        }
        return true;
    }

    // �����Ă���q�X���b�h��ExitScene()���s
    bool ExitScene()
    {
        if (IsAlive()) {
            for (IThread *ptr = GetFirstThread(); ptr != NULL; ptr = GetNextThread(ptr)) {
                if (ptr->IsAlive()) {
                    if (!ptr->ExitScene()) return false;
                }
            }
        }
        return true;
    }
};

