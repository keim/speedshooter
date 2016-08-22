//--------------------------------------------------------------------------------
//  Script Parser
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------

#ifndef _CORE_SIMPLESCRIPT_INCLUDED
#define _CORE_SIMPLESCRIPT_INCLUDED

#include <vector> 
#include <string>
#pragma message ("<< CORE_MESSAGE >>  include CSimpleScript")

namespace Util {
class CSimpleScript
{
private:
    FILE*               m_pFile;                // File
    int                 m_iLineCount;           // Open()してから，ReadLine()された回数
    int                 m_idx;                  // 文字列バッファのインデックス
   
    std::string         m_strFileName;          // File Name
    std::string         m_strFileDirectory;     // File が置かれているディレクトリパス
    
    std::string         m_strLine;              // 一行文字列バッファ
    std::vector<char>   m_achTokenPool;         // 切り出したトークン置場
    
    char                m_chSeparator;          // 改行文字
    std::string         m_strWhiteChar;         // 空白文字群
    std::string         m_strTokenChar;         // Token文字群
 
private:
    bool  DivScriptToLine();                            // Script -> m_strLine に分割 
    bool  SetTokenPool(const char* &ptr, int isize);    // m_achTokenPool に token を置く    

public:
    CSimpleScript();
    CSimpleScript(char* strFileName);
    ~CSimpleScript();

    // 設定
    void    SetSeparator(char  chSpe);              // 改行文字の設定 
    void    SetWhiteChar(char* ach);                // 空白文字群の設定 
    void    SetTokenChar(char* ach);                // Token文字群の設定 

    // 解析/操作
    bool    Open(char* strFileName);                // ファイル開く
    void    Close();                                // ファイル閉じる
    int     ReadLine();                             // １行解析（解析されたトークン数を返す）
    
    // データ参照 
    const char* m_strToken;                         // 現在参照しているトークンのポインタ 

    const char* GetHeadToken();                     // 最初のトークンを取得
    const char* GetNextToken();                     // 順次トークンを取得（ReadLine(),ResetSequence() 直後は，最初のトークンを取得） 
    const char* GetToken(int idx);                  // 任意インデックスのトークンを取得
    void        ResetSequence();                    // GetNextToken() の順次取得をリセット
    // GetNextToken + alpha
    const char* GetNextFloat(float* fv);            // atof(GetNextToken())
    const char* GetNextInt(int* iv);                // atoi(GetNextToken())
    const char* GetNextDWORD(unsigned long* dwv);   // strtoul(GetNextToken(), NULL, 16)
    const char* GetNextComp(char* strComp);         // (strcmp(GetNextToken(), strComp) == 0) ? strNextToken : NULL

    // ファイル終端に達した
    inline bool IsEOF() const
    {
        return (feof(m_pFile) != 0);
    }

    // ファイルポインタを返す
    inline FILE* GetFile() const
    {
        return m_pFile;
    }

    // ファイルの置かれているディレクトリパスを返す
    inline const char* GetFileDir() const
    {
        return m_strFileDirectory.data();
    }

    // 現在解析中の行番号を返す
    inline int GetLineCounter() const
    {
        return m_iLineCount;
    }

    // ReadLine 結果をそのまま返す
    inline const char* GetLine() const
    {
        return m_strLine.data();
    }
};

} // namespace Util

#endif  // _CORE_SIMPLESCRIPT_INCLUDED

