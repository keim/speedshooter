//--------------------------------------------------------------------------------
// 文字表示管理クラス 
//--------------------------------------------------------------------------------
class CScreenManager : public IManager
{
//--------------------------------------------------
// メンバ変数
//--------------------------------------------------
private:
    // 汎用アルファ値 
    float   m_fAlpha;

//--------------------------------------------------
// メンバ関数
//--------------------------------------------------
public:
    CScreenManager()
    {
    }

public:
    // メッセージ描画 
    void DrawMessage()
    {
        int i;
        float x, y;
        char* str;

        GL::SetAddBlend();
        y = 138;
        for (i=0; i<MESSAGE_MAX; i++) {
            str = SCORE.GetMessage(i);
            x = 310.0f - strlen(str) * 12.0f;
            RESOURCE.m_FontTable.Draw(x, y, 12, str);
            y -= 16.0;
        }
    }

    // 得点描画 
    void DrawScore(bool bHigh)
    {
        int   i, idx;
        char* str;

        glColor4f(1, 1, 1, 1);
        GL::SetAlphaBlend();

        // Score
        RESOURCE.m_DrawingSprite.SetTexRect(0.1875f, 0.75f, 0.25f, 0.0625f);
        RESOURCE.m_DrawingSprite.SetRect(-310, 200, 128, 32);
        RESOURCE.m_DrawingSprite.Draw();

        str = SCORE.GetScore(bHigh);
        for (i=0; i<8; i++) {
            idx = str[i] - '0';
            RESOURCE.m_DrawingSprite.SetTexRect(0.1875f+idx*0.0625f, 0.9375f, 0.0625f, 0.0625f);
            RESOURCE.m_DrawingSprite.SetRect((float)(i*21-316), 170, 32, 32);
            RESOURCE.m_DrawingSprite.Draw();
        }

        // Level
        RESOURCE.m_DrawingSprite.SetTexRect(0.1875f, 0.8125f, 0.25f, 0.0625f);
        RESOURCE.m_DrawingSprite.SetRect(-310, 130, 128, 32);
        RESOURCE.m_DrawingSprite.Draw();

        str = SCORE.GetLevel(bHigh);
        for (i=0; i<4; i++) {
            idx = str[i] - '0';
            RESOURCE.m_DrawingSprite.SetTexRect(0.1875f+idx*0.0625f, 0.9375f, 0.0625f, 0.0625f);
            RESOURCE.m_DrawingSprite.SetRect((float)(i*21-316), 100, 32, 32);
            RESOURCE.m_DrawingSprite.Draw();
        }
        
        // Max Chain
        RESOURCE.m_DrawingSprite.SetTexRect(0.1875f, 0.875f, 0.375f, 0.0625f);
        RESOURCE.m_DrawingSprite.SetRect(120, 200, 192, 32);
        RESOURCE.m_DrawingSprite.Draw();

        str = SCORE.GetMaxChain(bHigh);
        for (i=0; i<3; i++) {
            idx = str[i] - '0';
            RESOURCE.m_DrawingSprite.SetTexRect(0.1875f+idx*0.0625f, 0.9375f, 0.0625f, 0.0625f);
            RESOURCE.m_DrawingSprite.SetRect((float)(i*21+240), 170, 32, 32);
            RESOURCE.m_DrawingSprite.Draw();
        }
    }




//--------------------------------------------------
// コールバック
//--------------------------------------------------
// SCENE_TITLE
//------------------------------
protected:
    void DrawTITLE()
    {
        glColor4f(1, 1, 1, 1);
        
        // "SPEED"
        RESOURCE.m_DrawingSprite.SetTexRect(0.1875f, 0.5f, 0.6875f, 0.125f);
        RESOURCE.m_DrawingSprite.SetRect(-176, 80, 352, 64);
        RESOURCE.m_DrawingSprite.Draw();

        // "(C) Solid Image 2005"
        RESOURCE.m_DrawingSprite.SetTexRect(0.625f, 0.8125f, 0.375f, 0.0625f);
        RESOURCE.m_DrawingSprite.SetRect(100, -90, 192, 32);
        RESOURCE.m_DrawingSprite.Draw();

/*
        // "Push Button"
        RESOURCE.m_DrawingSprite.SetTexRect(0.625f, 0.625f, 0.375f, 0.0625f);
        RESOURCE.m_DrawingSprite.SetRect(-96, -20, 192, 32);
        RESOURCE.m_DrawingSprite.Draw();
*/

        glColor4f(1, 1, 1, 1);
        GL::SetAddBlend();
        RESOURCE.m_FontTable.Draw(-192, -16, 12, "PRESS KEY   -> PLAY WITH KEYBORAD");
        RESOURCE.m_FontTable.Draw(-192, -32, 12, "MOUSE CLICK -> PLAY WITH MOUSE");

        DrawScore(true);
    }

// SCENE_START
//------------------------------
protected:
    void RunSTART()
    {
        int cnt = RESOURCE.GetCounter();
        m_fAlpha = (cnt < 60) ? cnt * 0.01666667f : 1;
    }

    void DrawSTART()
    {
        int cnt = RESOURCE.GetCounter();
        
        if (cnt < 90) {
            // "Ready?" 
            RESOURCE.m_DrawingSprite.SetTexRect(0.625f, 0.6875f, 0.25f, 0.0625f);
            glColor4f(1, 1, 1, m_fAlpha*0.4f);
            RESOURCE.m_DrawingSprite.SetRect(-70+m_fAlpha*6, -22+m_fAlpha*6, 128, 32);
            RESOURCE.m_DrawingSprite.Draw();
            RESOURCE.m_DrawingSprite.SetRect(-70+m_fAlpha*6, -10-m_fAlpha*6, 128, 32);
            RESOURCE.m_DrawingSprite.Draw();
            RESOURCE.m_DrawingSprite.SetRect(-58-m_fAlpha*6, -22+m_fAlpha*6, 128, 32);
            RESOURCE.m_DrawingSprite.Draw();
            RESOURCE.m_DrawingSprite.SetRect(-58-m_fAlpha*6, -10-m_fAlpha*6, 128, 32);
            RESOURCE.m_DrawingSprite.Draw();
        } else {
            // "Go!!" 
            glColor4f(1, 1, 1, 1);
            RESOURCE.m_DrawingSprite.SetTexRect(0.625f, 0.75f, 0.1875f, 0.0625f);
            RESOURCE.m_DrawingSprite.SetRect(-48, -16, 96, 32);
            RESOURCE.m_DrawingSprite.Draw();
        }

        // "MOUSE MODE"
        if (RESOURCE.IsMouseMode()) {
            glColor4f(1, 1, 1, 1);
            GL::SetAddBlend();
            RESOURCE.m_FontTable.Draw(-84, -64, 12, "- MOUSE MODE -");
        }

        DrawScore(false);
    }

// SCENE_MAIN
//------------------------------
protected:
    void RunMAIN()
    {
    }

    void DrawMAIN()
    {
        DrawScore(false);
        DrawMessage();
    }

// SCENE_OVER
//------------------------------
protected:
    void RunOVER()
    {
        int cnt = RESOURCE.GetCounter();
        m_fAlpha = (cnt < 60) ? cnt * 0.01666667f : 1;
    }

    void DrawOVER()
    {
        int cnt = RESOURCE.GetCounter();
        
        // "GAME OVER" 
        RESOURCE.m_DrawingSprite.SetTexRect(0.1875f, 0.625f, 0.375f, 0.0625f);
        glColor4f(1, 1, 1, m_fAlpha*0.4f);
        RESOURCE.m_DrawingSprite.SetRect(-102+m_fAlpha*6, 66-m_fAlpha*6, 192, 32);
        RESOURCE.m_DrawingSprite.Draw();
        RESOURCE.m_DrawingSprite.SetRect(-102+m_fAlpha*6, 54+m_fAlpha*6, 192, 32);
        RESOURCE.m_DrawingSprite.Draw();
        RESOURCE.m_DrawingSprite.SetRect( -90-m_fAlpha*6, 66-m_fAlpha*6, 192, 32);
        RESOURCE.m_DrawingSprite.Draw();
        RESOURCE.m_DrawingSprite.SetRect( -90-m_fAlpha*6, 54+m_fAlpha*6, 192, 32);
        RESOURCE.m_DrawingSprite.Draw();

        if (cnt > 60) {
            // "Push Button"
            glColor4f(1, 1, 1, 1);
            RESOURCE.m_DrawingSprite.SetTexRect(0.625f, 0.625f, 0.375f, 0.0625f);
            RESOURCE.m_DrawingSprite.SetRect(-96, -62, 192, 32);
            RESOURCE.m_DrawingSprite.Draw();

            if (SCORE.IsHiScore()) {
                // "High Score !!" 
                glColor4f(1, 1, 1, 1);
                RESOURCE.m_DrawingSprite.SetTexRect(0.625f, 0.875f, 0.375f, 0.0625f);
                RESOURCE.m_DrawingSprite.SetRect(-96, -20, 192, 32);
                RESOURCE.m_DrawingSprite.Draw();
            }
        }

        DrawScore(false);
        DrawMessage();
    }
};


// インスタンス 
CScreenManager SCREEN;
