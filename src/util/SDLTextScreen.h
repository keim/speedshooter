//--------------------------------------------------------------------------------
//  Text Screen Class
//    Copyright  (C)  2006  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// class CSDLTextScreen;  
//   �őO�ʂɃe�L�X�g��\������N���X
//--------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------
#include <stdarg.h>

class CSDLTextScreen : public IThread
{
protected:
    // �t�H���g
    GL::CCharTable      m_FontTable;
    GL::CTexture        m_FontTexture;

    // ��ʏ�� 
    int                 m_iPosX;
    int                 m_iPosY;
    int                 m_iWidth;
    int                 m_iHeight;
    int                 m_iCursorX;
    int                 m_iCursorY;

    // �`�� 
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
// �ݒ� 
//--------------------------------------------------
public:
    // ��ʈʒu/�T�C�Y�ݒ�
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

    // �X�N���[��
    inline void Scroll(int iLine)
    {
        char* src  = &(m_aBuffer[iLine * m_iWidth]);
        char* dst  = &(m_aBuffer[0]);
        int imax, i;

        // �R�s�[
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
    
        // �J�[�\���ړ� 
        m_iCursorY -= iLine;
        if (m_iCursorY < 0) m_iCursorY = 0;
    }

    // �t�H���g�ݒ�i���ߐF�w��j 
    inline bool LoadFont(const char* strFileName, DWORD dwTransColor, float char_w, float char_h, int char_offset)
    {
        if (!m_FontTexture.Load(strFileName, dwTransColor)) return false;

        m_funcBlend = GL::SetAlphaBlend;
        m_FontTexture.SetParameters(false, false, false, false, false);
        m_FontTexture.Register();
        m_FontTable.SetTexture(&m_FontTexture, char_w, char_h, char_offset);

        return true;
    }

    // �t�H���g�ݒ�
    inline bool LoadFont(const char* strFileName, float char_w, float char_h, int char_offset)
    {
        if (m_FontTexture.Load(strFileName, 4)) return false;
        m_funcBlend = GL::SetAddBlend;

        m_FontTexture.SetParameters(false, false, false, false, false);
        m_FontTexture.Register();
        m_FontTable.SetTexture(&m_FontTexture, char_w, char_h, char_offset);
        
        return true;
    }
    
    // ��ʖ��ݒ� 
    inline bool IsEmpty()
    {
        return (m_iWidth == 0 && m_iHeight == 0);
    }

protected:
    // �J�[�\���ʒu�v�Z (xy->index)
    inline int GetCursorIndex()
    {
        return (m_iCursorY * m_iWidth + m_iCursorX);
    }
    
    // �J�[�\���ʒu�v�Z (index->xy)
    inline void SetCursorPosition(int idx)
    {
        m_iCursorY = idx / m_iWidth;
        m_iCursorX = idx - m_iCursorY * m_iWidth;
    }

    
//--------------------------------------------------
// �L�q 
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

    // ���s 
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
// ���s
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
