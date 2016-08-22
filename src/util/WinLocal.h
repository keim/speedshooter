//--------------------------------------------------------------------------------
//  Windows Local Sub Routines
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
#ifndef _CORE_WINLOCAL_INCLUDED
#define _CORE_WINLOCAL_INCLUDED

#include <windows.h>
#pragma comment (lib, "winmm.lib")
#include <mmsystem.h>

//--------------------------------------------------------------------------------
//  IsSuported(DWORD dwCheckSuport)
//  CPU �̋@�\�`�F�b�N;
//  �����G  CHECK_MMX, CHECK_SSE, CHECK_SSE2, CHECK_3DNOW, CHECK_E3DNOW
//--------------------------------------------------------------------------------
#define cpuid __asm __emit 0fh __asm __emit 0a2h
#define CHECK_MMX       0x00800000
#define CHECK_SSE       0x02000000
#define CHECK_SSE2      0x04000000
#define CHECK_3DNOW     0x40000000
#define CHECK_E3DNOW    0x80000000
inline bool IsSuported(DWORD dwCheckSuport)
{
/*
#ifdef _WIN32
    bool    bCheck = 0;
    __asm{
        mov     eax, 0
        cpuid
        cmp     eax, 0
        jz      non_mmx         //cpuid ���T�|�[�g
        mov     eax, 1
        cpuid
        and     edx, dwCheckSuport
        jz      non_mmx         //dwCheckSuport ���T�|�[�g
        mov     bCheck, 1
    non_mmx:
    }
    return bCheck;
#else
	return 0;
#endif
*/
	return 0;
}

//--------------------------------------------------------------------------------
//  MakeLocalFilePath(char* strFileName)
//  ���s�t�@�C������̑��΃p�X���΃p�X�ɕϊ��C������̓������ꎞ�I�ȃ|�C���^��Ԃ��D
//--------------------------------------------------------------------------------
inline char* MakeLocalFilePath(char* strFileName)
{
    static char str[_MAX_PATH];
    char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
    
    //���f�B���N�g���̎擾
    GetModuleFileName(NULL, str, _MAX_PATH);
    _splitpath(str, drive, dir, fname, ext);
    wsprintf(str, "%s%s%s", drive, dir, strFileName);

	return str;
}

//--------------------------------------------------------------------------------
//  FindFile(char* strDirName)
//    strDirName �ɂ��t�@�C�������C
//    strDirName = NULL; �ŏ����擾�D
//    return NULL; �Ŏ擾�I��
//--------------------------------------------------------------------------------
inline char* FindFile(char* strDirName)
{
    static HANDLE           hFindFile = NULL;
	static WIN32_FIND_DATA  FindFileData;

	if (strDirName != NULL) {
		hFindFile = FindFirstFile(strDirName, &FindFileData);
		if (hFindFile == INVALID_HANDLE_VALUE) return NULL;
	} else {
		if (!FindNextFile(hFindFile, &FindFileData)) {
			FindClose(hFindFile);
			hFindFile = NULL;
			return NULL;
		}
	}
    return FindFileData.cFileName;
}

//--------------------------------------------------------------------------------
//  Message Box �̕\��
//--------------------------------------------------------------------------------
inline void DisplayMessage(char* strMessage)
{
	MessageBox(NULL, strMessage, "Message", MB_OK);
}

inline bool DisplayMessageYN(char* strMessage)
{
	return (MessageBox(NULL, strMessage, "Message", MB_YESNO) == IDYES);
}

//--------------------------------------------------------------------------------
//  ���Ԃ̎擾
//--------------------------------------------------------------------------------
inline unsigned long GetTime()
{
    return timeGetTime();
}

#endif  // _CORE_WINLOCAL_INCLUDED
