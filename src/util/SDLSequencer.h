//--------------------------------------------------------------------------------
//  SDL Sound Wrapping Class
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
//  BPM 同期Wave再生クラス
//
//--------------------------------------------------------------------------------

#include "Math.h"
#include "SDLEffector.h"

#define  _SDLSEQ_CHANNEL_MAX    16



//------------------------------------------------------------
// シーケンサクラス
//------------------------------------------------------------
class CSDLSequencer
{
//------------------------------------------------------------
// メンバクラス
//------------------------------------------------------------
public:
    //--------------------------------------------------
    // チャンネルデータクラス
    //--------------------------------------------------
    class CSDLSeqChannel {
    //--------------------------------------------------
    // メンバ変数
    //--------------------------------------------------
    public:
        Uint8*  m_pData;        // 再生データ
        int     m_iDataLength;  // 再生データサイズ

        int     m_iVolume;      // 再生音量
        int     m_iLoopCount;   // ループカウンタ
        int     m_iDelay;       // 再生されるまでの待機バイト数
        int     m_iSilence;     // 次回ループ再生までの無音待機バイト数
        int     m_iEndPoint;    // 最終ループで再生終了するデータ位置
        int     m_iStartPoint;  // 次回ストリームに書込む再生データ位置

        int     m_fiFadeT;      // フェード音量（小数部16bit の固定小数）
        int     m_fiFadeDT;     // フェード音量変化（小数部16bit の固定小数）

        bool    m_bPlaying;     // 発音中

    //--------------------------------------------------
    // メンバ関数
    //--------------------------------------------------
    public:
        // 初期化
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

        // 終了処理
        void Exit()
        {
            m_iLoopCount = -1;
            m_pData = NULL;
            m_bPlaying = false;
        }

        // 発音の終了
        void Stop(int iStopDelay)
        {
            if (m_pData == NULL) return;
            if (!m_bPlaying) {
                // 待機中なら終了
                Exit();
            } else {
                // 発音中なら再生終了位置を計算
                int iStopPoint = m_iStartPoint + iStopDelay;
                // ループ数
                int iLoop = iStopPoint / m_iDataLength;
                // 最終ループの再生終了位置
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
            // 小数部16bit の固定小数を使用
            m_fiFadeT  = 0;
            m_fiFadeDT = (128 << 16) / (iFadeLength >> 8);
        }

        // FadeOut
        void FadeOut(int iFadeDelay, int iFadeLength)
        {
            // 小数部16bit の固定小数を使用
            m_fiFadeT  = (128 << 16);
            m_fiFadeDT = - ((128 << 16) / (iFadeLength >> 8));
        }

        // 待機（return true ならストリームに書き込みを要求）
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

        // ストリームに書き込む
        void Streaming(Uint8* stream, int iStreamLength)
        {
            // [書込長] = [ストリーム長] - [待機時間]
            int iCopyLength = iStreamLength - m_iDelay;

            // [データ先頭アドレス] + [書込長] > [データ長]
            if (m_iStartPoint + iCopyLength > m_iDataLength) {
                // 今回の書込みでデータ最後まで到達する場合
                // [書込長] = [データ長] - [データ先頭アドレス]
                iCopyLength = m_iDataLength - m_iStartPoint;
                // ストリームにデータコピー
                AddData((short*)&(stream[m_iDelay]), (short*)&(m_pData[m_iStartPoint]), iCopyLength);

                // ループカウンタをインクリメント
                ++m_iLoopCount;
                switch(m_iLoopCount) {
                case 0:
                    // ループカウンタ == 0 なら再生終了
                    Exit();
                    return;
                case -1:
                    // ループカウンタ == 1 なら次回再生終了ポイントを設定
                    m_iDataLength = m_iEndPoint;
                    break;
                }

                // [ストリーム書込位置] += [コピー済バイト数] + [次ループまでの無音待機バイト数]
                m_iDelay += iCopyLength + m_iSilence;
                // [書込長] = [ストリーム長] - [待機時間]
                iCopyLength = iStreamLength - m_iDelay;
                // 今回の書込みでは，次ループの最初に到達しない
                if (iCopyLength <= 0) return;
                // 次ループをデータ先頭から書込む
                m_iStartPoint = 0;
            }

            // ストリームにデータコピー
            AddData((short*)&(stream[m_iDelay]), (short*)&(m_pData[m_iStartPoint]), iCopyLength);

            // [データ先頭アドレス] を今回書き込んだ分進める
            m_iStartPoint += iCopyLength;
            // 次回はストリームの先頭から書込む
            m_iDelay = 0;
        }

    private:
        // ストリームに加算
        void AddData(short* stream, short* dat, int len)
        {
            int i;
            int vol = m_iVolume;

            // sampling 単位に変換
            len >>= 1;

            if (m_fiFadeDT == 0) {
                // 通常加算
                for (i=0; i<len; i++) stream[i] += (dat[i]*vol)>>7;
            } else {
                // フェーディングテーブルの作成
                char achFadeTable[64];
                int  imax = len >> 8;
                for (int i=0; i<imax; i++, m_fiFadeT+=m_fiFadeDT) achFadeTable[i] = (m_fiFadeT >> 16);
                // フェード加算
                for (i=0; i<len; i++) stream[i] += (dat[i]*vol*achFadeTable[i>>8])>>14;
                // フェード終了
                if (m_fiFadeT < 0) Exit();
                else if (m_fiFadeT > (128 << 16)) m_fiFadeDT = 0;
            }
        }
    };


//------------------------------------------------------------
// メンバ変数
//------------------------------------------------------------
private:
    // 内部パラメータ
    float   m_fBeatPerMinuts;           // １分間のビート数
    float   m_fBytePerBeats;            // １ビート当りのバイト数
    float   m_fBytePerFineBeats;        // m_funcFineCallBack を呼び出すタイミングのバイト数
    float   m_fSecondPerByte;           // １バイト分に相当する秒数
    float   m_fLatency;                 // バッファによるレイテンシー
    DWORD   m_dwPastByte;               // 初期化してからの経過バイト数
    int     m_iBeatCounter;             // 初期化してからのビートカウンタ
    int     m_iFineBeatCounter;         // m_funcFineCallBack を呼び出すカウンタ
    int     m_iBeatCallBackInterval;    // コールバックするビート間隔
    bool    m_bPlay;                    // シーケンサが動いてるか

    // チャンネル
    CSDLSeqChannel  m_Channel[_SDLSEQ_CHANNEL_MAX][2];  // チャンネルデータ
    int             m_iActiveSub[_SDLSEQ_CHANNEL_MAX];  // 使用中のサブチャンネル番号

    // ビートコールバック関数
    bool    (*m_funcBeatCallBack)(DWORD, float);
    bool    (*m_funcFineCallBack)(DWORD, float);

    // エフェクタ
    ISDLEffect*     m_pEffector;

//------------------------------------------------------------
// メンバ関数
//------------------------------------------------------------
public:
    CSDLSequencer()
    {
        // 初期化
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
        // ストリーム再生コールバック関数をはずす
        Mix_HookMusic(NULL, NULL);
    }


//------------------------------------------------------------
// 初期化
//------------------------------------------------------------
public:
    //------------------------------------------------------------------------------------------------------------------------
    // bool CSDLSequencer::Init()
    //   概要; シーケンサの初期化
    //   返値; 成否結果
    //   引数; float fBPM;                  作動テンポ
    //         bool (*funcBeatCallBack)();  拍のタイミングでコールバックされる関数
    //         int iCallBackInterval;       コールバックする拍間隔（4なら，4拍に1回コールバックする）
    //         float fLatencyEstimation;    バッファ再生時間の何％がシーケンス処理に使用されていると見積もるか．0.5位が妥当？
    //------------------------------------------------------------------------------------------------------------------------
    bool Init(float fBPM, bool (*funcBeatCallBack)(DWORD, float), int iCallBackInterval, float fLatencyEstimation = 0.0f)
    {
        // SDLCore の SDL_mixer 設定を参照
        CSDLCore::tag_SDLInfo::tag_Mixer*   pSetting = &(CSDLCore::s_pInstance->m_SDLInfo.Mixer);
        // 16bit/stereo 以外は不可
        if (pSetting->m_usFormat != AUDIO_S16LSB || !pSetting->m_bStereo) return false;

        // パラメータ初期化
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

        // ビートコールバック関数
        m_funcBeatCallBack      = funcBeatCallBack;
        m_funcFineCallBack      = NULL;

        // チャンネル初期化
        ResetAllChannel();

        // ストリーム再生コールバック関数の登録
        Mix_HookMusic(fpMusicHookCallBack, this);

        return true;
    }

    // 細かい刻みのコールバック関数を設定する
    void SetFineCallBack(bool (*funcFineCallBack)(DWORD, float), int iQuantize)
    {
        m_funcFineCallBack  = funcFineCallBack;
        m_fBytePerFineBeats = m_fBytePerBeats * 4 / iQuantize;
        m_iFineBeatCounter  = 0;
    }

    // エフェクタの追加
    void AddEffector(ISDLEffect* pEffect)
    {
        if (m_pEffector == NULL) m_pEffector = pEffect;
        else m_pEffector->Connect(pEffect);
    }

    // 全チャンネルリセット
    void ResetAllChannel()
    {
        // チャンネル初期化
        for (int i=0; i<_SDLSEQ_CHANNEL_MAX; i++) {
            m_Channel[i][0].Init();
            m_Channel[i][1].Init();
            m_iActiveSub[i] = 0;
        }
    }

//------------------------------------------------------------
// 実行
//------------------------------------------------------------
public:
    //------------------------------------------------------------------------------------------------------------------------
    // float CSDLSequencer::PlaySound()
    //   概要; ウェーブデータの BPM 同期再生
    //   返値; 同期による発音の遅れ[sec]．
    //   引数; CSDLSound* pSound;       再生データ
    //         int        iChannel;     再生チャンネルの指定 (0-15)
    //         int        iLoopCount;   ループ数   (0で無限繰り返し)
    //         int        iVolume;      ボリューム (0-128)
    //         【発音タイミングの指定】
    //         float      fQuantize;    発音を同期させるタイミング．fQuantize=4 で四分同期．fQuantize=16 で16分同期．
    //         float      fDelay;       発音遅延．fDelay=1 で四分音符長遅れ．
    //         【データ切出の指定】
    //         float      fStartPoint;  発音開始位置．fStartPoint=1 で四分音符長後から切り出して発音．0 指定で最初から発音．
    //         float      fLength;      発音長さ．fLength=1 で四分音符長を発音．0 指定でデータ長さ分発音．
    //         float      fVoidLength;  ループつなぎ目に設ける無音部分の大きさ．fVoidLength=0.05f で四分音符の5%分の無音を挟む
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
        // 発音遅延を計算
        int iDataDelay = CalcDataDelayByQuantize(fQuantize, fDelay);
        int iStartPos = 0;

        // 指定チャンネルが使用中の場合
        if (IsUsing(iChannel)) {
            if (m_Channel[iChannel][m_iActiveSub[iChannel]].m_iDelay != iDataDelay) {
                // 現在のチャンネルの発音を止める
                m_Channel[iChannel][m_iActiveSub[iChannel]].Stop(iDataDelay);
                // サブチャンネルを切り替える
                m_iActiveSub[iChannel] = 1 - m_iActiveSub[iChannel];
            }
        }

        // チャンネルデータ設定
        CSDLSeqChannel* pChannel = &(m_Channel[iChannel][m_iActiveSub[iChannel]]);

        // 諸値設定
        pChannel->m_iLoopCount  = -iLoopCount;  // 負値でループ回数を指定する．
        pChannel->m_iVolume     = iVolume;
        pChannel->m_iSilence    = 0;
        pChannel->m_iDelay      = iDataDelay;
        pChannel->m_iStartPoint = 0;
        pChannel->m_fiFadeT     = 0;
        pChannel->m_fiFadeDT    = 0;
        pChannel->m_bPlaying    = false;

        // 再生データ
        if (fStartPoint == 0.0f) {
            // 切り出し指定無
            pChannel->m_pData = pSound->GetData();
        } else {
            // 切り出し指定有
            iStartPos = CalcDataLengthFromBeat(fStartPoint);
            if (iStartPos > (int)pSound->GetLength()) return 0;
            pChannel->m_pData = &(pSound->GetData()[iStartPos]);
        }

        // 再生データ長の計算
        if (fLength == 0.0f) {
            // オリジナルデータ長を使用
            pChannel->m_iDataLength = pSound->GetLength();
        } else {
            // 空隙長を計算
            pChannel->m_iSilence    = CalcDataLengthFromBeat(fVoidLength);
            // ビート数指定からデータ長を計算
            pChannel->m_iDataLength = CalcDataLengthFromBeat(fLength) - pChannel->m_iSilence;
            if (pChannel->m_iDataLength > (int)pSound->GetLength()-iStartPos) {
                // オリジナルデータ長より長い場合，無音期間を設定する
                pChannel->m_iSilence    = pChannel->m_iDataLength - pSound->GetLength() - iStartPos;
                pChannel->m_iDataLength = pSound->GetLength() - iStartPos;
            }
        }
        // 最終ループでも最後まで再生する
        pChannel->m_iEndPoint   = pChannel->m_iDataLength;

        // 発音の遅れを計算して返す
        return CalcDelayByQuantize(iDataDelay);
    }

    float StopSound(int iChannel, float fQuantize=4, float fDelay=0)
    {
        // 発音遅延を計算
        int iDataDelay = CalcDataDelayByQuantize(fQuantize, fDelay);
        // チャンネルの発音を止める
        m_Channel[iChannel][m_iActiveSub[iChannel]].Stop(iDataDelay);
        // 発音の遅れを計算して返す
        return CalcDelayByQuantize(iDataDelay);
    }

    float FadeSound(int iChannel, bool bFadeIn, float fQuantize=4, float fDelay=0, float fLength = 16)
    {
        // 発音遅延を計算
        int iDataDelay = CalcDataDelayByQuantize(fQuantize, fDelay);
        int iLength    = CalcDataLengthFromBeat(fLength);
        // チャンネルFader
        if (bFadeIn) m_Channel[iChannel][m_iActiveSub[iChannel]].FadeIn(iDataDelay, iLength);
        else         m_Channel[iChannel][m_iActiveSub[iChannel]].FadeOut(iDataDelay, iLength);
        // 発音の遅れを計算して返す
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
// 参照
//------------------------------------------------------------
public:
    // チャンネル使用中か
    inline bool IsUsing(int iChannel)
    {
        return (m_Channel[iChannel][m_iActiveSub[iChannel]].m_pData != NULL);
    }

    // 両チャンネル再生中
    inline bool IsFlooding(int iChannel)
    {
        return (m_Channel[iChannel][0].m_bPlaying && m_Channel[iChannel][1].m_bPlaying);
    }

    // BPM 参照
    inline float GetBPM()
    {
        return m_fBeatPerMinuts;
    }

    // 空チャンネル検索
    inline int GetFreeChannel()
    {
        for (int i=0; i<_SDLSEQ_CHANNEL_MAX; i++) if (!IsUsing(i)) return i;
        return 15;
    }
   
    // 四部音符単位長からデータバイト数を計算
    inline int CalcDataLengthFromBeat(float fLengthInBeat)
    {
        return ((int)(fLengthInBeat * m_fBytePerBeats)) & 0x7ffffffc;
    }

    // データ長から拍数を計算
    inline float CalcBeatFromDataLength(int iDataLength)
    {
        return (float)iDataLength / m_fBytePerBeats;
    }

    // 発音遅れ秒数を計算
    inline float CalcDelayByQuantize(float fQuantize, float fDelay)
    {
        return CalcDelayByQuantize(CalcDataDelayByQuantize(fQuantize, fDelay));
    }

    // 発音遅れ秒数を計算（バイト数から）
    inline float CalcDelayByQuantize(int iDataDelay)
    {
        return iDataDelay * m_fSecondPerByte + m_fLatency;
    }

private:
    // 発音遅れバイト数を計算
    inline int CalcDataDelayByQuantize(float fQuantize, float fDelay)
    {
        // [グリッド長] = [1小節当りのデータ長] / [Quantize]
        float fGrid = m_fBytePerBeats * 4 / fQuantize;
        // [発音タイミング] = 4Byte 丸め込み ([グリッド長] - ([再生済みデータ長] + [Delay相当データ長]) % [グリッド長]) 
        return ((int)(fGrid - fmodf((m_dwPastByte + fDelay * m_fBytePerBeats + fGrid - 1), fGrid)) + 2) & 0x7ffffffc;
    }


//------------------------------------------------------------
// SDL_mixer から呼ばれるストリーム再生用コールバック関数
//------------------------------------------------------------
public:
    static void fpMusicHookCallBack(void *udata, Uint8 *stream, int len)
    {
        CSDLSequencer* pSeq = (CSDLSequencer*)udata;
        if (!pSeq->m_bPlay) return;

        // ビートを刻む．定間隔でビートコールバック関数を呼ぶ．
        pSeq->CountBeat(len);
        pSeq->FineCallBack(len);
        // チャンネルデータをストリームに合成．
        pSeq->StreamingChannel(stream, len);
        // マスターエフェクトをかける
        pSeq->MasterEffect(stream, len);
        // タイムカウンタを進める．
        pSeq->CountTime(len);
    }


//--------------------------------------------------
// ストリーム再生サブルーチン
//--------------------------------------------------
private:
    // ストリーム再生するデータ数から，ビートをカウントする
    void CountBeat(DWORD dwDataLength)
    {
        // [次のビート位置] = [ビートカウンタ] * [1Beat当りのバイト数]
        DWORD dwNextBeat = (DWORD)(m_iBeatCounter * m_fBytePerBeats);
        // [次のビート位置] < [ストリーム再生済みデータ数] + [今回のストリーム再生データ数]
        if (dwNextBeat < m_dwPastByte + dwDataLength) {
            // 今回のストリーム再生内で，1ビート刻む
            // m_iBeatCallBackInterval ビート間隔でコールバック
            if ((m_iBeatCounter % m_iBeatCallBackInterval) == 0 && m_funcBeatCallBack != NULL) {
                // ビートコールバック関数を呼ぶ
                m_funcBeatCallBack(m_iBeatCounter, CalcDelayByQuantize(dwNextBeat - m_dwPastByte));
            }
            // ビートカウンタをインクリメント
            m_iBeatCounter++;
        }
    }

    // 4Beatより短いコールバック関数を呼び出す
    void FineCallBack(DWORD dwDataLength)
    {
        if (m_funcFineCallBack == NULL) return;
        // [次のビート位置] = [カウンタ] * [1FineBeat当りのバイト数]
        DWORD dwNextBeat;

        while(true) {
            dwNextBeat = (DWORD)(m_iFineBeatCounter * m_fBytePerFineBeats);
            // [次のビート位置] < [ストリーム再生済みデータ数] + [今回のストリーム再生データ数]
            if (dwNextBeat < m_dwPastByte + dwDataLength) {
                // コールバック関数を呼ぶ
                m_funcFineCallBack(m_iFineBeatCounter, CalcDelayByQuantize(dwNextBeat - m_dwPastByte));
                // ビートカウンタをインクリメント
                m_iFineBeatCounter++;
            } else break;
        }
    }

    // 全チャンネルデータをストリームに合成
    void StreamingChannel(Uint8 *stream, int len)
    {
        int i;
        for (i=0; i<_SDLSEQ_CHANNEL_MAX; i++) {
            if (m_Channel[i][0].Wait(len)) m_Channel[i][0].Streaming(stream, len);
            if (m_Channel[i][1].Wait(len)) m_Channel[i][1].Streaming(stream, len);
        }
    }

    // マスターエフェクト
    void MasterEffect(Uint8 *stream, int len)
    {
        ISDLEffect* pEffect;
        for (pEffect = m_pEffector; pEffect != NULL; pEffect = pEffect->GetNextEffect()) {
            if (!pEffect->IsActive()) continue;
            if (!pEffect->Exec(stream, len)) return;
        }
    }

    // 時間更新
    void CountTime(DWORD dwDataLength)
    {
        // ストリーム再生済みデータ数をインクリメント
        m_dwPastByte += dwDataLength;
    }
};
