// ��ʓ��̍ő�I�u�W�F�N�g�� 
#define FLOATING_CARD_MAX       64
#define PARTICLE_MAX            256
#define CHAIN_TEXT_MAX          64

#define FLOATING_CARD_FILTER    (FLOATING_CARD_MAX-1)
#define PARTICLE_FILTER         (PARTICLE_MAX-1)
#define CHAIN_TEXT_FILTER       (CHAIN_TEXT_MAX-1)

// �C���f�b�N�X���琔���ƋL���𓾂� 
#define GET_CARD_NUMBER(n)  ((n)>>2)
#define GET_CARD_SYMBOL(n)  ((n)&3)

// �J�����p�x�ʒu 
#define CAMERA_ANGLE        50
#define CAMERA_POSITION     40 

#include "Card.h"

//--------------------------------------------------------------------------------
// �J�[�h�Ǘ��N���X 
//--------------------------------------------------------------------------------
class CCardManager : public IManager
{
//--------------------------------------------------
// �����o�񋓎q 
//--------------------------------------------------
// �J�[�h��ԗ񋓎q
//------------------------------
private:
    enum _CARD_STATUS {
        CS_VISIBLE,     // �����Ă�
        CS_FLOATING,    // �ړ���
        CS_DISAPPEAR    // ������\��
    };



//--------------------------------------------------
// �����o�N���X 
//--------------------------------------------------
// �ړ����J�[�h
//------------------------------
private:
    class CFloatingCard
    {
    public:
        CCard*       pCard;         // �ړ����̃J�[�h
        CVector      vcPosition;    // ���݂̍��W
        CVector      vcTarget;      // �ړI�̍��W
        int          iDestIndex;    // �ړI�̃t�B�[���h���̏ꏊ
        int          iLife;         // �ړ����Ă��鎞��
        _CARD_STATUS enStatus;      // �ړI�ʒu���B��ɕύX����J�[�h���

    public:
        CFloatingCard()
        {
            pCard = NULL;
        }

    	// �ݒ�D�o���ʒu, �ړI�ʒu�C�X�N���[���ʒu, ��΂��J�[�h, ���B��̃J�[�h���
        void Set(int srcx, int srcy, int dstx, int dsty, float bot, CCard* ptr, _CARD_STATUS en)
        {
            if (srcy > 7) vcPosition.SetPosition((float)(srcx*100-150), -240, (float)(8-(srcy-8)*40));
            else          vcPosition.SetPosition((float)(srcx*60-120), (float)(624-(srcy+bot)*96), 8);
            if (dsty > 7) vcTarget.SetPosition((float)(dstx*100-150), -240, (float)(2-(dsty-8)*34));
            else          vcTarget.SetPosition((float)(dstx*60-120), (float)(624-(dsty+bot)*96), 2);
            pCard    = ptr;
            enStatus = en;
            iLife    = _CARD_MOVE_SPEED;
            iDestIndex = dstx + dsty*5 - (dsty == 9);
        }

    	// �ړ��Dreturn true �ŖړI�ʒu�ɓ��B
        bool Move()
        {
            if (!IsAlive()) return false;
            
            vcPosition += (vcTarget - vcPosition) * 0.15f;
            if (--iLife == 0) {
                pCard = NULL;
                return true;
            }
            return false;
        }

    	// ����
        bool IsAlive()
        {
            return (pCard != NULL);
        }
    };



// �p�[�e�B�N��
//------------------------------
private:
    class CParticle
    {
    private:
        CVector  v0, v1, v2;    // �p�[�e�B�N���̂R���_
    	CVector  pos, vel;      // �ʒu/����
        CMatrix  rotmat;        // ��]�}�g���b�N�X
        float    fLife;         // ����
        float    fDLife;        // ���������鑁��

    public:
        CParticle()
        {
            fLife = 0;
        }
        
        // �ݒ�Dxy���W�Cxy���x�C�O�p�`�̕��� 
        void Set(float x, float y, float vx, float vy, bool binv)
        {
            v0.SetPosition(-7.5f, -12.0f, 0);
            v2.SetPosition(+7.5f, +12.0f, 0);
            if (binv) v1.SetPosition(-7.5f, +12.0f, 0);
            else      v1.SetPosition(+7.5f, -12.0f, 0);

            pos.SetPosition(x, y, 0);
            vel.SetDirection(vx, vy, randMTf()-0.5f);

            CVector rotaxis((float)(randMT()&255+1), (float)(randMT()&255), (float)(randMT()&255));
            rotmat.LoadRotate(rotaxis, randMTf()*0.04f-0.02f);
            fDLife = 0.03f;
            fLife = 1.0f;
        }

        // �ړ� 
        void Move()
        {
            if (!IsAlive()) return;

            pos += vel;
            rotmat.Transform(v0, v0);
            rotmat.Transform(v1, v1);
            rotmat.Transform(v2, v2);
            fLife -= fDLife;
        }

        // �`��
        void Draw()
        {
            if (!IsAlive()) return;

            glPushMatrix();
                glTranslatef(pos.x, pos.y, pos.z);
                glBegin(GL_TRIANGLES);
                    glColor4f(1.0f, 1.0f, 1.0f, fLife);
                    glVertex3fv(v0);
                    glVertex3fv(v1);
                    glVertex3fv(v2);
                glEnd();
            glPopMatrix();
        }

    	// ����
        bool IsAlive()
        {
            return (fLife > 0.0f);
        }
    };



// ����
//------------------------------
    class CChainText
    {
    private:
        CVector     pos;
        float       fLife;
        char        str[4];

    public:
        CChainText()
        {
            fLife = 0.0f;
        }

        // �ݒ�Dxy���W 
        void Set(float x, float y)
        {
            pos.SetPosition(x-40, y, 0);
            fLife = 2.0f;
            sprintf(str, "%3d", SCORE.GetChain());
        }

        // �ړ�
        void Move()
        {
            if (!IsAlive()) return;

            pos.z += 0.6f;
            fLife -= 0.03333333f;
        }

        // �`��
        void Draw()
        {
            if (!IsAlive()) return;

            int     idx, i;
            float   x;

            glPushMatrix();

                glTranslatef(pos.x, pos.y, pos.z);
                glRotatef(CAMERA_ANGLE, 1, 0, 0);
                glColor4f(1, 1, 1, 1.0f-fabsf(fLife-1.0f));

                // Number
                x = -40;
                for (i=0; str[i]!='\0'; i++) {
                    idx = str[i] - '0';
                    if (idx < 0) continue;
                    RESOURCE.m_DrawingSprite.SetTexRect(0.1875f+idx*0.0625f, 0.9375f, 0.0625f, 0.0625f);
                    RESOURCE.m_DrawingSprite.SetRect(x, -16, 32, 32);
                    RESOURCE.m_DrawingSprite.Draw();
                    x += 21;
                }

                // Chain
                RESOURCE.m_DrawingSprite.SetTexRect(0.1875f, 0.6875f, 0.15625f, 0.0625f);
                RESOURCE.m_DrawingSprite.SetRect(x, -16, 80, 32);
                RESOURCE.m_DrawingSprite.Draw();

            glPopMatrix();
        }

    	// ����
        bool IsAlive()
        {
            return (fLife > 0.0f);
        }
    };



//--------------------------------------------------
// �����o�ϐ�
//--------------------------------------------------
private:
    // �R�D 
    CCardSuit           m_CardSuit;

    // �t�B�[���h�J�[�h/��� 
    CCard*              m_apField[40];
    _CARD_STATUS        m_enStatus[48];

    // ��D
    CCard*              m_apUserField[4];
    CCard*              m_apUserNext[4];

    // �J�[�\�����W 
    int                 m_iCursorX;
    int                 m_iCursorY;

    // �}�E�X�J�[�\�����W 
    float               m_fMouseCursorX;
    float               m_fMouseCursorY;

    // �X�N���[���ʒu�̏����_��
    float               m_fBottom;

    // �X�N���[�����邳 
    float               m_fScreenFlash;
    
    // �J�[�\���̐F 
    CVector             m_vcCursorColor;

    // �e�C���X�^���X 
    CFloatingCard       m_FloatingCard[FLOATING_CARD_MAX];
    CParticle           m_Particle[PARTICLE_MAX];
    CChainText          m_ChainText[CHAIN_TEXT_MAX];



//--------------------------------------------------
// �����o�֐� 
//--------------------------------------------------
// �R���X�g���N�^
//------------------------------
public:
    CCardManager()
    {
    }


// ��C���X�^���X�̎擾
//------------------------------
private:
    // ���V�J�[�h
    CFloatingCard* GetFloatCard()
    {
        static int start_idx = 0;

        // �󂫃C���X�^���X�̌��� 
        int i, idx;
        for (i=0; i<FLOATING_CARD_MAX; i++) {
            idx = (start_idx+i) & FLOATING_CARD_FILTER;
            if (!m_FloatingCard[idx].IsAlive()) {
                start_idx = (idx+1) & FLOATING_CARD_FILTER;
                return &m_FloatingCard[idx];
            }
        }
        
        // ���V�J�[�h������Ȃ��Ȃ�����C�ł��Â����V�J�[�h�������I�ɐݒu���āC
        PutCard(&m_FloatingCard[start_idx]);
        // ���̃J�[�h��Ԃ��D 
        return &m_FloatingCard[start_idx];
    }

    // �p�[�e�B�N�� 
    CParticle* GetParticle()
    {
        static int idx = 0;
        idx = (idx+1) & PARTICLE_FILTER;
        return &m_Particle[idx];
    }

    // �e�L�X�g
    CChainText* GetChainText()
    {
        static int idx = 0;
        idx = (idx+1) & CHAIN_TEXT_FILTER;
        return &m_ChainText[idx];
    }


// ����
//------------------------------
public:
    // �t�B�[���h�̑S������ 
    void Reset()
    {
        int i;
        for (i=0; i<40; i++) m_apField[i]  = NULL;
        for (i=0; i<48; i++) m_enStatus[i] = CS_VISIBLE;
        for (i=0; i<4; i++) {
            m_apUserField[i] = NULL;
        }
        for (i=0; i<4;  i++) {
            m_apUserNext[i]  = NULL;
        }
        m_iCursorX = 2;
        m_iCursorY = 0;
        m_fMouseCursorX = 2;
        m_fMouseCursorY = 0;
        m_fBottom  = 0.0f;
        m_fScreenFlash = 0.0f;
        m_vcCursorColor.Set(0.2f, 0.5f, 1.0f, 1.0f);
        
        m_CardSuit.Reset();
    }

    // ��D��������Ԃɐݒ肷�� 
    void SetStart()
    {
        int i;
        for (i=0; i<20; i++) {
            m_apField[i] = NULL;
        }
        for (i=0; i<4;  i++) {
            m_apUserField[i] = m_CardSuit.Draw();
        }
        for (i=0; i<4;  i++) {
            m_apUserNext[i]  = m_CardSuit.Draw();
        }
    }

    // �X�N���[�����x�擾 
    float GetScreenFlash()
    {
        return m_fScreenFlash;
    }



// �}�E�X����
//------------------------------
public:
    // �}�E�X�ʒu���X�V�p�R�[���o�b�N(Call from CMain::EventHandler())
    void MousePosition(int x, int y)
    {
        static int cx = CSDLCore::GetScreenWidth()>>1;
        static int cy = CSDLCore::GetScreenHeight()>>1;
        static float dx = 0.8f * 640 / CSDLCore::GetScreenWidth();
        static float dy = 2.0f * 480 / CSDLCore::GetScreenHeight();
        
        m_fMouseCursorX = (float)(x-cx)*dx;
        m_fMouseCursorY = (float)(cy-y)*dy;
        if (m_fMouseCursorX < -150) m_fMouseCursorX = -150;
        else if (m_fMouseCursorX > 150) m_fMouseCursorX = 150;
        if (m_fMouseCursorY < -200) m_fMouseCursorY = -200;
        else if (m_fMouseCursorY > 480) m_fMouseCursorY = 480;
    }



    
// ���� sub-routine
//------------------------------
private:
    // �V�K���C������
    void AddNewLine()
    {
        int i;
        
        // �X�N���[��
        for (i=34; i>=0; i--) {
            m_apField[i+5]  = m_apField[i];
            m_enStatus[i+5] = m_enStatus[i];
        }
        for (i=0; i<FLOATING_CARD_MAX; i++) {
            if (m_FloatingCard[i].iDestIndex < 40) {
                m_FloatingCard[i].iDestIndex += 5;
            }
        }

        // �V�K�J�[�h���C����ݒu 
        for (i=0;  i<5; i++)  {
            m_apField[i]  = m_CardSuit.Draw();
            m_enStatus[i] = CS_VISIBLE;
        }
    }

    // �J�[�h�����i�ˏo�悪�A�Ԃł͖����j
    void SwapCard(int curidx, int usridx)
    {
        CFloatingCard* pFCard;

        // �J�[�\������D�ɔ�΂�
        pFCard = GetFloatCard();
        pFCard->Set(m_iCursorX, m_iCursorY, usridx, 8, m_fBottom, m_apField[curidx], CS_VISIBLE);
        m_enStatus[curidx]    = CS_FLOATING;

        // ��D���J�[�\���ɔ�΂�
        pFCard = GetFloatCard();
        pFCard->Set(usridx, 8, m_iCursorX, m_iCursorY, m_fBottom, m_apUserField[usridx], CS_VISIBLE);
        m_enStatus[40+usridx] = CS_FLOATING;

        // �J�[�h����
        CCard* pSwCard        = m_apField[curidx];
        m_apField[curidx]     = m_apUserField[usridx];
        m_apUserField[usridx] = pSwCard;
    }

    // �J�[�h�����i�ˏo�悪�A�ԁj
    void ShootCard(int curidx, int usridx)
    {
        CFloatingCard* pFCard;
        CCard*         pNext;

        // ���X��D������
        pNext = m_CardSuit.Draw();
        // ��΂����J�[�h���R�D�ɖ߂�
        m_CardSuit.Drop(m_apUserField[usridx]);
        // ��΂����J�[�h���L�^����
        SCORE.ShootCard(m_apUserField[usridx]->GetCardNumber(), 
                        m_apUserField[usridx]->GetCardSymbol());

        // ��D���J�[�\���ɔ�΂�
        pFCard = GetFloatCard();
        pFCard->Set(usridx, 8, m_iCursorX, m_iCursorY, m_fBottom, m_apUserField[usridx], CS_DISAPPEAR);

        // ���X��D������D�ɔ�΂�
        pFCard = GetFloatCard();
        pFCard->Set(usridx, 10, usridx, 9, m_fBottom, pNext, CS_VISIBLE);
        m_enStatus[44+usridx] = CS_FLOATING;

        // ����D����D�ɔ�΂�
        pFCard = GetFloatCard();
        pFCard->Set(usridx, 9, usridx, 8, m_fBottom, m_apUserNext[usridx], CS_VISIBLE);
        m_enStatus[40+usridx] = CS_FLOATING;

        // ��D�V�t�g
        m_apUserField[usridx] = m_apUserNext[usridx];
        m_apUserNext[usridx] = pNext;

        // �A������J�[�h�𒲂ׂ�
        MarkChainCards(curidx);
    }

    // �J�[�h�ݒu�i�ˏo��ɉ��������ꍇ�j 
    void SetCard(int curidx, int usridx)
    {
        CFloatingCard* pFCard;
        CCard*         pNext;
        
        // ���X��D������ 
        pNext = m_CardSuit.Draw();

        // ��D���J�[�\���ɔ�΂�
        pFCard = GetFloatCard();
        pFCard->Set(usridx, 8, m_iCursorX, m_iCursorY, m_fBottom, m_apUserField[usridx], CS_VISIBLE);
        m_enStatus[curidx] = CS_FLOATING;

        // ���X��D������D�ɔ�΂�
        pFCard = GetFloatCard();
        pFCard->Set(usridx, 10, usridx, 9, m_fBottom, pNext, CS_VISIBLE);
        m_enStatus[44+usridx] = CS_FLOATING;

        // ����D����D�ɔ�΂�
        pFCard = GetFloatCard();
        pFCard->Set(usridx, 9, usridx, 8, m_fBottom, m_apUserNext[usridx], CS_VISIBLE);
        m_enStatus[40+usridx] = CS_FLOATING;

        // ��D�V�t�g
        m_apField[curidx] = m_apUserField[usridx];
        m_apUserField[usridx] = m_apUserNext[usridx];
        m_apUserNext[usridx] = pNext;
    }

    // �J�[�h�j��G�t�F�N�g 
    void BreakEffect(int idx)
    {
        float       i, j;
        CParticle*  prt;
        float cx = 0.0f;
        float cy = 0.0f;

        // �J�[�h�Z���^�[���W�̌v�Z 
        CalcCardCenterPosition(idx, &cx, &cy);
        
        // �p�[�e�B�N���̐��� 
        for (i=-22.5f; i<30.0f; i+=15.0f) {
            for (j=-36.0f; j<50.0f; j+=24.0f) {
                prt = GetParticle();
                prt->Set(cx+i, cy+j, i*0.03f, j*0.03f, true);
                prt = GetParticle();
                prt->Set(cx+i, cy+j, i*0.03f, j*0.03f, false);
            }
        }

        // �w�i�̃t���b�V�� 
        m_fScreenFlash = 1.0f;

        // �A�����\���̐��� 
        CChainText* pText = GetChainText();
        pText->Set(cx, cy);

        // �J�[�h�j��
        RESOURCE.Sound(SND_CRASH);
    }

    // ���V�J�[�h���ړI�ʒu�ɓ��B������Ăяo�����
    void PutCard(CFloatingCard* pCard)
    {
        int idx = pCard->iDestIndex;
        switch (pCard->enStatus) {
        case CS_DISAPPEAR:

            // CS_DISAPPEAR �̏ꍇ
            if (m_apField[idx] != NULL) {
                // �t�B�[���h�̃J�[�h���L�^����
                SCORE.DisappearCard(m_apField[idx]->GetCardNumber(), m_apField[idx]->GetCardSymbol());
                // �t�B�[���h�̃J�[�h��j�� 
                BreakEffect(idx);
                // �A������ 
                ChainCard(idx);
                // �J�[�h���R�D�ɖ߂�
                m_CardSuit.Drop(m_apField[idx]);
                // �t�B�[���h����ɂ���
                m_apField[idx] = NULL;
                m_enStatus[idx] = CS_VISIBLE;
            }
            
            break;
            
        default:
            // �ʏ�C�ړI�ʒu�ɓ��B������X�e�[�^�X���������� 
            m_enStatus[idx] = pCard->enStatus;
            break;
        }
    }

    // �A������ 
    void ChainCard(int idx)
    {
        BYTE eval = ChainEvaluation(idx);
        if (eval & 1) ChainEffect(idx, idx+5);
        if (eval & 2) ChainEffect(idx, idx-5);
        if (eval & 4) ChainEffect(idx, idx-1);
        if (eval & 8) ChainEffect(idx, idx+1);
    }

    // �A���G�t�F�N�g 
    void ChainEffect(int srcidx, int dstidx)
    {
        CFloatingCard* pFCard;

        // Move cards
        pFCard = GetFloatCard();
        pFCard->Set((srcidx % 5), (srcidx / 5), (dstidx % 5), (dstidx / 5), 
                    m_fBottom, m_apField[srcidx], CS_DISAPPEAR);
        m_enStatus[dstidx] = CS_DISAPPEAR;
    }
    
    // �A������\��̃J�[�h��ԐF�iCS_DISAPPEAR�j�ɐݒ� 
    void MarkChainCards(int idx)
    {
        if (m_enStatus[idx] == CS_DISAPPEAR) return;
        m_enStatus[idx] = CS_DISAPPEAR;
        BYTE eval = ChainEvaluation(idx);
        if (eval & 1) MarkChainCards(idx+5);
        if (eval & 2) MarkChainCards(idx-5);
        if (eval & 4) MarkChainCards(idx-1);
        if (eval & 8) MarkChainCards(idx+1);
    }

    // �J�[�h�������W���C���f�b�N�X�l����v�Z����    
    void CalcCardCenterPosition(int idx, float *pcx, float *pcy)
    {
        *pcx = (float)((idx % 5)*60-120);
        *pcy = (float)(624-((idx / 5)+m_fBottom)*96);
    }


// ����
//------------------------------
private:
    // �A������D�Ԓl = (1=down | 2=up | 4=left | 8=right); 
    BYTE ChainEvaluation(int idx)
    {
        BYTE ret = 0;
        int x = idx % 5;

        if (idx<35) ret |= (BYTE)(CardEvaluation(m_apField[idx], m_apField[idx+5]));
        if (idx>4)  ret |= (BYTE)(CardEvaluation(m_apField[idx], m_apField[idx-5])) << 1;
        if (x>0)    ret |= (BYTE)(CardEvaluation(m_apField[idx], m_apField[idx-1])) << 2;
        if (x<4)    ret |= (BYTE)(CardEvaluation(m_apField[idx], m_apField[idx+1])) << 3;

        return ret;
    }

    // �Q�̃J�[�h���A�Ԃ��H
    bool CardEvaluation(CCard* c0, CCard* c1)
    {
        if (c0 == NULL || c1 == NULL) return false;
        int c0n = c0->GetCardNumber();
        int c1n = c1->GetCardNumber();
        return (c1n == ((c0n+1)%13) || c1n == ((c0n+12)%13));
    }

    // �ŉ��i�ɃJ�[�h�����݂��邩�H 
    bool IsCardInNearestColumn()
    {
        return (m_apField[35] != NULL ||
                m_apField[36] != NULL ||
                m_apField[37] != NULL ||
                m_apField[38] != NULL ||
                m_apField[39] != NULL);
    }


// �`�� 
//------------------------------
private:
    // �J�����ݒ� 
    void MultCamera()
    {
        glTranslatef(0, CAMERA_POSITION, 0);
        glRotatef(-CAMERA_ANGLE, 1, 0, 0);
    }
     
    // �J�[�h�`�� 
    void DrawCard(float x, float y, float z, CCard* pCard)
    {
        if (pCard == NULL) return;
        float u, v;

        pCard->GetUV(&u, &v);
        RESOURCE.m_DrawingSprite.SetTexRect(u, v, 0.078125f, 0.125f);
        RESOURCE.m_DrawingSprite.SetRect(x-30, y-48, 60, 96, z);
        RESOURCE.m_DrawingSprite.Draw();
    }

    // �t�B�[���h�`�� 
    void DrawField()
    {
        glColor4f(0.2f, 0.5f, 1.0f, 0.2f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(-150,  672, -10);
            glVertex3f( 150,  672, -10);
            glVertex3f( 150, -192, -10);
            glVertex3f(-150, -192, -10);
        glEnd();
    }

    // �J�[�\���`�� 
    void DrawCursor()
    {
        m_vcCursorColor.a = 0.5f;
        glColor4fv(m_vcCursorColor);
        glBegin(GL_TRIANGLE_FAN);
            glVertex3f((float)(m_iCursorX*60-150), 576-(m_iCursorY+m_fBottom)*96, 5);
            glVertex3f((float)(m_iCursorX*60- 90), 576-(m_iCursorY+m_fBottom)*96, 5);
            glVertex3f((float)(m_iCursorX*60- 90), 672-(m_iCursorY+m_fBottom)*96, 5);
            glVertex3f((float)(m_iCursorX*60-150), 672-(m_iCursorY+m_fBottom)*96, 5);
        glEnd();
        m_vcCursorColor.a = 1.0f;
        glColor4fv(m_vcCursorColor);
        glBegin(GL_LINE_LOOP);
            glVertex3f((float)(m_iCursorX*60-150), 576-(m_iCursorY+m_fBottom)*96, 10);
            glVertex3f((float)(m_iCursorX*60- 90), 576-(m_iCursorY+m_fBottom)*96, 10);
            glVertex3f((float)(m_iCursorX*60- 90), 672-(m_iCursorY+m_fBottom)*96, 10);
            glVertex3f((float)(m_iCursorX*60-150), 672-(m_iCursorY+m_fBottom)*96, 10);
        glEnd();
    }

    // �}�E�X�J�[�\���`�� 
    void DrawMouseCursor()
    {
        float fRotation = RESOURCE.GetCounter() * 8.0f;
        float fPopping  = (RESOURCE.GetCounter() & 31) * 0.0625f - 1.0f;
        fPopping *= fPopping;
        fPopping *= 10;
        
        glPushMatrix();
            glTranslatef(m_fMouseCursorX, m_fMouseCursorY, 30-fPopping);
            glRotatef(fRotation, 0, 0, 1);
            m_vcCursorColor.a = 0.5f;
            glColor4fv(m_vcCursorColor);
            glBegin(GL_TRIANGLE_FAN);
                glVertex3f(  0, 0, -10);
                glVertex3f(-10, 0,  10);
                glVertex3f( 10, 0,  10);
            glEnd();
            m_vcCursorColor.a = 1.0f;
            glColor4fv(m_vcCursorColor);
            glBegin(GL_LINE_LOOP);
                glVertex3f(  0, 0, -10);
                glVertex3f(-10, 0,  10);
                glVertex3f( 10, 0,  10);
            glEnd();
        glPopMatrix();
    }

    // �f�b�h���C���`�� 
    void DrawDeadline()
    {
        if (IsCardInNearestColumn()) glColor4f(1.0f, 0.3f, 0.2f, 1.0f);
        else glColor4f(0.2f, 0.3f, 1.0f, 1.0f);
        glBegin(GL_LINES);
            glVertex3f(-150, -192, 5);
            glVertex3f( 150, -192, 5);
        glEnd();
    }



//--------------------------------------------------
// �R�[���o�b�N
//--------------------------------------------------
// ���ʏ��� 
//------------------------------
public:
    bool Run()
    {
        IManager::Run();
        MoveAllObject();
        return true;
    }
    
    bool Draw()
    {
        glPushMatrix();
            DrawAllObject();
            IManager::Draw();
        glPopMatrix();
        return true;
    }
    
private:
    void MoveAllObject()
    {
        int i;

        // Move floating card
        for (i=0; i<FLOATING_CARD_MAX; i++) {
            if (m_FloatingCard[i].Move()) PutCard(&(m_FloatingCard[i]));
        }

        // Move particle
        for (i=0; i<PARTICLE_MAX; i++) m_Particle[i].Move();

        // Chain Text
        for (i=0; i<CHAIN_TEXT_MAX; i++) m_ChainText[i].Move();

        // Screen Flash
        if (m_fScreenFlash >= 0.0f) m_fScreenFlash -= 0.05f;
        else m_fScreenFlash = 0.0f;
    }
    
    void DrawAllObject()
    {
        int i, j, idx;

        // Camera
        MultCamera(); 

        // Field
        GL::SetAddBlend();
        DrawField();
        
        GL::SetAlphaBlend();
        
        // Field cards
        for (idx=0, j=0; j<8; j++) {
            for (i=0; i<5; i++, idx++) {
                switch(m_enStatus[idx]) {
                case CS_VISIBLE:
                    glColor3f(1, 1, 1);
                    DrawCard((float)(i*60-120), (float)(624-(j+m_fBottom)*96), 0, m_apField[idx]);
                    break;
                case CS_DISAPPEAR:
                    glColor3f(1, 0.5f, 0.5f);
                    DrawCard((float)(i*60-120), (float)(624-(j+m_fBottom)*96), 0, m_apField[idx]);
                    break;
                }
            }
        }

        glColor3f(1, 1, 1);

        // Next cards
        for (i=0; i<4; i++) {
            if (m_enStatus[44+i] == CS_VISIBLE) {
                DrawCard((float)(i*100-150), -240, -32, m_apUserNext[i]);
            }
        }
        // User cards
        for (i=0; i<4; i++) {
            if (m_enStatus[40+i] == CS_VISIBLE) {
                DrawCard((float)(i*100-150), -240, 0,   m_apUserField[i]);
            }
        }
        // Floating cards
        CVector* pvc;
        for (i=0; i<FLOATING_CARD_MAX; i++) {
            pvc = &(m_FloatingCard[i].vcPosition);
            DrawCard(pvc->x, pvc->y, pvc->z, m_FloatingCard[i].pCard);
        }

        // Particles
        for (i=0; i<PARTICLE_MAX; i++) m_Particle[i].Draw();

        // Chain Texts
        for (i=0; i<CHAIN_TEXT_MAX; i++) m_ChainText[i].Draw();

        DrawDeadline();
    }



// SCENE_TITLE
//------------------------------
protected:
    void InitTITLE()
    {
        Reset();
        SetStart();
    }
    
    

// SCENE_START
//------------------------------
protected:    
    void InitSTART()
    {
        ChangeCursorColor();
    }

    void RunSTART()
    {
        StartFeadCard();
    }

    void DrawSTART()
    {
        DrawCursor();
    }

    // �X�^�[�g���̃J�[�h�t�B�[�h 
    void StartFeadCard()
    {
        // 80Frame �̊ԁC0.05line �Â����X�N���[�� 
        if (RESOURCE.GetCounter() <= 80) {
            m_fBottom += 0.05f;
            if (m_fBottom >= 1.0f) {
                AddNewLine();
                m_fBottom -= 1.0f;
                m_iCursorY += (m_iCursorY<7);
            }
        }
    }

    // ���[�h�ɂ���ăJ�[�\���̐F��ύX 
    void ChangeCursorColor()
    {
        if (RESOURCE.IsMouseMode()) m_vcCursorColor.Set(1.0f, 0.2f, 0.2f);
        else                        m_vcCursorColor.Set(0.2f, 0.5f, 1.0f);
    }


// SCENE_MAIN
//------------------------------
protected:
    void RunMAIN()
    {
        if (RESOURCE.IsMouseMode()) { 
            // Through card
            if (SCORE.IsLButtonDClicked()) ThroughCard(0);
            if (SCORE.IsLButtonClicked())  ThroughCard(1);
            if (SCORE.IsRButtonClicked())  ThroughCard(2);
            if (SCORE.IsRButtonDClicked()) ThroughCard(3);
            // Move cursor
            if (!SCORE.IsClicking()) {
                m_iCursorX = (int)((m_fMouseCursorX/60)+2.5f);
                m_iCursorY = (int)(7.0f-(m_fMouseCursorY/96)-m_fBottom);
                if (m_iCursorX < 0) m_iCursorX = 0;
                else if (m_iCursorX > 4) m_iCursorX = 4;
                if (m_iCursorY < 0) m_iCursorY = 0;
                else if (m_iCursorY > 7) m_iCursorY = 7;
            }
        } else {
            // Move cursor
            m_iCursorX += ((m_iCursorX<4) & CSDLCore::IsHitted(0, SKC_XUP)) - 
                          ((m_iCursorX>0) & CSDLCore::IsHitted(0, SKC_XDOWN));
            m_iCursorY += ((m_iCursorY<7) & CSDLCore::IsHitted(0, SKC_YUP)) - 
                          ((m_iCursorY>1) & CSDLCore::IsHitted(0, SKC_YDOWN));
            // Through card
            for (int i=0; i<4; i++) {
                if (CSDLCore::IsHitted(0, (_SDLKEYCONF_ASSIGN)(SKC_BTN0+i))) ThroughCard(i);
            }
        }

        ScrollField(); 
    }

    void DrawMAIN()
    {
        DrawCursor();
        if (RESOURCE.IsMouseMode()) DrawMouseCursor();
    }

    // �t�B�[���h�̃X�N���[�� 
    void ScrollField()
    {
        // �X�N���[���X�s�[�h 
        float fSpeed  = SCORE.GetScrollSpeed();

        // ����
        if (CSDLCore::IsPressed(0, SKC_BTN4) || 
            SCORE.IsPressingMButton())        fSpeed = 0.03f;
        // �A������~
        else if (SCORE.IsScrollStoped())      fSpeed = 0.0f;
        // �ŉ��i���B�Ō��� 
        else if (IsCardInNearestColumn())     fSpeed *= 0.5f;

        // �X�N���[��
        m_fBottom += fSpeed;
        
        if (m_fBottom >= 1.0f) {
            if (IsCardInNearestColumn()) {
                // �f�b�h���C�����B
                CSDLCore::ChangeScene(SCENE_OVER);
            } else {
                // �V�K���C������ 
                AddNewLine();
                m_fBottom -= 1.0f;
                m_iCursorY += (m_iCursorY<7);
            }
        }        
    }
    
    // ��D�𓊂��� 
    void ThroughCard(int usridx)
    {
        int curidx = m_iCursorX + m_iCursorY*5;
        
        if (m_apField[curidx] != NULL) {
            // �J�[�\���ʒu�ɃJ�[�h���݂� 

            if (m_enStatus[curidx] != CS_DISAPPEAR) {
                // �J�[�\���ʒu�̃J�[�h���A���\��ł͖��� 

                if (CardEvaluation(m_apField[curidx], m_apUserField[usridx])) {
                    // �J�[�\���ʒu�̃J�[�h���A�� �� ShootCard() 
                    ShootCard(curidx, usridx);
                    
                } else {
                    // �J�[�\���ʒu�̃J�[�h���A�Ԃł͖��� �� SwapCard()
                    SwapCard(curidx, usridx);
                }

                RESOURCE.Sound(SND_PUT);
            }
            
        } else {
            // �J�[�\���ʒu�ɃJ�[�h������ �� SetCard()
            SetCard(curidx, usridx);
            RESOURCE.Sound(SND_PUT);
        }
    }


// SCENE_OVER
//------------------------------

};


// �C���X�^���X 
CCardManager CARD;
