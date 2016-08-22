//--------------------------------------------------------------------------------
//  Map Class
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// class CKeywordMap;  簡易コマンド管理クラス（文字列とDataの紐付け）
//   RegData() で登録／GetData() で呼び出し／Clear() で全消去
// class CIDManager;   簡易ID管理クラス（文字列にuniqueなint値を割り当てる）
//   GetID() でIDを"0"から取得(return 0xffffffff;でエラー)／Clear() で全消去
//--------------------------------------------------------------------------------
#ifndef _CORE_COMMANDMANAGER_INCLUDED
#define _CORE_COMMANDMANAGER_INCLUDED

#include "SimpleScript.h"
#pragma message ("<< CORE_MESSAGE >>  include CCommandManager")

#define _KEYMAP_BUFFER_SIZE		1024

namespace Util {
//--------------------------------------------------------------------------------
// class CKeywordMap;  簡易コマンド管理クラス（文字列とDataの紐付け）
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

    // 全消去
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

    // 文字列 strKeyword とそれに関連したデータ void* pData を紐付けする
    void* RegData(char* strKeyword, void* pData, int iDataSize)
    {
        int len = strlen(strKeyword) + 1;
        int szTotal = len + sizeof(short) + iDataSize;
        int idx = m_iTailIndex;
        if (idx == -1 || idx + szTotal >= m_iSize) return NULL;

        // strKeyword + '\0' 書き込み
        strcpy(&(m_achMapBuffer[idx]), strKeyword);
        idx += len;
        // iParam 書き込み/ pParam 書き込み
        *(short*)(&(m_achMapBuffer[idx])) = iDataSize;
    	idx += sizeof(short);
        memcpy(&(m_achMapBuffer[idx]), pData, iDataSize);
        m_iTailIndex = idx + iDataSize;
        return &(m_achMapBuffer[idx]);
    }

    // 文字列 strKeyword に紐付けされたデータ を返す．piSize にポインタ渡しでサイズを返す．
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
// class CIDManager;  簡易ID管理クラス（文字列にuniqueなint値を割り当てる）
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

    // 全消去
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

    // 文字列 strKeyword に紐付けされたデータ を返す．piSize にポインタ渡しでサイズを返す．
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