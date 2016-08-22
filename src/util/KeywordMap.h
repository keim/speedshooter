//--------------------------------------------------------------------------------
//  Map Class
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// class CKeywordMap;  �ȈՃR�}���h�Ǘ��N���X�i�������Data�̕R�t���j
//   RegData() �œo�^�^GetData() �ŌĂяo���^Clear() �őS����
// class CIDManager;   �Ȉ�ID�Ǘ��N���X�i�������unique��int�l�����蓖�Ă�j
//   GetID() ��ID��"0"����擾(return 0xffffffff;�ŃG���[)�^Clear() �őS����
//--------------------------------------------------------------------------------
#ifndef _CORE_COMMANDMANAGER_INCLUDED
#define _CORE_COMMANDMANAGER_INCLUDED

#include "SimpleScript.h"
#pragma message ("<< CORE_MESSAGE >>  include CCommandManager")

#define _KEYMAP_BUFFER_SIZE		1024

namespace Util {
//--------------------------------------------------------------------------------
// class CKeywordMap;  �ȈՃR�}���h�Ǘ��N���X�i�������Data�̕R�t���j
//--------------------------------------------------------------------------------
class CKeywordMap
{
private:
    char*  m_achMapBuffer;
	int	   m_iSize;
    int    m_iTailIndex;

public:
    CKeywordMap()
    {
		m_achMapBuffer = NULL;
		m_iSize = 0;
        Clear(_KEYMAP_BUFFER_SIZE);
    }

    CKeywordMap(int size)
    {
		m_achMapBuffer = NULL;
		m_iSize = 0;
        Clear(size);
    }

	~CKeywordMap()
	{
		memDelete(m_achMapBuffer);
		m_iSize = 0;
	}

    // �S����
    void Clear(int AllocSize)
    {
		if (memLength(m_achMapBuffer) != AllocSize) {
			memDelete(m_achMapBuffer);
			m_achMapBuffer = memCreate(char, AllocSize);
		} else {
			memset(m_achMapBuffer, 0, AllocSize);
		}
		m_iSize = AllocSize;
        m_iTailIndex = 0;
    }

    // ������ strKeyword �Ƃ���Ɋ֘A�����f�[�^ void* pData ��R�t������
    void* RegData(char* strKeyword, void* pData, int iDataSize)
    {
        int len = strlen(strKeyword) + 1;
        int szTotal = len + sizeof(short) + iDataSize;
        int idx = m_iTailIndex;
        if (idx == -1 || idx + szTotal >= m_iSize) return NULL;

        // strKeyword + '\0' ��������
        strcpy(&(m_achMapBuffer[idx]), strKeyword);
        idx += len;
        // iParam ��������/ pParam ��������
        *(short*)(&(m_achMapBuffer[idx])) = iDataSize;
    	idx += sizeof(short);
        memcpy(&(m_achMapBuffer[idx]), pData, iDataSize);
        m_iTailIndex = idx + iDataSize;
        return &(m_achMapBuffer[idx]);
    }

    // ������ strKeyword �ɕR�t�����ꂽ�f�[�^ ��Ԃ��DpiSize �Ƀ|�C���^�n���ŃT�C�Y��Ԃ��D
    void* GetData(char* strKeyword, int* piSize=NULL)
    {
        int idx = 0;
        int datasize;
        while (idx < m_iSize) {
            if (stricmp(strKeyword, &(m_achMapBuffer[idx])) == 0) {
                idx += strlen(&(m_achMapBuffer[idx])) + 1;
                datasize = *(short*)(&(m_achMapBuffer[idx]));
                if (piSize != NULL) *piSize = datasize;
                return &(m_achMapBuffer[idx+sizeof(short)]);
            }
            idx += strlen(&(m_achMapBuffer[idx])) + 1;
            datasize = *(short*)(&(m_achMapBuffer[idx]));
            idx += datasize + sizeof(short);
			if (m_achMapBuffer[idx] == '\0') break;
        }
		return NULL;
    }
};

//--------------------------------------------------------------------------------
// class CIDManager;  �Ȉ�ID�Ǘ��N���X�i�������unique��int�l�����蓖�Ă�j
//--------------------------------------------------------------------------------
class CIDManager
{
private:
    char*  m_achMapBuffer;
	int	   m_iSize;
    int    m_NextID;

public:

    CIDManager()
    {
		m_achMapBuffer = NULL;
		m_iSize = 0;
        Clear(_KEYMAP_BUFFER_SIZE);
    }

    CIDManager(int size)
    {
		m_achMapBuffer = NULL;
		m_iSize = 0;
        Clear(size);
    }

	~CIDManager()
	{
		memDelete(m_achMapBuffer);
		m_iSize = 0;
	}

    // �S����
    void Clear(int AllocSize)
    {
		if (memLength(m_achMapBuffer) != AllocSize) {
			memDelete(m_achMapBuffer);
			m_achMapBuffer = memCreate(char, AllocSize);
		} else {
			memset(m_achMapBuffer, 0, AllocSize);
		}
		m_iSize = AllocSize;
        m_NextID = -1;
    }

    // ������ strKeyword �ɕR�t�����ꂽ�f�[�^ ��Ԃ��DpiSize �Ƀ|�C���^�n���ŃT�C�Y��Ԃ��D
    int GetID(char* strID)
    {
		char* pIDString;
        for (int i=0; i<m_iSize; i+=4) {
            if (m_achMapBuffer[i] == '\0') {
                if (i+strlen(strID)+4 > (unsigned int)m_iSize) break;
                strcpy(&m_achMapBuffer[i], strID);
	            i += strlen(strID) + 1;
                *(int*)(&(m_achMapBuffer[i])) = ++m_NextID;
                return m_NextID;
            }
			pIDString = &(m_achMapBuffer[i]);
            i += strlen(&(m_achMapBuffer[i])) + 1;
            if (stricmp(strID, pIDString) == 0) return *(int*)(&(m_achMapBuffer[i]));
        }
		return -1;
    }
};
}

#endif // _CORE_COMMANDMANAGER_INCLUDED