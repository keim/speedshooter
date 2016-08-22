//--------------------------------------------------------------------------------
//  CSDLCore Thread Class
//    Copyright  (C)  2006  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// class IThread
//   Thread の基本クラス．
//   仮想関数 Init()/Run()/Draw()/Exit()/InitScene()/ExitScene() をオーバーライドして，
//   SDLCore::AddThread() でポインタを登録すると，
//   MainLoop から呼び出されるようになる．
//--------------------------------------------------------------------------------
class IThread
{
//--------------------------------------------------
//  フレンドクラス
//--------------------------------------------------
friend class CSDLThreadManager;
//--------------------------------------------------
//  メンバ変数
//--------------------------------------------------
private:
    IThread*    m_pNextChainedThread;   // 双方向リスト構造
    IThread*    m_pPrevChainedThread;
    bool        m_bIsAlive;             // スレッドのOn/Off

//--------------------------------------------------
//  コンストラクタ
//--------------------------------------------------
public:
	// コンストラクタ
    IThread()
    {
        m_pNextChainedThread = NULL;
        m_pPrevChainedThread = NULL;
        m_bIsAlive = true;
    }

    // コンストラクタ（スレッドのOn/Off指定）
    IThread(bool bAlive)
    {
        m_pNextChainedThread = NULL;
        m_pPrevChainedThread = NULL;
        m_bIsAlive = bAlive;
    }

//--------------------------------------------------
//  スレッド生死
//--------------------------------------------------
public:
    // スレッドの生死設定
    inline void Alive(bool bAlive)
    {
        m_bIsAlive = bAlive;
    }

    // スレッドの生死参照
    inline bool IsAlive()
    {
        return m_bIsAlive;
    }

//--------------------------------------------------
//  仮想関数
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
//   複数の IThread をリストとして保持し，順次処理を行う．
//   CSDLCore がメンバ変数として実体を持っている．
//--------------------------------------------------------------------------------
class CSDLThreadManager : public IThread
{
//--------------------------------------------------
//  メンバ変数
//--------------------------------------------------
private:
    // スレッドリストの最初と最後
    IThread*    m_pFirstThread;
    IThread*    m_pLastThread;

	// シーン管理
    int         m_iSceneIndex;          // 現在のシーン番号
    int         m_iNextSceneIndex;      // 次フレームで切り替わるシーン番号
    bool        m_bSceneChanged;        // 次フレームでシーンが切り替わるか？

//--------------------------------------------------
//  コンストラクタ
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
//  スレッド操作
//--------------------------------------------------
public:
    // 実行リストにスレッドを追加
    void AddThread(IThread* pThread)
    {
        if (m_pFirstThread == NULL) {
            m_pLastThread = m_pFirstThread = pThread;
        } else {
            InsertThreadAfter(m_pLastThread, pThread);
        }
    }

    // ptrInsertAfter の後に newptr を追加，ptrInsertAfter == NULL なら先頭に追加
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
//  スレッド参照
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
//  シーン操作
//--------------------------------------------------
public:
    // CSDLCore::ResetScene() から呼ばれる
    inline void ResetScene(int iSceneIndex)
    {
        m_iSceneIndex     = iSceneIndex;
        m_iNextSceneIndex = iSceneIndex;
        m_bSceneChanged   = false;
    }

    // CSDLCore::GetScene() から呼ばれる
    inline int GetScene()
    {
        return m_iSceneIndex;
    }

    // CSDLCore::ChangeScene() から呼ばれる
    inline void ChangeScene(int iNextSceneIndex)
    {
        m_iNextSceneIndex = iNextSceneIndex;
        m_bSceneChanged = true;
    }

    // MainLoop() から，IThread::Run()の直前に呼ばれる
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
//  スレッド実行
//--------------------------------------------------
public:
    // 全子スレッドのInit()実行
    bool Init()
    {
        for (IThread *ptr = GetFirstThread(); ptr != NULL; ptr = GetNextThread(ptr)) {
            if (!ptr->Init()) return false;
        }
        return true;
    }

    // 生きている子スレッドのRun()実行
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

    // 生きている子スレッドのDraw()実行
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

    // 全子スレッドのExit()を逆順で実行
    bool Exit()
    {
        for (IThread *ptr = GetLastThread(); ptr != NULL; ptr = GetPrevThread(ptr)) {
            if (!ptr->Exit()) return false;
        }
        return true;
    }

    // 生きている子スレッドのInitScene()実行
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

    // 生きている子スレッドのExitScene()実行
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

