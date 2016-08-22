//--------------------------------------------------------------------------------
//  SDL Sound Wrapping Class
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

//------------------------------------------------------------
// サウンドエフェクター基本クラス
//------------------------------------------------------------
class ISDLEffect
{
private:
    ISDLEffect*     m_pPrevEffect;
    ISDLEffect*     m_pNextEffect;
    bool            m_bActive;

public:
    ISDLEffect()
    {
        m_pPrevEffect = NULL;
        m_pNextEffect = NULL;
        m_bActive = true;
    }

    void Connect(ISDLEffect* pEffect)
    {
        ISDLEffect* pEnd = this;
        while (pEnd->m_pNextEffect != NULL) pEnd = pEnd->m_pNextEffect;

        pEnd->m_pNextEffect    = pEffect;
        pEffect->m_pPrevEffect = pEnd;
    }

    void Delete()
    {
        if (m_pNextEffect != NULL) m_pNextEffect->m_pPrevEffect = m_pPrevEffect;
        if (m_pPrevEffect != NULL) m_pPrevEffect->m_pNextEffect = m_pNextEffect;
        m_pPrevEffect = NULL;
        m_pNextEffect = NULL;
    }

    ISDLEffect* GetPrevEffect()
    {
        return m_pPrevEffect;
    }

    ISDLEffect* GetNextEffect()
    {
        return m_pNextEffect;
    }

public:
    void Switch(bool bActive)
    {
        m_bActive = bActive;
    }

    bool IsActive()
    {
        return m_bActive;
    }

    virtual bool Init() = 0;
    virtual bool Exec(Uint8* stream, int len) = 0;
};





//------------------------------------------------------------
// レゾナンスエフェクター
//------------------------------------------------------------
class CRezonanceEffect : public ISDLEffect
{
private:
    int iVout[2];
    int iI[2];

    int m_iRezonance;
    int m_iCutoffFreq;

public:
    CRezonanceEffect()
    {
    }

public:
    // レゾナンス(128-1)，カットオフ(0-128-256)
    bool Set(int iRez, int iCutoff)
    {
        m_iRezonance = iRez;
        m_iCutoffFreq = iCutoff;
        return true;
    }

public:
    bool Init()
    {
        iVout[0] = 0;
        iVout[1] = 0;
        iI[0] = 0;
        iI[1] = 0;
        return Set(128, 128);
    }

    bool Exec(Uint8* stream, int len)
    {
        short* dat = (short*)stream;
        len >>= 1;

        int i;
        int iRez = m_iRezonance;
        int iCut = m_iCutoffFreq;

        for (i=0; i<len; i+=2) {
            iI[0] += ((dat[i]   - iVout[0] - ((iI[0]*iRez)>>7)) * iCut) >> 7;
            iVout[0] += (iI[0] * iCut) >> 7;
            dat[i]   = iVout[0];

            iI[1] += ((dat[i+1] - iVout[1] - ((iI[1]*iRez)>>7)) * iCut) >> 7;
            iVout[1] += (iI[1] * iCut) >> 7;
            dat[i+1] = iVout[1];
        }

        return true;
    }
};






//------------------------------------------------------------
// Lo-Fi エフェクター
//------------------------------------------------------------
class CLofiEffect : public ISDLEffect
{
private:
    short   m_asNoiseTable[8192];
    int     iVout;
    int     iI;

    int     m_iRezonance;
    int     m_iCutoffFreq;
    int     m_iWhiteNoiseLevel;
    int     m_iCrackNoiseLevel;

public:
    CLofiEffect()
    {
    }

public:
    // レゾナンス(128-1)，カットオフ(0-128-256), WhiteNoiseLevel(0-128), CrackNoiseLevel(0-4)
    bool Set(int iRez, int iCutoff, int iWNoiz, int iCNoiz)
    {
        m_iRezonance = iRez;
        m_iCutoffFreq = iCutoff;
        m_iWhiteNoiseLevel = iWNoiz;
        m_iCrackNoiseLevel = 4-iCNoiz;
        return true;
    }

public:
    bool Init()
    {
        iVout = 0;
        iI    = 0;

        for (int i=0; i<8192; i++) {
            m_asNoiseTable[i] = (short)(randMT() & 0xfff) - 2048;
        }

        return Set(128, 128, 0, 0);
    }

    bool Exec(Uint8* stream, int len)
    {
        short* dat = (short*)stream;
        len >>= 1;

        int     i, offset, v, idx;
        int     iRez = m_iRezonance;
        int     iCut = m_iCutoffFreq;
        int     iNzl = m_iWhiteNoiseLevel;
        short*  noiz = m_asNoiseTable;

        offset = randMT() & 511;
        for (i=0; i<len; i+=2) {
            iI    += ((((dat[i]+dat[i+1])>>1) - iVout - ((iI*iRez)>>7)) * iCut) >> 7;
            iVout += (iI * iCut) >> 7;
            dat[i+1] = dat[i] = iVout + ((noiz[i+offset] * iNzl) >> 7);
        }

        if ((randMT() & ((1 << m_iCrackNoiseLevel)-1)) != 0) return true;
        len  = (len >> 1) - 2;
        idx = (randMT() & len) << 1;
        v = ((randMT() & 0x7fff) * iNzl) >> 7;
        dat[idx+1] = dat[idx]   = v;
        dat[idx+3] = dat[idx+2] = -v;

        return true;
    }
};






//------------------------------------------------------------
// ステレオコーラスエフェクター
//------------------------------------------------------------
class CChorusEffect : public ISDLEffect
{
private:
    short   m_asBuffer[8192];
    int     m_iDelay;
    int     m_iModuleWidth;
    int     m_iModuleFreq;
    int     m_iFeadBack;
    short   m_fsPhase;      // （小数部8bitの固定小数）

public:
    CChorusEffect()
    {
    }

public:
    // ディレイ量(1-2048)，変調幅(0-2048)，変調周波数(通常0-5000)，フィードバック量(0-128)
    bool Set(int iDelay, int iModulWidth, int iModulFreq, int iFeadBack)
    {
        // m_iDelay と m_iModuleWidth はデータ数で記憶，ステレオなのでx2
        m_iDelay       = iDelay * 2;
        m_iModuleWidth = iModulWidth * 2;
        m_iModuleFreq  = iModulFreq;
        m_iFeadBack    = iFeadBack;
        memset(m_asBuffer, 0, sizeof(short)*8192);

        return true;
    }

public:
    bool Init()
    {
        m_fsPhase = 0;
        return Set(1, 0, 0, 0);
    }

    bool Exec(Uint8* stream, int len)
    {
        short* buf = m_asBuffer;
        short* dat = (short*)stream;
        len >>= 1;

        m_fsPhase += m_iModuleFreq;

        int i, j;
        int iPhase     = (m_fsPhase * m_fsPhase) >> 16;
        int iDataDelay = len - (m_iDelay + ((m_iModuleWidth * iPhase) >> 14));
        int iEffVol    = m_iFeadBack;
        int iFilter    = len - 1;

        for (i=0; i<len; i+=2) {
            j = (i + iDataDelay) & iFilter;
            dat[i]   = buf[i]   = ((dat[i]   << 8) + buf[j]   * iEffVol) >> 8;
            dat[i+1] = buf[i+1] = ((dat[i+1] << 8) + buf[j+1] * iEffVol) >> 8;
        }

        return true;
    }
};








//------------------------------------------------------------
// モノラルコーラスエフェクター
//------------------------------------------------------------
class CMonoChorusEffect : public ISDLEffect
{
private:
    short   m_asBuffer[8192];
    int     m_iDelay;
    int     m_iModuleWidth;
    int     m_iModuleFreq;
    int     m_iFeadBack;
    short   m_fsPhase;      // （小数部8bitの固定小数）

public:
    CMonoChorusEffect()
    {
    }

public:
    // ディレイ量(1-2048)，変調幅(0-2048)，変調周波数(通常0-5000)，フィードバック量(0-128)
    bool Set(int iDelay, int iModulWidth, int iModulFreq, int iFeadBack)
    {
        // m_iDelay と m_iModuleWidth はデータ数で記憶，ステレオなのでx2
        m_iDelay       = iDelay * 2;
        m_iModuleWidth = iModulWidth * 2;
        m_iModuleFreq  = iModulFreq;
        m_iFeadBack    = iFeadBack;
        memset(m_asBuffer, 0, sizeof(short)*8192);

        return true;
    }

public:
    bool Init()
    {
        m_fsPhase = 0;
        return Set(1, 0, 0, 0);
    }

    bool Exec(Uint8* stream, int len)
    {
        short* buf = m_asBuffer;
        short* dat = (short*)stream;
        len >>= 1;

        m_fsPhase += m_iModuleFreq;

        int i, j;
        int iPhase     = (m_fsPhase * m_fsPhase) >> 16;
        int iDataDelay = len - (m_iDelay + ((m_iModuleWidth * iPhase) >> 14));
        int iEffVol    = m_iFeadBack;
        int iFilter    = len - 1;

        for (i=0; i<len; i+=2) {
            j = (i + iDataDelay) & iFilter;
            dat[i] = dat[i+1] = buf[i] = (((dat[i] + dat[i+1]) << 7) + buf[j] * iEffVol) >> 8;
        }

        return true;
    }
};








//------------------------------------------------------------
// フェードエフェクト
//------------------------------------------------------------
class CFaderEffect : public ISDLEffect
{
private:
    int     m_iDelayCounter;
    int     m_iFadeLenth;
    int     m_fiFadeT;
    int     m_fiFadeDT;

public:
    CFaderEffect()
    {
    }

public:
    // 遅延，効果時間（データサイズで指定）
    bool Set(bool bFadeIn, int iDelayLength, int iFadeLength)
    {
        m_iFadeLenth    = (iFadeLength == 0) ? 256 : iFadeLength;
        m_iDelayCounter = -iDelayLength;

        if (bFadeIn) {
            m_fiFadeT  = 0;
            m_fiFadeDT = ((128 << 16) / (m_iFadeLenth >> 8));
        } else {
            m_fiFadeT  = (128 << 16);
            m_fiFadeDT = -((128 << 16) / (m_iFadeLenth >> 8));
        }

        return true;
    }

public:
    bool Init()
    {
        return Set(true, 0, 0);
    }

    bool Exec(Uint8* stream, int len)
    {
        short* dat = (short*)stream;
        len >>= 1;

        int i, v;

        m_iDelayCounter += len;

        if (m_iDelayCounter > 0) {
            if (m_iDelayCounter < m_iFadeLenth) {
                // フェーディング中
                // フェーディングテーブルの作成
                char achFadeTable[64];
                int  imax = len >> 8;
                for (int i=0; i<imax; i++, m_fiFadeT+=m_fiFadeDT) achFadeTable[i] = (m_fiFadeT >> 16);

                for (i=0; i<len; i+=2) {
                    v = achFadeTable[i>>8];
                    dat[i]   = (dat[i]   * v) >> 7;
                    dat[i+1] = (dat[i+1] * v) >> 7;
                }
            } else {
                // フェーディング終了
                v = m_fiFadeT >> 16;
                for (i=0; i<len; i+=2) {
                    dat[i]   = (dat[i]   * v) >> 7;
                    dat[i+1] = (dat[i+1] * v) >> 7;
                }
            }
        }

        return true;
    }
};








//------------------------------------------------------------
// オートパンエフェクト
//------------------------------------------------------------
class CAutoPanEffect : public ISDLEffect
{
private:
    int     m_iWidth;
    int     m_iFreqency;
    short   m_fsPhase;      // （小数部8bitの固定小数）

public:
    CAutoPanEffect()
    {
    }

public:
    // 振幅(0-128)，周期(0-5000)
    bool Set(int iWidth, int iFreq)
    {
        m_iWidth    = 128 - (((128-iWidth) * (128-iWidth)) >> 7);
        m_iFreqency = iFreq;
        return true;
    }

public:
    bool Init()
    {
        m_fsPhase = 0;
        return Set(0, 0);
    }

    bool Exec(Uint8* stream, int len)
    {
        short* dat = (short*)stream;
        len >>= 1;

        m_fsPhase += m_iFreqency;

        int i, v;
        int iPhase = abs(m_fsPhase >> 8);
        int iLVol  = (iPhase * m_iWidth) >> 7;
        int iRVol  = ((128-iPhase) * m_iWidth) >> 7;
        iLVol = 128 - ((iLVol * iLVol) >> 7);
        iRVol = 128 - ((iRVol * iRVol) >> 7);

        for (i=0; i<len; i+=2) {
            v = dat[i] + dat[i+1];
            dat[i]   = (v * iLVol) >> 7;
            dat[i+1] = (v * iRVol) >> 7;
        }

        return true;
    }
};








//------------------------------------------------------------
// 周波数解析
//------------------------------------------------------------
class CFreqAnalysis : public ISDLEffect
{
private:
    float*  m_afFrequence;  // 周波数解析データ
    float*  m_afCosTable;   // Cos Table
    int     m_iTableSize;   // Table Size

public:
    CFreqAnalysis()
    {
    }

    ~CFreqAnalysis()
    {
        DestroyTable();
    }

public:
    // 周波数解析テーブルの生成
    bool Set(int iTableSize)
    {
        return CreateTable(iTableSize);
    }

private:
    // 周波数解析テーブル作成
    bool CreateTable(int iTableSize)
    {
        DestroyTable();

        m_afFrequence= (float*)malloc(sizeof(float) * iTableSize);
        m_afCosTable = (float*)malloc(sizeof(float) * iTableSize);
        if (m_afFrequence == NULL || m_afCosTable == NULL) {
            DestroyTable();
            return false;
        }

        m_iTableSize = iTableSize;
        float fInv = fRD / ((float)iTableSize);
        for (int i=0; i<iTableSize; i++) {
            m_afCosTable[i] = cosf(i*fInv);
        }

        return true;
    }

    // 周波数解析テーブル破棄
    void DestroyTable()
    {
        if (m_afFrequence != NULL) free(m_afFrequence);
        if (m_afCosTable  != NULL) free(m_afCosTable);
        m_afFrequence = NULL;
        m_afCosTable  = NULL;
        m_iTableSize  = 0;
    }

public:
    bool Init()
    {
        return true;
    }

    bool Exec(Uint8* stream, int len)
    {
        short* dat = (short*)stream;
        len >>= 1;

        int i, j;
        int iStep = len / m_iTableSize;

        for (i=0, j=0; i<m_iTableSize; i++, j+=iStep) {
            m_afFrequence[i] = (dat[j] + dat[j+1]) * 0.5f;
        }
        CalcDCT(m_iTableSize, m_afFrequence, m_afCosTable);
    }
};
