//--------------------------------------------------------------------------------
//  Text Screen Class
//    Copyright  (C)  2006  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// class CSDLTextScreen;  
//   最前面にテキストを表示するクラス
//--------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------
#include <stdarg.h>

class CSDLTextScreen : public IThread
{
protected:
    // フォント
    GL::CCharTable      m_FontTable;
    GL::CTexture        m_FontTexture;

    // 画面情報 
    int                 m_iPosX;
    int                 m_iPosY;
    int                 m_iWidth;
    int                 m_iHeight;
    int                 m_iCursorX;
    int                 m_iCursorY;

    // 描画 
    void                (*m_funcBlend)();
    std::vector<char>   m_aBuffer;
        
    
public:
    CSDLTextScreen()
    {
        m_iPosX = 0;
        m_iPosY = 0;
        m_iWidth = 0;
        m_iHeight = 0;
        m_iCursorX = 0;
        m_iCursorY = 0;
        m_funcBlend = GL::SetAddBlend;
    }

//--------------------------------------------------
// 設定 
//--------------------------------------------------
public:
    // 画面位置/サイズ設定
    inline void SetScreen(int x, int y, int cx, int cy)
    {
        m_iCursorX = 0;
        m_iCursorY = 0;
        m_iPosX = x - CSDLCore::GetScreenWidth() / 2;
        m_iPosY = y + CSDLCore::GetScreenHeight() / 2 - (int)(m_FontTable.m_vcCharWidth.y);
        m_iWidth  = cx;
        m_iHeight = cy;
        m_aBuffer.resize(cx * cy);
    }

    // スクロール
    inline void Scroll(int iLine)
    {
        char* src  = &(m_aBuffer[iLine * m_iWidth]);
        char* dst  = &(m_aBuffer[0]);
        int imax, i;

        // コピー
        imax  = m_iHeight - iLine;
        for (i=0; i<imax; i++) {
            memcpy(dst, src, m_iWidth);
            src = &(src[m_iWidth]);
            dst = &(dst[m_iWidth]);
        }
        for (i=imax; i<m_iHeight; i++) {
            memset(dst, 0, m_iWidth);
            dst = &(dst[m_iWidth]);
        }
    
        // カーソル移動 
        m_iCursorY -= iLine;
        if (m_iCursorY < 0) m_iCursorY = 0;
    }

    // フォント設定（透過色指定） 
    inline bool LoadFont(const char* strFileName, DWORD dwTransColor, float char_w, float char_h, int char_offset)
    {
        if (!m_FontTexture.Load(strFileName, dwTransColor)) return false;

        m_funcBlend = GL::SetAlphaBlend;
        m_FontTexture.SetParameters(false, false, false, false, false);
        m_FontTexture.Register();
        m_FontTable.SetTexture(&m_FontTexture, char_w, char_h, char_offset);

        return true;
    }

    // フォント設定
    inline bool LoadFont(const char* strFileName, float char_w, float char_h, int char_offset)
    {
        if (m_FontTexture.Load(strFileName, 4)) return false;
        m_funcBlend = GL::SetAddBlend;

        m_FontTexture.SetParameters(false, false, false, false, false);
        m_FontTexture.Register();
        m_FontTable.SetTexture(&m_FontTexture, char_w, char_h, char_offset);
        
        return true;
    }
    
    // 画面未設定 
    inline bool IsEmpty()
    {
        return (m_iWidth == 0 && m_iHeight == 0);
    }

protected:
    // カーソル位置計算 (xy->index)
    inline int GetCursorIndex()
    {
        return (m_iCursorY * m_iWidth + m_iCursorX);
    }
    
    // カーソル位置計算 (index->xy)
    inline void SetCursorPosition(int idx)
    {
        m_iCursorY = idx / m_iWidth;
        m_iCursorX = idx - m_iCursorY * m_iWidth;
    }

    
//--------------------------------------------------
// 記述 
//--------------------------------------------------
public:
    // Clear Screen
    inline void Cls()
    {
        memset(&(m_aBuffer[0]), 0, m_aBuffer.size());
        m_iCursorX = 0;
        m_iCursorY = 0;
    }
    
    // LOCATE 
    inline void Locate(int x, int y)
    {
        m_iCursorX = x;
        m_iCursorY = y;
    }

    // PRINT
    inline void Print(const char* str)
    {
        int len = strlen(str);
        int idx = GetCursorIndex();
        
        if (idx + len > m_aBuffer.size()) {
            Scroll((idx + len - m_aBuffer.size()) / m_iWidth + 1);
            idx = GetCursorIndex();
        }
        
        strcpy(&(m_aBuffer[idx]), str);
        idx += len;
        
        SetCursorPosition(idx);
    }

    // PRINTF 
    inline void Printf(const char* strForm, ...)
    {
        va_list arg;
        va_start(arg, strForm);
        VPrintf(strForm, arg);
        va_end(arg);
    }

    // 改行 
    inline void LineFeed()
    {
        m_iCursorY++;
        m_iCursorX = 0;
        if (m_iCursorY >= m_iHeight) {
            Scroll(m_iCursorY - m_iHeight + 1);
        }
    }

protected:
    // VPRINTF 
    inline void VPrintf(const char* strForm, va_list args)
    {
        char buf[257];
        vsnprintf(buf, 256, strForm, args);
        Print(buf);
    }
    
//--------------------------------------------------
// 実行
//--------------------------------------------------
public:
    virtual bool Draw()
    {
        m_funcBlend();
        
        int i, idx, y, yinc;
    
        idx = 0;
        y   = m_iPosY;
        yinc = (int)(m_FontTable.m_vcCharWidth.y);
        
        for (i=0; i<m_iHeight; i++) {
            m_FontTable.Draw(m_iPosX, y, 6, &(m_aBuffer[idx]), m_iWidth);
            idx += m_iWidth;
            y   -= yinc;
        }

        return true;
    }
};
