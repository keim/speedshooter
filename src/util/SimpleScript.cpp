//--------------------------------------------------------------------------------
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
#include "std_include.h"
#include "SimpleScript.h"
#include "Memory.h"

namespace Util {
    
// �R���X�g���N�^ 
//------------------------------------------------------------
CSimpleScript::CSimpleScript()
{
    m_pFile = NULL;
    SetSeparator('\n');
    SetWhiteChar(" \t\r");
}

CSimpleScript::CSimpleScript(char* strFileName)
{
    m_pFile = NULL;
    SetSeparator('\n');
    SetWhiteChar(" \t\r");
    Open(strFileName);
}

CSimpleScript::~CSimpleScript()
{
    Close();
}

// �ݒ� 
//------------------------------------------------------------
void CSimpleScript::SetSeparator(char chSep)
{
    m_chSeparator = chSep;
}
 
void CSimpleScript::SetWhiteChar(char* ach)
{
    m_strWhiteChar = ach;
    m_strWhiteChar += m_chSeparator;
}

void CSimpleScript::SetTokenChar(char* ach)
{
    m_strTokenChar = ach;
}

// ���� 
//------------------------------------------------------------
// �t�@�C�����J��
bool CSimpleScript::Open(char* strFileName)
{
    m_pFile = fopen(strFileName, "rt");
    if (m_pFile == NULL) return false;

    // �t�@�C����
    m_strFileName = strFileName;
    std::string::size_type idx = m_strFileName.rfind('\\');

    // �t�@�C���f�B���N�g�� 
    if (idx != std::string::npos) m_strFileDirectory = m_strFileName.substr(0, idx);
    else m_strFileDirectory.clear();

    m_iLineCount = 0;

    return true;
}

// �t�@�C�������
void CSimpleScript::Close()
{
    if (m_pFile != NULL) fclose(m_pFile);
    m_pFile = NULL;
}

// 1�s�ǂ݂���
int CSimpleScript::ReadLine()
{
    if (m_pFile == NULL) return -1;     // �G���[

    // 1�s�ǂ݂���
    if (!DivScriptToLine()) return -1;  // �I�� 
    m_iLineCount++;

    int idx, isize, iTokenCount;
    const char* ptr = m_strLine.data();

    m_idx = 0;
    m_achTokenPool.clear();
    iTokenCount = 0;

    while (1) {
        ptr  += strspn(ptr, m_strWhiteChar.data());     // White Space �̃X�L�b�v
        if (*ptr == '\0') break;                        // �I��

        isize = strcspn(ptr, m_strWhiteChar.data());    // White Space �܂ł̒���
        idx   = strcspn(ptr, m_strTokenChar.data());    // Single Token �̃C���f�b�N�X
        
        if (idx == 0) {                                 // Single Token �̐؏o��
            if (!SetTokenPool(ptr, 1)) return -1;
        } else {                                        // Token �̐؏o��
            if (!SetTokenPool(ptr, __min(idx, isize))) return -1;
        }

        iTokenCount++;
    }

    m_strToken = NULL;
    return iTokenCount;
}

// sub routine
//------------------------------------------------------------
// 1�s�ǂݍ��� 
bool CSimpleScript::DivScriptToLine()
{
    if (IsEOF()) return -1;
    
    int i;
    
    m_strLine.clear();
    while ((i = fgetc(m_pFile)) != EOF) {
        if (Util::IsSJISTop(i)) {
            m_strLine += (char)i;
            i = fgetc(m_pFile);
            m_strLine += (char)i;
        } else {
            if (i == m_chSeparator) break;
            m_strLine += (char)i;
        }
    }

    return (i != EOF);
} 

bool CSimpleScript::SetTokenPool(const char* &ptr, int isize)
{
    m_achTokenPool.resize(m_idx + isize + 2);

    strncpy(&(m_achTokenPool[m_idx]), ptr, isize);
    m_idx += isize;
    ptr += isize;
    m_achTokenPool[m_idx++] = '\0';
    m_achTokenPool[m_idx]   = '\0';
    
    return true;
}

// ���Q�� 
//------------------------------------------------------------
// �ŏ��̃g�[�N�����擾
const char* CSimpleScript::GetHeadToken()
{
    m_strToken = &(m_achTokenPool[0]); 
    return m_strToken;
}

// �����g�[�N�����擾�iReadLine(),ResetSequence() ����́C�ŏ��̃g�[�N�����擾
const char* CSimpleScript::GetNextToken()
{
    if (m_strToken == NULL) return GetHeadToken(); 
    m_strToken = &(m_strToken[strlen(m_strToken)+1]);
    if (*m_strToken == '\0') m_strToken = NULL;
    return m_strToken;
}

// GetNextToken() �̏����擾�����Z�b�g
void CSimpleScript::ResetSequence()
{
    m_strToken = NULL;
}

// atof(GetNextToken())
const char* CSimpleScript::GetNextFloat(float* pf)
{
    const char* ptrNext = GetNextToken();
    *pf = (ptrNext != NULL) ? ((float)atof(ptrNext)) : 0.0f;
    return ptrNext;
}

// atoi(GetNextToken())
const char* CSimpleScript::GetNextInt(int* pi)
{
    const char* ptrNext = GetNextToken();
    *pi = (ptrNext != NULL) ? (atoi(ptrNext)) : 0;
    return ptrNext;
}

// strtoul(GetNextToken(), NULL, 16)
const char* CSimpleScript::GetNextDWORD(unsigned long* pdw)
{
    const char* ptrNext = GetNextToken();
    *pdw = (ptrNext != NULL) ? (strtoul(ptrNext, NULL, 16)) : 0;
    return ptrNext;
}

// (strcmp(GetNextToken(), strComp) == 0) ? strNextToken : NULL
const char* CSimpleScript::GetNextComp(char* strComp)
{
    const char* ptrNext = GetNextToken();
    if (ptrNext == NULL) return NULL;
    return (strcmp(ptrNext, strComp) != 0) ? NULL : ptrNext;
}

// �C���f�b�N�X�Ńg�[�N���擾
const char* CSimpleScript::GetToken(int idx)
{
    const char* ptrReturn = GetHeadToken();
    for (int i=0; i!=idx; i++) {
        ptrReturn = GetNextToken();
        if (ptrReturn == NULL) return NULL;
    }
    return ptrReturn;
}


} // namespace Util
