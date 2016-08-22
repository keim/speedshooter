//--------------------------------------------------------------------------------
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
#include "std_include.h"
#include "Memory.h"

namespace Util {
//--------------------------------------------------------------------------------
// メモリ操作
//--------------------------------------------------------------------------------
// サイズ記録メモリ確保
char* memalloc(long size)
{
    long *block;
    long lDataSize;

    lDataSize = (size + 4) >> 2;
    block = (long*)calloc(lDataSize+1, 4);
    if (block == NULL) return NULL;
    block[0] = size;
    return (char*)(&block[1]);
}

// サイズ記録メモリ解放
void memfree(char *ptr)
{
    if (ptr != NULL) free(&(((long*)ptr)[-1]));
}

// メモリ拡張
char* memexpand(char *a, long expand)
{
    return memrealloc(a, memlength(a) + expand);
}

// メモリ再確保
char* memrealloc(char *ptr, long size)
{
    long *block;
    long i, oldsize, newsize;

    if (size == 0) {
        memfree(ptr);
        return NULL;
    }

    if (ptr == NULL) {
        block = NULL;
        oldsize = 0;
    } else {
        block = &(((long*)ptr)[-1]);
        oldsize = block[0];
    }

    newsize = (size + 3) & 0xfffffffc;

    if (newsize != oldsize) {
        block = (long*)realloc(block, sizeof(long)+newsize);
        if (block == NULL) return NULL;
        ptr = (char*)&(block[1]);
        for (i=oldsize; i<newsize; i++) ptr[i]=0;
    }

    block[0] = size;
    return (char*)&(block[1]);
}

// メモリ連結
char* memjoin(char *ptr1, char *ptr2)
{
    long i, size, extra;

    size = memlength(ptr1);
    extra = memlength(ptr2);
    ptr1 = memexpand(ptr1, extra);
    if (ptr1 != NULL) {
        for (i=0; i<extra; i++) ptr1[i+size] = ptr2[i];
    }
    return ptr1;
}

// ファイル読み込み
char* memload(char *path, int buffer_size)
{
    FILE* pFile = fopen(path, "rb");
    if (pFile == NULL) return NULL;
    try {
        int   isize = 0;
        char* mem = memalloc(buffer_size);
        char* newarea = mem;
        while (mem != NULL) {
            newarea = &(mem[isize]);
            if (fread(newarea, 1, buffer_size, pFile) != 1024) throw mem;
            isize = memlength(mem);
            mem = memexpand(mem, buffer_size);
        }
        throw 0;
    }
    catch (char* return_ptr) {
        fclose(pFile);
        return return_ptr;
    }
}

//--------------------------------------------------------------------------------
// Riff Data 関連 [ChunkID Size Data]...
//--------------------------------------------------------------------------------
// Riff Data Chunk の ID文字列 を読みこむ
bool ReadRiffChunkID(FILE* pFile, char* strID)
{
    if (fread(strID,  1, 4, pFile) != 4) return false;
    return true;
}

// ReadRiffChunkID() 後，Riff Data Chunk の データ本体 を読みこむ．
// NULL渡しでサイズのみ読みこむ．NULL渡しで無い場合，メモリ不足でも次のChunkまで読み飛ばす．
int ReadRiffChunkData(FILE* pFile, void* pData/*=NULL*/, int iMemorySize/*=0*/)
{
    int isize;
    if (fread(&isize, 4, 1, pFile) != 1) return 0;
    if (pData == NULL) return isize;
    if (isize > iMemorySize) {
        if ((long)fread(pData, 1, iMemorySize, pFile) != iMemorySize) return 0;
        if (!fseek(pFile, isize - iMemorySize, SEEK_CUR) == 0)        return 0;
    } else {
        if ((long)fread(pData, 1, isize, pFile) != isize)             return 0;
    }
    return isize;
}

// ReadRiffChunkID() 後，Riff Data を読み飛ばす
int SkipRiffChunk(FILE* pFile)
{
    int isize;
    if (fread(&isize, 4, 1, pFile) != 1) return 0;
    if (fseek(pFile, isize, SEEK_CUR) != 0) return 0;
    return isize;
}

// Riff Data Chunk を書きこむ
bool WriteRiffChunk(FILE* pFile, char* strID, int iSize, void* pData)
{
    if (fwrite(strID,  1, 4, pFile) != 4) return false;
    if (fwrite(&iSize, 4, 1, pFile) != 1) return false;
    if ((long)fwrite(pData, 1, iSize, pFile) != iSize) return false;
    return true;
}

//--------------------------------------------------------------------------------
// File 文字列操作関連
//--------------------------------------------------------------------------------
// ディレクトリパス以下削除（最後に"\"を含む） 
bool GetFileDir(char* strFileName)
{
    char* pLastSlash = StrRChar(strFileName, '\\');
    if (pLastSlash == NULL) return false;
    pLastSlash[1] = '\0';
    return true;
}

// ディレクトリパス（最後に"\"を含まない）/ファイル名/拡張子に分離
bool GetFileName(char* strFilePath, char* &pDir, char* &pName, char* &pExt)
{
    char* pLastSlash = StrRChar(strFilePath, '\\');
    char* pLastDot   = StrRChar(strFilePath, '.');
    
    if (pLastSlash != NULL) {
        pLastSlash[0] = '\0';
        pDir = strFilePath;
        pName = &(pLastSlash[1]);
    } else {
        pDir = NULL;
        pName = strFilePath;
    }
    
    if (pLastDot != NULL) {
        *pLastDot = '\0';
        pExt = &(pLastDot[1]);
    } else {
        pExt = NULL;
    }
    
    return (pDir != NULL && pExt != NULL);
}

// ファイルの相対パスを取得
bool SetRelativePath(char* strFilePath, char* strHomeDir, char* strFileName)
{
    int match_length = StrSearchIComp(strHomeDir, strFileName);
    if (match_length == 0) {
        strcpy(strFilePath, strFileName);
        return false;
    }
    
    strncpy(strFilePath, strHomeDir, match_length);
    strFilePath[match_length] = '\0';
    char* pLastSlash = StrRChar(strFilePath, '\\');
    if (pLastSlash == NULL) {
        strcpy(strFilePath, strFileName);
        return false;
    }
    match_length = strFilePath - pLastSlash + 1;
    strFilePath[0] = '\0';
    int layer = StrCharCount(&(strHomeDir[match_length]), '\\');
    for (int i=0; i<layer; i++) strcat(strFilePath, "..\\");
    strcat(strFilePath, &(strFileName[match_length]));
    return true;
}

} // namespace Util

