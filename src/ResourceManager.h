#define SND_CRASH       0
#define SND_CRASH_FILE  "resource\\crash.wav"
#define SND_PARARA      1
#define SND_PARARA_FILE "resource\\parara.wav"
#define SND_PUT         2
#define SND_PUT_FILE    "resource\\put.wav"
#define SND_PIPIPI      3
#define SND_PIPIPI_FILE "resource\\pipipi.wav"

//--------------------------------------------------------------------------------
// ���\�[�X�Ǘ��N���X 
//--------------------------------------------------------------------------------
class CResourceManager :public IThread
{
//--------------------------------------------------
// �����o�ϐ�
//--------------------------------------------------
private:
    GL::CTexture        m_CardTex;
    GL::CTexture        m_Font;
    CSDLSound           m_Sound[8];
    int                 m_iSceneCounter;
    bool                m_bMouseMode;

public:
    GL::CTextureSprite  m_DrawingSprite;
    GL::CCharTable      m_FontTable;
    
//--------------------------------------------------
// �����o�֐�
//--------------------------------------------------
public:
    // ����炷 
    void Sound(int idx)
    {
        m_Sound[idx].Play(idx, 0);
    }

    // �J�E���^�̃��Z�b�g 
    void ResetCounter()
    {
        m_iSceneCounter = 0;
    }
    
    // �J�E���^�擾 
    int GetCounter()
    {
        return m_iSceneCounter;
    }
    
    // �}�E�X���[�h 
    void SetMouseMode(bool bMouseMode)
    {
        m_bMouseMode = bMouseMode;
    }
    
    bool IsMouseMode()
    {
        return m_bMouseMode;
    }
    
//--------------------------------------------------
// �R�[���o�b�N
//--------------------------------------------------
public:
    bool Init()
    {
        // �e���\�[�X�̓ǂݍ��� 
        if (!m_CardTex.Load("resource\\cards.rgba",  4))  return false;
        m_CardTex.SetParameters(false, true, true, false, false);
        if (!m_CardTex.Register())                        return false;
        if (!m_DrawingSprite.SetTexture(&m_CardTex))      return false;

        if (!m_Font.Load("resource\\font.bmp", 3))        return false;
        m_Font.SetParameters(false, true, true, false, false);
        if (!m_Font.Register())                           return false;
        if (!m_FontTable.SetTexture(&m_Font, 16, 24, 32)) return false;
    
        m_Sound[SND_CRASH]. Load(SND_CRASH_FILE);
        m_Sound[SND_PARARA].Load(SND_PARARA_FILE);
        m_Sound[SND_PUT].   Load(SND_PUT_FILE);
        m_Sound[SND_PIPIPI].Load(SND_PIPIPI_FILE);
    
        // �V�[�������� 
        CSDLCore::ChangeScene(SCENE_TITLE);

        m_bMouseMode = false;

        return true;
    }
    
    bool InitScene()
    {
        ResetCounter();
        return true;
    }
    
    bool Run()
    {
        m_iSceneCounter++;
        return true;
    }
};


// �C���X�^���X 
CResourceManager RESOURCE;
