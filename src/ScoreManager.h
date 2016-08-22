#define HISCORE_FILE    "hiscore.dat"
#define HISCORE_HEADER  "SS02"
#define KEYCONFIG_FILE  "key.conf"
#define MESSAGE_MAX     16

#include <stdarg.h>
#include "util/FileIO.h"

//--------------------------------------------------------------------------------
// スコア管理クラス 
//--------------------------------------------------------------------------------
class CScoreManager : public IManager
{
//--------------------------------------------------
// メンバクラス 
//--------------------------------------------------
// ハイスコア
//------------------------------
private:
    struct HIGH_SCORE {
        char    achCheck[4];
        DWORD   dwHiScore;
        DWORD   dwHiScoreLevel;
        DWORD   dwHiScoreMaxChain;
        DWORD   dwHiLevel;
        DWORD   dwHiMaxChain;
    } m_HighScore;



//--------------------------------------------------
// メンバ変数
//--------------------------------------------------
private:
    DWORD   m_dwScore;          // スコア 
    DWORD   m_dwLevel;          // レベル 
    DWORD   m_dwMaxChain;       // 最大連鎖数

    // 一回の連鎖内
    DWORD   m_dwChain;          // 連鎖数 
    DWORD   m_dwDispCount;      // 消えたカード数 
    DWORD   m_dwChainScore;     // 連鎖総得点 

    int     m_iLevelCounter;    // レベル上昇用カウンタ 
    int     m_iChainCounter;    // 連鎖判定用カウンタ 
    DWORD   m_dwChainTable[4];  // 役判定用テーブル 

    // スクロールスピード 
    float   m_fScrollSpeed;

    // メッセージ 
    char    m_strMessage[MESSAGE_MAX][17];
    int     m_iLineIndex;
    
    // マウスボタン
    int     m_iClickedIndex;
    int     m_iDoubleClickCounter;
    bool    m_bDoubleClicked;
    
    // ナンバーキャラクタ 
    static char    s_achNumber[13];

    // 点数表 
    static DWORD   s_dwChainBonus[16];



//--------------------------------------------------
// メンバ関数 
//--------------------------------------------------
// コンストラクタ
//------------------------------
public:
    CScoreManager()
    {
    }

// ファイル
//------------------------------
public:
    bool Load()
    {
        if (CFileIO::Load(HISCORE_FILE, &m_HighScore, sizeof(HIGH_SCORE))) {
            if (memcmp(m_HighScore.achCheck, HISCORE_HEADER, 4) != 0) ResetAll();
        }
        CSDLCore::LoadKeyConfigFile(KEYCONFIG_FILE);
        return true;
    }

    bool Save()
    {
        CFileIO::Save(HISCORE_FILE, &m_HighScore, sizeof(HIGH_SCORE));
        CSDLCore::SaveKeyConfigFile(KEYCONFIG_FILE);
        return true;
    }


// 操作
//------------------------------
public:
    // アプリケーション開始時初期化 
    void ResetAll()
    {
        memcpy(m_HighScore.achCheck, HISCORE_HEADER, 4);
        m_HighScore.dwHiScore = 0;
        m_HighScore.dwHiScoreLevel = 0;
        m_HighScore.dwHiScoreMaxChain = 0;
        m_HighScore.dwHiLevel = 0;
        m_HighScore.dwHiMaxChain = 0;

        StartGame();
    }

    // ゲーム開始時初期化 
    void StartGame()
    {
        m_dwScore = 0;
        m_dwLevel = 0;
        m_dwMaxChain = 0;
        m_dwChain = 0;
        m_dwDispCount = 0;
        m_dwChainScore = 0;
        m_iLevelCounter = 0;
        m_iChainCounter = 0;
        m_fScrollSpeed = 0.001f;

        for (int i=0; i<MESSAGE_MAX; i++) m_strMessage[i][0] = '\0';
        ClearChainTable();
    }



private:
    // ハイスコアの更新 
    void UpdateHiScore()
    {
        if (IsHiScore()) {
            m_HighScore.dwHiScore = m_dwScore;
            m_HighScore.dwHiScoreLevel = m_dwLevel;
            m_HighScore.dwHiScoreMaxChain = m_dwMaxChain;
        }

        if (m_HighScore.dwHiLevel    < m_dwLevel)    m_HighScore.dwHiLevel    = m_dwLevel;
        if (m_HighScore.dwHiMaxChain < m_dwMaxChain) m_HighScore.dwHiMaxChain = m_dwMaxChain;
    }


// 参照 
//------------------------------
public:
    // スコア文字列取得
    char* GetScore(bool bHigh)
    {
        static char buf[9];
        sprintf(buf, "%08d", (bHigh) ? m_HighScore.dwHiScore : m_dwScore);
        return buf;
    }

    // レベル文字列取得
    char* GetLevel(bool bHigh)
    {
        static char buf[5];
        sprintf(buf, "%04d", (bHigh) ? m_HighScore.dwHiLevel : m_dwLevel);
        return buf;
    }

    // 最大連鎖数文字列取得
    char* GetMaxChain(bool bHigh)
    {
        static char buf[4];
        sprintf(buf, "%03d", (bHigh) ? (m_HighScore.dwHiMaxChain+1) : (m_dwMaxChain+1));
        return buf;
    }

    // 現在の連鎖数取得
    DWORD GetChain()
    {
        return m_dwChain+1;
    }

    // スクロール停止中（連鎖中）か？
    bool IsScrollStoped()
    {
        return (m_iChainCounter>=0);
    }

    // メッセージ取得 
    char* GetMessage(int line)
    {
        return m_strMessage[(line+m_iLineIndex) & (MESSAGE_MAX-1)];
    }

    // スクロールスピード取得 
    float GetScrollSpeed()
    {
        return m_fScrollSpeed;
    }

    // ハイスコアか？ 
    bool IsHiScore()
    {
        return (m_HighScore.dwHiScore < m_dwScore);
    }

// 操作 
//------------------------------
public:
    // 連鎖起動カードを放ったら呼び出す(Call from CCardManager::ShootCard())
    void ShootCard(int num, int sym)
    {
        m_dwChainTable[sym] |= 1 << num;
    }

    // カードが消えたら呼び出す(Call from CCardManager::PutCard())
    void DisappearCard(int num, int sym)
    {
        if (m_iChainCounter<0) {
            StartChain();
        } else if (m_iChainCounter < _CHAIN_INTERVAL-10) {
            Chaining();
        }

        m_iChainCounter  = _CHAIN_INTERVAL;
        m_iLevelCounter += _LEVEL_COUNT_ACCEL;
        m_dwDispCount++;
        AddChainBonus();

        // check flag table
        m_dwChainTable[sym] |= 1 << num;
    }

    // メッセージ表示
    void Message(char* strMessage, ...)
    {
        va_list arg;
        va_start(arg, strMessage);
        vsnprintf(m_strMessage[m_iLineIndex], 16, strMessage, arg);
        va_end(arg);

        m_iLineIndex = (m_iLineIndex+1) & (MESSAGE_MAX-1);
    }

// 操作 sub-routine 
//------------------------------
private:
    // 連鎖開始時に呼び出す(Call from DisappearCard())
    void StartChain()
    {
        m_dwChain = 0;
        m_dwDispCount = 0;
        m_dwChainScore = 0;
    }

    // 連鎖中に呼び出す(Call from DisappearCard())
    void Chaining()
    {
        m_dwChain++;
        if (m_dwMaxChain < m_dwChain) m_dwMaxChain = m_dwChain;
    }

    // 連鎖時に呼び出す(Call from RunMAIN()) 
    void EndChain()
    {
        int  i, cnt, cntmax;
        int  bonus;
        bool bRSF = false;
        bool bSuccess = false;

        // Over 5 Chain
        if (m_dwChain > 3) {
            Message("%d Chain +%d", m_dwChain+1, m_dwChainScore);
        }

        // Royal Straight Flash (Evaluation Only)
        for (i=0; i<4; i++) 
            if (CheckRoyalStraightFlash(i)) bRSF = true;

        // All Black/Red (5=+10000 /6=+15000/ 7=+20000 ...)
        if (m_dwDispCount > 4 && !bRSF) {
            bonus = (m_dwDispCount - 3) * 5000;
            if (CheckAllBlack()) {
                Message("# All Black    ");
                Message("+%d", bonus);
                m_dwScore += bonus;
                bSuccess = true;
            } else 
            if (CheckAllRed()) {
                Message("# All Red      ");
                Message("+%d", bonus);
                m_dwScore += bonus;
                bSuccess = true;
            }
        }

        // Royal Straight (+10000)
        // Ace to King    (+50000)
        if (m_dwDispCount > 4 && !bRSF) {
            if (CheckRoyalStraight()) {
                if (CheckOneToThirteen()) {
                    Message("# Ace to King  ");
                    Message("+50000");
                    m_dwScore += 50000;
                    bSuccess = true;
                } else {
                    Message("# RoyalStraight");
                    Message("+10000");
                    m_dwScore += 10000;
                    bSuccess = true;
                }
            }
        }

        // 4 of a kind (A=+30000 /2-9=+10000 /10-K=+20000)
        if (m_dwDispCount > 0) {
            for (i=0; i<13; i++) {
                if (CheckFourOfAKind(i)) {
                    bonus = (i<9) ? ((i==0) ? 30000 : 10000) : 20000;
                    Message("# FourOfAKind %c", s_achNumber[i]);
                    Message("+%d", bonus);
                    m_dwScore += bonus;
                    bSuccess = true;
                }
            }
        }

        // Straight Flash       (5=+50000  /6=+60000  /7=+70000  ...)
        // Royal Straight Flash (5=+100000 /6=+120000 /7=+140000 ...)
        if (m_dwDispCount > 4) {
            cntmax = 0;
            for (i=0; i<4; i++) {
                cnt = CheckStraightFlash(i);
                if (cntmax < cnt) cntmax = cnt;
            }
            if (cntmax != 0) {
                if (bRSF) {
                    bonus = cntmax * 20000;
                    Message("# Royal S.Flash");
                } else {
                    bonus = cntmax * 10000;
                    Message("# StraightFlash");
                }
                Message("+%d", bonus);
                m_dwScore += bonus;
                bSuccess = true;
            }
        }

        // Include some combinations
        if (bSuccess) {
            RESOURCE.Sound(SND_PIPIPI);
        }

        // Clear
        ClearChainTable();
    }

    // 連鎖ボーナス計算 
    void AddChainBonus()
    {
        int idx  = (m_dwChain > 15) ? 15 : m_dwChain;
        int base = (m_dwLevel / 10) * 10 + 50;
        m_dwScore      += s_dwChainBonus[idx] * base;
        m_dwChainScore += s_dwChainBonus[idx] * base;
    }

    // レベル上昇 
    void LevelUp()
    {
        m_iLevelCounter = 0;
        m_dwLevel++;
        m_fScrollSpeed = _SCROLL_SPEED(m_dwLevel);
    }

    // 役判定テーブルのクリア 
    void ClearChainTable()
    {
        m_dwChainTable[0] = 0;
        m_dwChainTable[1] = 0;
        m_dwChainTable[2] = 0;
        m_dwChainTable[3] = 0;
    }



// 役判定
//------------------------------
private:
    bool CheckAllBlack()
    {
        return ((m_dwChainTable[1] | m_dwChainTable[3]) == 0);
    }

    bool CheckAllRed()
    {
        return ((m_dwChainTable[0] | m_dwChainTable[2]) == 0);
    }

    bool CheckRoyalStraight()
    {
        DWORD dw = m_dwChainTable[0] | 
                   m_dwChainTable[1] | 
                   m_dwChainTable[2] | 
                   m_dwChainTable[3];
        return ((dw & 0x1e01) == 0x1e01);
    }

    int CheckOneToThirteen()
    {
        DWORD dw = m_dwChainTable[0] | 
                   m_dwChainTable[1] | 
                   m_dwChainTable[2] | 
                   m_dwChainTable[3];
        return (dw == 0x1fff);
    }

    bool CheckFourOfAKind(int num)
    {
        DWORD fil = 1<<num;
        DWORD dw  = (m_dwChainTable[0] & fil) +
                    (m_dwChainTable[1] & fil) +
                    (m_dwChainTable[2] & fil) +
                    (m_dwChainTable[3] & fil);
        return (dw == (fil << 2));
    }

    int CheckStraightFlash(int sym)
    {
        DWORD dw  = m_dwChainTable[sym] | (m_dwChainTable[sym]<<13);
        DWORD chk = 0x1f;
        int   i;
        int   cnt = 0;
        int   cntmax = 0;
        for (i=0; i<21; i++) {
            if ((dw & chk) == chk) cnt++;
            else {
                if (cntmax < cnt) cntmax = cnt;
                cnt = 0;
            }
            chk <<= 1;
        }
        if (cntmax < cnt) cntmax = cnt;
        return (cntmax == 0) ? 0 : cntmax+4;
    }

    bool CheckRoyalStraightFlash(int sym)
    {
        return ((m_dwChainTable[sym] & 0x1e01) == 0x1e01);
    }




// コントローラ関係 
//------------------------------
public:
    // マウスボタン情報更新用コールバック(Call from CMain::EventHandler())
    void MouseBottunPressed(int idx)
    {
        m_iClickedIndex = (idx == SDL_BUTTON_MIDDLE) ? idx : 0;
        m_bDoubleClicked |= IsClicking();
    }

    // マウスボタン情報更新用コールバック(Call from CMain::EventHandler())
    void MouseBottunReleased(int idx)
    {
        m_iClickedIndex = (idx != SDL_BUTTON_MIDDLE) ? idx : 0;
        m_iDoubleClickCounter = _DCLICK_INTERVAL;
    }
    
    // クリック中？
    bool IsClicking()
    {
        return (m_iDoubleClickCounter > 0);
    }
    
    // 中央マウスボタンクリック中？ 
    bool IsPressingMButton()
    {
        return (m_iClickedIndex == SDL_BUTTON_MIDDLE);
    }
    
    // 左マウスボタンクリックされた？ 
    bool IsLButtonClicked()
    {
        if (m_iClickedIndex != SDL_BUTTON_LEFT || m_iDoubleClickCounter > 0 || m_bDoubleClicked) return false;
        m_iClickedIndex = 0;
        return true;
    }

    // 右マウスボタンクリックされた？ 
    bool IsRButtonClicked()
    {
        if (m_iClickedIndex != SDL_BUTTON_RIGHT || m_iDoubleClickCounter > 0 || m_bDoubleClicked) return false;
        m_iClickedIndex = 0;
        return true;
    }    
    
    // 左マウスボタンダブルクリックされた？ 
    bool IsLButtonDClicked()
    {
        if (m_iClickedIndex != SDL_BUTTON_LEFT || !m_bDoubleClicked) return false;
        m_iClickedIndex = 0;
        m_bDoubleClicked = false;
        return true;
    }

    // 右マウスボタンダブルクリックされた？ 
    bool IsRButtonDClicked()
    {
        if (m_iClickedIndex != SDL_BUTTON_RIGHT || !m_bDoubleClicked) return false;
        m_iClickedIndex = 0;
        m_bDoubleClicked = false;
        return true;
    }    

private:
    // マウスボタンメッセージキューのクリア
    void ClearMouseButtonQue()
    {
        m_iClickedIndex = 0;
        m_bDoubleClicked = false;
        m_iDoubleClickCounter = 0;
    }

    // どれかボタンが押されたか？ 
    bool IsButtonHitted()
    {
        return CSDLCore::IsHitted(0, SKC_BTN0) |
               CSDLCore::IsHitted(0, SKC_BTN1) |
               CSDLCore::IsHitted(0, SKC_BTN2) |
               CSDLCore::IsHitted(0, SKC_BTN3) |
               CSDLCore::IsHitted(0, SKC_BTN4);
    }

    // どれかマウスクリックされたか？ 
    bool IsMouseClisked()
    {
        return IsLButtonClicked() || IsRButtonClicked();
    }

    //  エスケープボタンのチェック 
    void CheckEscape()
    {
        if (CSDLCore::IsHitted(0, SKC_BTN5)) {
            if (CSDLCore::GetScene() == SCENE_TITLE) CSDLCore::ExitMainLoop(0);
            else                                     CSDLCore::ChangeScene(SCENE_TITLE);
        }
    }


//--------------------------------------------------
// コールバック
//--------------------------------------------------
public:
    bool Init()
    {
        ResetAll();
        Load();
        return true;
    }

    bool InitScene()
    {
        IManager::InitScene();
        ClearMouseButtonQue();
        return true;
    }

    bool Run()
    {
        IManager::Run();
        CheckEscape();
        m_iDoubleClickCounter--;
        return true;
    }

    bool Exit()
    {
        Save();
        return true;
    }
    


// SCENE_TITLE
//------------------------------
protected:
    void InitTITLE()
    {
        StartGame();
    }
    
    void RunTITLE()
    {
        if (IsButtonHitted()) {
            RESOURCE.SetMouseMode(false);
            CSDLCore::ChangeScene(SCENE_START);
        } else
        if (IsMouseClisked()) {
            RESOURCE.SetMouseMode(true);
            CSDLCore::ChangeScene(SCENE_START);
        }
    }


// SCENE_START
//------------------------------
protected:
    void InitSTART()
    {
        RESOURCE.Sound(SND_PARARA);
    }
    
    void RunSTART()
    {
        if (RESOURCE.GetCounter() >= 120) CSDLCore::ChangeScene(SCENE_MAIN);
    }


// SCENE_MAIN
//------------------------------
protected:    
    void RunMAIN()
    {
        m_iChainCounter--;
        if (m_iChainCounter == 0) EndChain();
        
        m_iLevelCounter++;
        if (m_iLevelCounter >= _LEVELUP_INTERVAL) LevelUp();
    }


// SCENE_OVER
//------------------------------
protected:
    void RunOVER()
    {
        if (RESOURCE.GetCounter() > 60 && (IsButtonHitted() || IsMouseClisked())) {
            CSDLCore::ChangeScene(SCENE_TITLE);
        }
    }
    
    void ExitOVER()
    {
        UpdateHiScore();
    }
};

// スタティックメンバ
char    CScoreManager::s_achNumber[13] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 127, 'J', 'Q', 'K'};
DWORD   CScoreManager::s_dwChainBonus[16] = {1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120, 150};


// インスタンス 
CScoreManager SCORE;

