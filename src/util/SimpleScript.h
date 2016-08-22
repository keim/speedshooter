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
    int                 m_iLineCount;           // Open()���Ă���CReadLine()���ꂽ��
    int                 m_idx;                  // ������o�b�t�@�̃C���f�b�N�X
   
    std::string         m_strFileName;          // File Name
    std::string         m_strFileDirectory;     // File ���u����Ă���f�B���N�g���p�X
    
    std::string         m_strLine;              // ��s������o�b�t�@
    std::vector<char>   m_achTokenPool;         // �؂�o�����g�[�N���u��
    
    char                m_chSeparator;          // ���s����
    std::string         m_strWhiteChar;         // �󔒕����Q
    std::string         m_strTokenChar;         // Token�����Q
 
private:
    bool  DivScriptToLine();                            // Script -> m_strLine �ɕ��� 
    bool  SetTokenPool(const char* &ptr, int isize);    // m_achTokenPool �� token ��u��    

public:
    CSimpleScript();
    CSimpleScript(char* strFileName);
    ~CSimpleScript();

    // �ݒ�
    void    SetSeparator(char  chSpe);              // ���s�����̐ݒ� 
    void    SetWhiteChar(char* ach);                // �󔒕����Q�̐ݒ� 
    void    SetTokenChar(char* ach);                // Token�����Q�̐ݒ� 

    // ���/����
    bool    Open(char* strFileName);                // �t�@�C���J��
    void    Close();                                // �t�@�C������
    int     ReadLine();                             // �P�s��́i��͂��ꂽ�g�[�N������Ԃ��j
    
    // �f�[�^�Q�� 
    const char* m_strToken;                         // ���ݎQ�Ƃ��Ă���g�[�N���̃|�C���^ 

    const char* GetHeadToken();                     // �ŏ��̃g�[�N�����擾
    const char* GetNextToken();                     // �����g�[�N�����擾�iReadLine(),ResetSequence() ����́C�ŏ��̃g�[�N�����擾�j 
    const char* GetToken(int idx);                  // �C�ӃC���f�b�N�X�̃g�[�N�����擾
    void        ResetSequence();                    // GetNextToken() �̏����擾�����Z�b�g
    // GetNextToken + alpha
    const char* GetNextFloat(float* fv);            // atof(GetNextToken())
    const char* GetNextInt(int* iv);                // atoi(GetNextToken())
    const char* GetNextDWORD(unsigned long* dwv);   // strtoul(GetNextToken(), NULL, 16)
    const char* GetNextComp(char* strComp);         // (strcmp(GetNextToken(), strComp) == 0) ? strNextToken : NULL

    // �t�@�C���I�[�ɒB����
    inline bool IsEOF() const
    {
        return (feof(m_pFile) != 0);
    }

    // �t�@�C���|�C���^��Ԃ�
    inline FILE* GetFile() const
    {
        return m_pFile;
    }

    // �t�@�C���̒u����Ă���f�B���N�g���p�X��Ԃ�
    inline const char* GetFileDir() const
    {
        return m_strFileDirectory.data();
    }

    // ���݉�͒��̍s�ԍ���Ԃ�
    inline int GetLineCounter() const
    {
        return m_iLineCount;
    }

    // ReadLine ���ʂ����̂܂ܕԂ�
    inline const char* GetLine() const
    {
        return m_strLine.data();
    }
};

} // namespace Util

#endif  // _CORE_SIMPLESCRIPT_INCLUDED

