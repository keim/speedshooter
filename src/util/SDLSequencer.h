//--------------------------------------------------------------------------------
//  SDL Sound Wrapping Class
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
//  BPM ����Wave�Đ��N���X
//
//--------------------------------------------------------------------------------

#include "Math.h"
#include "SDLEffector.h"

#define  _SDLSEQ_CHANNEL_MAX    16



//------------------------------------------------------------
// �V�[�P���T�N���X
//------------------------------------------------------------
class CSDLSequencer
{
//------------------------------------------------------------
// �����o�N���X
//------------------------------------------------------------
public:
    //--------------------------------------------------
    // �`�����l���f�[�^�N���X
    //--------------------------------------------------
    class CSDLSeqChannel {
    //--------------------------------------------------
    // �����o�ϐ�
    //--------------------------------------------------
    public:
        Uint8*  m_pData;        // �Đ��f�[�^
        int     m_iDataLength;  // �Đ��f�[�^�T�C�Y

        int     m_iVolume;      // �Đ�����
        int     m_iLoopCount;   // ���[�v�J�E���^
        int     m_iDelay;       // �Đ������܂ł̑ҋ@�o�C�g��
        int     m_iSilence;     // ���񃋁[�v�Đ��܂ł̖����ҋ@�o�C�g��
        int     m_iEndPoint;    // �ŏI���[�v�ōĐ��I������f�[�^�ʒu
        int     m_iStartPoint;  // ����X�g���[���ɏ����ލĐ��f�[�^�ʒu

        int     m_fiFadeT;      // �t�F�[�h���ʁi������16bit �̌Œ菬���j
        int     m_fiFadeDT;     // �t�F�[�h���ʕω��i������16bit �̌Œ菬���j

        bool    m_bPlaying;     // ������

    //--------------------------------------------------
    // �����o�֐�
    //--------------------------------------------------
    public:
        // ������
        void Init()
        {
            m_pData = NULL;
            m_iDataLength = 0;

            m_iVolume = 0;
            m_iLoopCount = -1;
            m_iDelay = 0;
            m_iSilence = 0;
            m_iEndPoint = 0;
            m_iStartPoint = 0;

            m_fiFadeT = 0;
            m_fiFadeDT = 0;

            m_bPlaying = false;
        }

        // �I������
        void Exit()
        {
            m_iLoopCount = -1;
            m_pData = NULL;
            m_bPlaying = false;
        }

        // �����̏I��
        void Stop(int iStopDelay)
        {
            if (m_pData == NULL) return;
            if (!m_bPlaying) {
                // �ҋ@���Ȃ�I��
                Exit();
            } else {
                // �������Ȃ�Đ��I���ʒu���v�Z
                int iStopPoint = m_iStartPoint + iStopDelay;
                // ���[�v��
                int iLoop = iStopPoint / m_iDataLength;
                // �ŏI���[�v�̍Đ��I���ʒu
                if (m_iLoopCount >= 0 || m_iLoopCount <= -(iLoop + 1)) {
                    m_iLoopCount = -(iLoop + 1);
                    if (iLoop == 0) m_iDataLength = iStopPoint;
                    else            m_iEndPoint   = iStopPoint - iLoop * m_iDataLength;
                }
            }
        }

        // FadeIn
        void FadeIn(int iFadeDelay, int iFadeLength)
        {
            // ������16bit �̌Œ菬�����g�p
            m_fiFadeT  = 0;
            m_fiFadeDT = (128 << 16) / (iFadeLength >> 8);
        }

        // FadeOut
        void FadeOut(int iFadeDelay, int iFadeLength)
        {
            // ������16bit �̌Œ菬�����g�p
            m_fiFadeT  = (128 << 16);
            m_fiFadeDT = - ((128 << 16) / (iFadeLength >> 8));
        }

        // �ҋ@�ireturn true �Ȃ�X�g���[���ɏ������݂�v���j
        bool Wait(int iStreamLength)
        {
            if (m_pData == NULL) return false;
            if (m_iDelay < iStreamLength) {
                m_bPlaying = true;
                return true;
            }
            m_iDelay -= iStreamLength;
            return false;
        }

        // �X�g���[���ɏ�������
        void Streaming(Uint8* stream, int iStreamLength)
        {
            // [������] = [�X�g���[����] - [�ҋ@����]
            int iCopyLength = iStreamLength - m_iDelay;

            // [�f�[�^�擪�A�h���X] + [������] > [�f�[�^��]
            if (m_iStartPoint + iCopyLength > m_iDataLength) {
                // ����̏����݂Ńf�[�^�Ō�܂œ��B����ꍇ
                // [������] = [�f�[�^��] - [�f�[�^�擪�A�h���X]
                iCopyLength = m_iDataLength - m_iStartPoint;
                // �X�g���[���Ƀf�[�^�R�s�[
                AddData((short*)&(stream[m_iDelay]), (short*)&(m_pData[m_iStartPoint]), iCopyLength);

                // ���[�v�J�E���^���C���N�������g
                ++m_iLoopCount;
                switch(m_iLoopCount) {
                case 0:
                    // ���[�v�J�E���^ == 0 �Ȃ�Đ��I��
                    Exit();
                    return;
                case -1:
                    // ���[�v�J�E���^ == 1 �Ȃ玟��Đ��I���|�C���g��ݒ�
                    m_iDataLength = m_iEndPoint;
                    break;
                }

                // [�X�g���[�������ʒu] += [�R�s�[�σo�C�g��] + [�����[�v�܂ł̖����ҋ@�o�C�g��]
                m_iDelay += iCopyLength + m_iSilence;
                // [������] = [�X�g���[����] - [�ҋ@����]
                iCopyLength = iStreamLength - m_iDelay;
                // ����̏����݂ł́C�����[�v�̍ŏ��ɓ��B���Ȃ�
                if (iCopyLength <= 0) return;
                // �����[�v���f�[�^�擪���珑����
                m_iStartPoint = 0;
            }

            // �X�g���[���Ƀf�[�^�R�s�[
            AddData((short*)&(stream[m_iDelay]), (short*)&(m_pData[m_iStartPoint]), iCopyLength);

            // [�f�[�^�擪�A�h���X] �����񏑂����񂾕��i�߂�
            m_iStartPoint += iCopyLength;
            // ����̓X�g���[���̐擪���珑����
            m_iDelay = 0;
        }

    private:
        // �X�g���[���ɉ��Z
        void AddData(short* stream, short* dat, int len)
        {
            int i;
            int vol = m_iVolume;

            // sampling �P�ʂɕϊ�
            len >>= 1;

            if (m_fiFadeDT == 0) {
                // �ʏ���Z
                for (i=0; i<len; i++) stream[i] += (dat[i]*vol)>>7;
            } else {
                // �t�F�[�f�B���O�e�[�u���̍쐬
                char achFadeTable[64];
                int  imax = len >> 8;
                for (int i=0; i<imax; i++, m_fiFadeT+=m_fiFadeDT) achFadeTable[i] = (m_fiFadeT >> 16);
                // �t�F�[�h���Z
                for (i=0; i<len; i++) stream[i] += (dat[i]*vol*achFadeTable[i>>8])>>14;
                // �t�F�[�h�I��
                if (m_fiFadeT < 0) Exit();
                else if (m_fiFadeT > (128 << 16)) m_fiFadeDT = 0;
            }
        }
    };


//------------------------------------------------------------
// �����o�ϐ�
//------------------------------------------------------------
private:
    // �����p�����[�^
    float   m_fBeatPerMinuts;           // �P���Ԃ̃r�[�g��
    float   m_fBytePerBeats;            // �P�r�[�g����̃o�C�g��
    float   m_fBytePerFineBeats;        // m_funcFineCallBack ���Ăяo���^�C�~���O�̃o�C�g��
    float   m_fSecondPerByte;           // �P�o�C�g���ɑ�������b��
    float   m_fLatency;                 // �o�b�t�@�ɂ�郌�C�e���V�[
    DWORD   m_dwPastByte;               // ���������Ă���̌o�߃o�C�g��
    int     m_iBeatCounter;             // ���������Ă���̃r�[�g�J�E���^
    int     m_iFineBeatCounter;         // m_funcFineCallBack ���Ăяo���J�E���^
    int     m_iBeatCallBackInterval;    // �R�[���o�b�N����r�[�g�Ԋu
    bool    m_bPlay;                    // �V�[�P���T�������Ă邩

    // �`�����l��
    CSDLSeqChannel  m_Channel[_SDLSEQ_CHANNEL_MAX][2];  // �`�����l���f�[�^
    int             m_iActiveSub[_SDLSEQ_CHANNEL_MAX];  // �g�p���̃T�u�`�����l���ԍ�

    // �r�[�g�R�[���o�b�N�֐�
    bool    (*m_funcBeatCallBack)(DWORD, float);
    bool    (*m_funcFineCallBack)(DWORD, float);

    // �G�t�F�N�^
    ISDLEffect*     m_pEffector;

//------------------------------------------------------------
// �����o�֐�
//------------------------------------------------------------
public:
    CSDLSequencer()
    {
        // ������
        m_fBeatPerMinuts = 0.0f;
        m_fBytePerBeats = 0.0f;
        m_fBytePerFineBeats = 0.0f;
        m_fSecondPerByte = 0.0f;
        m_fLatency = 0.0f;
        m_dwPastByte = 0;
        m_iBeatCounter = 0;
        m_iFineBeatCounter = 0;
        m_iBeatCallBackInterval = 4;
        m_bPlay = false;

        ResetAllChannel();

        m_funcBeatCallBack = NULL;
        m_funcFineCallBack = NULL;

        m_pEffector = NULL;
    }

    ~CSDLSequencer()
    {
        // �X�g���[���Đ��R�[���o�b�N�֐����͂���
        Mix_HookMusic(NULL, NULL);
    }


//------------------------------------------------------------
// ������
//------------------------------------------------------------
public:
    //------------------------------------------------------------------------------------------------------------------------
    // bool CSDLSequencer::Init()
    //   �T�v; �V�[�P���T�̏�����
    //   �Ԓl; ���ی���
    //   ����; float fBPM;                  �쓮�e���|
    //         bool (*funcBeatCallBack)();  ���̃^�C�~���O�ŃR�[���o�b�N�����֐�
    //         int iCallBackInterval;       �R�[���o�b�N���锏�Ԋu�i4�Ȃ�C4����1��R�[���o�b�N����j
    //         float fLatencyEstimation;    �o�b�t�@�Đ����Ԃ̉������V�[�P���X�����Ɏg�p����Ă���ƌ��ς��邩�D0.5�ʂ��Ó��H
    //------------------------------------------------------------------------------------------------------------------------
    bool Init(float fBPM, bool (*funcBeatCallBack)(DWORD, float), int iCallBackInterval, float fLatencyEstimation = 0.0f)
    {
        // SDLCore �� SDL_mixer �ݒ���Q��
        CSDLCore::tag_SDLInfo::tag_Mixer*   pSetting = &(CSDLCore::s_pInstance->m_SDLInfo.Mixer);
        // 16bit/stereo �ȊO�͕s��
        if (pSetting->m_usFormat != AUDIO_S16LSB || !pSetting->m_bStereo) return false;

        // �p�����[�^������
        m_iBeatCallBackInterval = iCallBackInterval;
        m_dwPastByte            = 0;
        m_iBeatCounter          = 0;
        m_iFineBeatCounter      = 0;
        m_fBeatPerMinuts        = fBPM;
        m_fBytePerBeats         = pSetting->m_ulFreq * 240.0f / fBPM;   // 240  = 60[sec/min]*2[byte/data]*2[stereo]
        m_fBytePerFineBeats     = 0;
        m_fSecondPerByte        = 0.25f / pSetting->m_ulFreq;           // 0.25 = 1 / (2[byte/data]*2[stereo])
        m_fLatency              = (float)pSetting->m_ulBufferSize / (float)pSetting->m_ulFreq * (1.0f-fLatencyEstimation);
        m_bPlay                 = false;

        // �r�[�g�R�[���o�b�N�֐�
        m_funcBeatCallBack      = funcBeatCallBack;
        m_funcFineCallBack      = NULL;

        // �`�����l��������
        ResetAllChannel();

        // �X�g���[���Đ��R�[���o�b�N�֐��̓o�^
        Mix_HookMusic(fpMusicHookCallBack, this);

        return true;
    }

    // �ׂ������݂̃R�[���o�b�N�֐���ݒ肷��
    void SetFineCallBack(bool (*funcFineCallBack)(DWORD, float), int iQuantize)
    {
        m_funcFineCallBack  = funcFineCallBack;
        m_fBytePerFineBeats = m_fBytePerBeats * 4 / iQuantize;
        m_iFineBeatCounter  = 0;
    }

    // �G�t�F�N�^�̒ǉ�
    void AddEffector(ISDLEffect* pEffect)
    {
        if (m_pEffector == NULL) m_pEffector = pEffect;
        else m_pEffector->Connect(pEffect);
    }

    // �S�`�����l�����Z�b�g
    void ResetAllChannel()
    {
        // �`�����l��������
        for (int i=0; i<_SDLSEQ_CHANNEL_MAX; i++) {
            m_Channel[i][0].Init();
            m_Channel[i][1].Init();
            m_iActiveSub[i] = 0;
        }
    }

//------------------------------------------------------------
// ���s
//------------------------------------------------------------
public:
    //------------------------------------------------------------------------------------------------------------------------
    // float CSDLSequencer::PlaySound()
    //   �T�v; �E�F�[�u�f�[�^�� BPM �����Đ�
    //   �Ԓl; �����ɂ�锭���̒x��[sec]�D
    //   ����; CSDLSound* pSound;       �Đ��f�[�^
    //         int        iChannel;     �Đ��`�����l���̎w�� (0-15)
    //         int        iLoopCount;   ���[�v��   (0�Ŗ����J��Ԃ�)
    //         int        iVolume;      �{�����[�� (0-128)
    //         �y�����^�C�~���O�̎w��z
    //         float      fQuantize;    �����𓯊�������^�C�~���O�DfQuantize=4 �Ŏl�������DfQuantize=16 ��16�������D
    //         float      fDelay;       �����x���DfDelay=1 �Ŏl���������x��D
    //         �y�f�[�^�؏o�̎w��z
    //         float      fStartPoint;  �����J�n�ʒu�DfStartPoint=1 �Ŏl���������ォ��؂�o���Ĕ����D0 �w��ōŏ����甭���D
    //         float      fLength;      ���������DfLength=1 �Ŏl���������𔭉��D0 �w��Ńf�[�^�����������D
    //         float      fVoidLength;  ���[�v�Ȃ��ڂɐ݂��閳�������̑傫���DfVoidLength=0.05f �Ŏl��������5%���̖���������
    //------------------------------------------------------------------------------------------------------------------------
    float PlaySound(CSDLSound* pSound, 
                    int iChannel, 
                    int iLoopCount    = 1, 
                    int iVolume       = 80, 
                    float fQuantize   = 4, 
                    float fDelay      = 0, 
                    float fStartPoint = 0, 
                    float fLength     = 0, 
                    float fVoidLength = 0)
    {
        // �����x�����v�Z
        int iDataDelay = CalcDataDelayByQuantize(fQuantize, fDelay);
        int iStartPos = 0;

        // �w��`�����l�����g�p���̏ꍇ
        if (IsUsing(iChannel)) {
            if (m_Channel[iChannel][m_iActiveSub[iChannel]].m_iDelay != iDataDelay) {
                // ���݂̃`�����l���̔������~�߂�
                m_Channel[iChannel][m_iActiveSub[iChannel]].Stop(iDataDelay);
                // �T�u�`�����l����؂�ւ���
                m_iActiveSub[iChannel] = 1 - m_iActiveSub[iChannel];
            }
        }

        // �`�����l���f�[�^�ݒ�
        CSDLSeqChannel* pChannel = &(m_Channel[iChannel][m_iActiveSub[iChannel]]);

        // ���l�ݒ�
        pChannel->m_iLoopCount  = -iLoopCount;  // ���l�Ń��[�v�񐔂��w�肷��D
        pChannel->m_iVolume     = iVolume;
        pChannel->m_iSilence    = 0;
        pChannel->m_iDelay      = iDataDelay;
        pChannel->m_iStartPoint = 0;
        pChannel->m_fiFadeT     = 0;
        pChannel->m_fiFadeDT    = 0;
        pChannel->m_bPlaying    = false;

        // �Đ��f�[�^
        if (fStartPoint == 0.0f) {
            // �؂�o���w�薳
            pChannel->m_pData = pSound->GetData();
        } else {
            // �؂�o���w��L
            iStartPos = CalcDataLengthFromBeat(fStartPoint);
            if (iStartPos > (int)pSound->GetLength()) return 0;
            pChannel->m_pData = &(pSound->GetData()[iStartPos]);
        }

        // �Đ��f�[�^���̌v�Z
        if (fLength == 0.0f) {
            // �I���W�i���f�[�^�����g�p
            pChannel->m_iDataLength = pSound->GetLength();
        } else {
            // �󌄒����v�Z
            pChannel->m_iSilence    = CalcDataLengthFromBeat(fVoidLength);
            // �r�[�g���w�肩��f�[�^�����v�Z
            pChannel->m_iDataLength = CalcDataLengthFromBeat(fLength) - pChannel->m_iSilence;
            if (pChannel->m_iDataLength > (int)pSound->GetLength()-iStartPos) {
                // �I���W�i���f�[�^����蒷���ꍇ�C�������Ԃ�ݒ肷��
                pChannel->m_iSilence    = pChannel->m_iDataLength - pSound->GetLength() - iStartPos;
                pChannel->m_iDataLength = pSound->GetLength() - iStartPos;
            }
        }
        // �ŏI���[�v�ł��Ō�܂ōĐ�����
        pChannel->m_iEndPoint   = pChannel->m_iDataLength;

        // �����̒x����v�Z���ĕԂ�
        return CalcDelayByQuantize(iDataDelay);
    }

    float StopSound(int iChannel, float fQuantize=4, float fDelay=0)
    {
        // �����x�����v�Z
        int iDataDelay = CalcDataDelayByQuantize(fQuantize, fDelay);
        // �`�����l���̔������~�߂�
        m_Channel[iChannel][m_iActiveSub[iChannel]].Stop(iDataDelay);
        // �����̒x����v�Z���ĕԂ�
        return CalcDelayByQuantize(iDataDelay);
    }

    float FadeSound(int iChannel, bool bFadeIn, float fQuantize=4, float fDelay=0, float fLength = 16)
    {
        // �����x�����v�Z
        int iDataDelay = CalcDataDelayByQuantize(fQuantize, fDelay);
        int iLength    = CalcDataLengthFromBeat(fLength);
        // �`�����l��Fader
        if (bFadeIn) m_Channel[iChannel][m_iActiveSub[iChannel]].FadeIn(iDataDelay, iLength);
        else         m_Channel[iChannel][m_iActiveSub[iChannel]].FadeOut(iDataDelay, iLength);
        // �����̒x����v�Z���ĕԂ�
        return CalcDelayByQuantize(iDataDelay);
    }

    void Stop()
    {
        m_bPlay = false;
    }

    void Start()
    {
        m_bPlay = true;
        m_dwPastByte   = 0;
        m_iBeatCounter = 0;
        m_iFineBeatCounter = 0;
        ResetAllChannel();
    }

    void Pause()
    {
        m_bPlay = false;
    }

    void Resume()
    {
        m_bPlay = true;
    }


//------------------------------------------------------------
// �Q��
//------------------------------------------------------------
public:
    // �`�����l���g�p����
    inline bool IsUsing(int iChannel)
    {
        return (m_Channel[iChannel][m_iActiveSub[iChannel]].m_pData != NULL);
    }

    // ���`�����l���Đ���
    inline bool IsFlooding(int iChannel)
    {
        return (m_Channel[iChannel][0].m_bPlaying && m_Channel[iChannel][1].m_bPlaying);
    }

    // BPM �Q��
    inline float GetBPM()
    {
        return m_fBeatPerMinuts;
    }

    // ��`�����l������
    inline int GetFreeChannel()
    {
        for (int i=0; i<_SDLSEQ_CHANNEL_MAX; i++) if (!IsUsing(i)) return i;
        return 15;
    }
   
    // �l�������P�ʒ�����f�[�^�o�C�g�����v�Z
    inline int CalcDataLengthFromBeat(float fLengthInBeat)
    {
        return ((int)(fLengthInBeat * m_fBytePerBeats)) & 0x7ffffffc;
    }

    // �f�[�^�����甏�����v�Z
    inline float CalcBeatFromDataLength(int iDataLength)
    {
        return (float)iDataLength / m_fBytePerBeats;
    }

    // �����x��b�����v�Z
    inline float CalcDelayByQuantize(float fQuantize, float fDelay)
    {
        return CalcDelayByQuantize(CalcDataDelayByQuantize(fQuantize, fDelay));
    }

    // �����x��b�����v�Z�i�o�C�g������j
    inline float CalcDelayByQuantize(int iDataDelay)
    {
        return iDataDelay * m_fSecondPerByte + m_fLatency;
    }

private:
    // �����x��o�C�g�����v�Z
    inline int CalcDataDelayByQuantize(float fQuantize, float fDelay)
    {
        // [�O���b�h��] = [1���ߓ���̃f�[�^��] / [Quantize]
        float fGrid = m_fBytePerBeats * 4 / fQuantize;
        // [�����^�C�~���O] = 4Byte �ۂߍ��� ([�O���b�h��] - ([�Đ��ς݃f�[�^��] + [Delay�����f�[�^��]) % [�O���b�h��]) 
        return ((int)(fGrid - fmodf((m_dwPastByte + fDelay * m_fBytePerBeats + fGrid - 1), fGrid)) + 2) & 0x7ffffffc;
    }


//------------------------------------------------------------
// SDL_mixer ����Ă΂��X�g���[���Đ��p�R�[���o�b�N�֐�
//------------------------------------------------------------
public:
    static void fpMusicHookCallBack(void *udata, Uint8 *stream, int len)
    {
        CSDLSequencer* pSeq = (CSDLSequencer*)udata;
        if (!pSeq->m_bPlay) return;

        // �r�[�g�����ށD��Ԋu�Ńr�[�g�R�[���o�b�N�֐����ĂԁD
        pSeq->CountBeat(len);
        pSeq->FineCallBack(len);
        // �`�����l���f�[�^���X�g���[���ɍ����D
        pSeq->StreamingChannel(stream, len);
        // �}�X�^�[�G�t�F�N�g��������
        pSeq->MasterEffect(stream, len);
        // �^�C���J�E���^��i�߂�D
        pSeq->CountTime(len);
    }


//--------------------------------------------------
// �X�g���[���Đ��T�u���[�`��
//--------------------------------------------------
private:
    // �X�g���[���Đ�����f�[�^������C�r�[�g���J�E���g����
    void CountBeat(DWORD dwDataLength)
    {
        // [���̃r�[�g�ʒu] = [�r�[�g�J�E���^] * [1Beat����̃o�C�g��]
        DWORD dwNextBeat = (DWORD)(m_iBeatCounter * m_fBytePerBeats);
        // [���̃r�[�g�ʒu] < [�X�g���[���Đ��ς݃f�[�^��] + [����̃X�g���[���Đ��f�[�^��]
        if (dwNextBeat < m_dwPastByte + dwDataLength) {
            // ����̃X�g���[���Đ����ŁC1�r�[�g����
            // m_iBeatCallBackInterval �r�[�g�Ԋu�ŃR�[���o�b�N
            if ((m_iBeatCounter % m_iBeatCallBackInterval) == 0 && m_funcBeatCallBack != NULL) {
                // �r�[�g�R�[���o�b�N�֐����Ă�
                m_funcBeatCallBack(m_iBeatCounter, CalcDelayByQuantize(dwNextBeat - m_dwPastByte));
            }
            // �r�[�g�J�E���^���C���N�������g
            m_iBeatCounter++;
        }
    }

    // 4Beat���Z���R�[���o�b�N�֐����Ăяo��
    void FineCallBack(DWORD dwDataLength)
    {
        if (m_funcFineCallBack == NULL) return;
        // [���̃r�[�g�ʒu] = [�J�E���^] * [1FineBeat����̃o�C�g��]
        DWORD dwNextBeat;

        while(true) {
            dwNextBeat = (DWORD)(m_iFineBeatCounter * m_fBytePerFineBeats);
            // [���̃r�[�g�ʒu] < [�X�g���[���Đ��ς݃f�[�^��] + [����̃X�g���[���Đ��f�[�^��]
            if (dwNextBeat < m_dwPastByte + dwDataLength) {
                // �R�[���o�b�N�֐����Ă�
                m_funcFineCallBack(m_iFineBeatCounter, CalcDelayByQuantize(dwNextBeat - m_dwPastByte));
                // �r�[�g�J�E���^���C���N�������g
                m_iFineBeatCounter++;
            } else break;
        }
    }

    // �S�`�����l���f�[�^���X�g���[���ɍ���
    void StreamingChannel(Uint8 *stream, int len)
    {
        int i;
        for (i=0; i<_SDLSEQ_CHANNEL_MAX; i++) {
            if (m_Channel[i][0].Wait(len)) m_Channel[i][0].Streaming(stream, len);
            if (m_Channel[i][1].Wait(len)) m_Channel[i][1].Streaming(stream, len);
        }
    }

    // �}�X�^�[�G�t�F�N�g
    void MasterEffect(Uint8 *stream, int len)
    {
        ISDLEffect* pEffect;
        for (pEffect = m_pEffector; pEffect != NULL; pEffect = pEffect->GetNextEffect()) {
            if (!pEffect->IsActive()) continue;
            if (!pEffect->Exec(stream, len)) return;
        }
    }

    // ���ԍX�V
    void CountTime(DWORD dwDataLength)
    {
        // �X�g���[���Đ��ς݃f�[�^�����C���N�������g
        m_dwPastByte += dwDataLength;
    }
};
