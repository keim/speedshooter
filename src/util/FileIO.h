#ifndef _CORE_FILEIO_INCLUDED
#define _CORE_FILEIO_INCLUDED

// Windows standard files
#include "std_include.h"
#include <stdarg.h>

class CFileIO
{
private:
    FILE* m_pFile;
    
    
public:
    CFileIO()
    {
        m_pFile = NULL;
    }
    
    CFileIO(const char* strFileName, const char* strOption)
    {
        Open(strFileName, strOption);
    }
    
    virtual ~CFileIO()
    {
        Close();
    }


public:
    operator FILE* ()
    {
        return m_pFile;
    }
    

public:
    bool IsOpened()
    {
        return (m_pFile != NULL);
    }

    FILE* GetFilePointer()
    {
        return m_pFile;
    }
    
    
public:
    bool Open(const char* strFileName, const char* strOption)
    {
        m_pFile = fopen(strFileName, strOption);
        return IsOpened();
    }
    
    void Close()
    {
        if (m_pFile != NULL) fclose(m_pFile);
        m_pFile = NULL;
    }
    
    bool Read(void* pData, int iSize)
    {
        if (!IsOpened()) return false;
        return (fread(pData, iSize, 1, m_pFile) == 1);
    }
    
    bool Write(void* pData, int iSize)
    {
        if (!IsOpened()) return false;
        return (fwrite(pData, iSize, 1, m_pFile) == 1);
    }
    
    int Printf(const char* strFormat, ...) 
    {
        va_list arg;
        va_start(arg, strFormat);
        int ret = vfprintf(m_pFile, strFormat, arg);
        va_end(arg);
        return ret;
    }

    int Scanf(const char* strFormat, ...) 
    {
        va_list arg;
        va_start(arg, strFormat);
        int ret = vfscanf(m_pFile, strFormat, arg);
        va_end(arg);
        return ret;
    }
    
    
public:
    static bool Load(char* strFileName, void* pData, int iSize)
    {
        CFileIO File(strFileName, "rb");
        return File.Read(pData, iSize);
    }

    static bool Save(char* strFileName, void* pData, int iSize)
    {
        CFileIO File(strFileName, "wb");
        return File.Write(pData, iSize);
    }
};

#endif
