#include "RingBuffer.h"

//------------------------------------------------------------
// Socket class
//------------------------------------------------------------
class CSDLSocket
{
private:
    TCPsocket           m_Socket;
    bool                m_bDisconnect;
    TRingBuffer<char>   m_Buffer;

public:
    CSDLSocket()
    {
        m_Socket = NULL;
        m_bDisconnect = false;
    }

public:
    bool Open(char* strHost, int iPort, int iBufferSize = 1024)
    {
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, strHost, iPort) == -1) return false;
        m_Socket = SDLNet_TCP_Open(&ip);
        m_bDisconnect = false;
        return true;
    }

    bool Close()
    {
        if (IsOpened()) SDLNet_TCP_Close(m_Socket);
        m_Socket = NULL;
        m_bDisconnect = false;
        return true;
    }

public:
    void Update()
    {
        if (!IsConnected()) return;
        if (SDLNet_SocketReady(m_Socket)) {
            char buffer[4096];
            int maxlen = m_Buffer.GetFreeSize();
            maxlen = (maxlen > 4096) ? 4096 : maxlen;
            int n = SDLNet_TCP_Recv(m_Socket, buffer, maxlen);
            if (n <= 0) {
                m_bDisconnect = true;
                return;
            }
            m_Buffer.Put(buffer, n);
        }
    }

    int Recv(void* pData, int len)
    {
        Update();
        if (!IsConnected()) return false;
        return m_Buffer.Get((char*)pData, len);
    }

    int Send(void* pSrc, int len)
    {
        if (!IsConnected()) return false;
        return SDLNet_TCP_Send(m_Socket, pSrc, len);
    }

public:
    bool IsOpened()
    {
        return (m_Socket != NULL);
    }

    bool IsConnected()
    {
        return (IsOpened() && !m_bDisconnect);
    }

    TCPsocket* GetSocket()
    {
        return &m_Socket;
    }
};


class CSDLSocketUDP
{
public:
    CSDLSocketUDP()
    {
    }
};

class CSDLSocketSet
{
public:
    CSDLSocketSet()
    {
    }

public:
    bool AddSocket(CSDLSocket* pSocket)
    {
        return false;
    }

    bool DelSocket(CSDLSocket* pSocket)
    {
        return false;
    }

    bool Check(int iTimeOut=0)
    {
        return false;
    }
};

//------------------------------------------------------------
// Socket class
//------------------------------------------------------------
class CSDLServer
{
public:
    CSDLServer()
    {
    }
};