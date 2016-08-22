template<class type> class TRingBuffer
{
private:
    type*   m_aData;
    int     m_iDataSize;
    int     m_iIndexFilter;
    int     m_iGetIndex;
    int     m_iPutIndex;

public:
    TRingBuffer()
    {
        m_aData = NULL;
        m_iDataSize = 0;
        m_iIndexFilter = 0;
        m_iGetIndex = 0;
        m_iPutIndex = 0;
    }

    ~TRingBuffer()
    {
    }

public:
    bool Alloc(int iSize)
    {
        Free();
        m_aData = (type*)malloc(sizeof(type)*iSize);
        if (m_aData == NULL) return false;
        m_iDataSize = iSize;
        m_iIndexFilter = iSize-1;
        return true;
    }

    bool Free()
    {
        if (m_aData != NULL) free(m_aData);
        m_aData = NULL;
        m_iDataSize = 0;
        m_iIndexFilter = 0;
        m_iGetIndex = 0;
        m_iPutIndex = 0;
        return true;
    }

    int GetSize()
    {
        return m_iDataSize;
    }

    int GetFreeSize()
    {
        return (m_iPutIndex < m_iGetIndex) ? m_iGetIndex - m_iPutIndex :
                                             m_iDataSize - (m_iPutIndex - m_iGetIndex);
    }

    int GetBufferSize()
    {
        return (m_iPutIndex < m_iGetIndex) ? m_iDataSize - (m_iGetIndex - m_iPutIndex):
                                             m_iPutIndex - m_iGetIndex;
    }

public:
    int Get(type* pData, int len)
    {
        len = (GetBufferSize() > len) ? len : GetBufferSize();
        memcpy(pData, &(m_aData[m_iGetIndex]), len * sizeof(type));
        return len;
    }

    int Put(type* pData, int len)
    {
        len = (GetFreeSize() > len) ? len : GetFreeSize();
        memcpy(&(m_aData[m_iGetIndex]), pData, len * sizeof(type));
        return len;
    }

    type* Get()
    {
        if (m_iGetIndex == m_iPutIndex) return NULL;
        type* pRet = &(m_aData[m_iGetIndex]);
        m_iGetIndex = (m_iGetIndex+1) & m_iIndexFilter;
        return pRet;
    }

    bool Put(type* ptr)
    {
        int nextidx = (m_iPutIndex+1) & m_iIndexFilter;
        if (m_iGetIndex == nextidx) return false;
        m_aData[m_iPutIndex] = *ptr;
        m_iPutIndex = nextidx;
        return true;
    }
};
