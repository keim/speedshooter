#include <fstream>
#define  LOG_FILE_NAME   "log.txt"

class CLogger
{
public:
    std::ofstream* m_pOutStream;
    
    CLogger()
    {
        m_pOutStream = new std::ofstream(LOG_FILE_NAME);
    }
    
    ~CLogger()
    {
        if (m_pOutStream != NULL) delete m_pOutStream;
    }
};

CLogger g_Logger;

#define LOG(o) (*(g_Logger.m_pOutStream))<<o<<std::endl;
