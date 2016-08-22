//--------------------------------------------------------------------------------
//  SDL Sound Wrapping Class
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
//  SDL_mixer 関数の名前が長いので．それだけ
//--------------------------------------------------------------------------------

class CSDLSound
{
private:
    Mix_Chunk* m_pWave;

public:
    CSDLSound()
    {
        m_pWave = NULL;
    }

    ~CSDLSound()
    {
        Free();
    }

//--------------------------------------------------
//  参照
//--------------------------------------------------
public:
    inline Mix_Chunk* GetChunk() const 
    {
        return m_pWave;
    }

    inline bool IsAllocated()
    {
        return (m_pWave->allocated != 0);
    }

    inline unsigned long GetLength()
    {
        return m_pWave->alen;
    }

    inline unsigned char* GetData()
    {
        return m_pWave->abuf;
    }

    inline int GetVolume()
    {
        return m_pWave->volume;
    }

//--------------------------------------------------
//  操作
//--------------------------------------------------
public:
    bool Load(const char* strFileName)
    {
        m_pWave = Mix_LoadWAV(strFileName);
        return (m_pWave != NULL);
    }

    bool Alloc(int iSize)
    {
        m_pWave = new Mix_Chunk;
        if (m_pWave == NULL) return false;
	    m_pWave->abuf = (Uint8*)malloc(iSize);
        if (m_pWave->abuf == NULL) {
            delete m_pWave;
            return false;
        }
	    m_pWave->allocated = 1;
	    m_pWave->alen = iSize;
	    m_pWave->volume = 128;
        return true;
    }

    void Free()
    {
        Mix_FreeChunk(m_pWave);
        m_pWave = NULL;
    }

    int Play(int channel, int iLoopCount)
    {
        return (Mix_PlayChannel(channel, m_pWave, iLoopCount) != -1);
    }

    void FadeIn(int channel, int iLoopCount, int iMiliSec)
    {
        Mix_FadeInChannel(channel, m_pWave, iLoopCount, iMiliSec);
    }

    static void Stop(int channel)
    {
        Mix_HaltChannel(channel);
    }

    static void FadeOut(int channel, int iMiliSec)
    {
        Mix_FadeOutChannel(channel, iMiliSec);
    }

    static void Volume(int channel, int iVolume)
    {
        Mix_Volume(channel, iVolume);
    }
    
    static void Pause(int channel)
    {
        Mix_Pause(channel);
    }

    static void Resume(int channel)
    {
        Mix_Resume(channel);
    }
};




class CSDLMusic
{
private:
    Mix_Music* m_pWave;

public:
    CSDLMusic()
    {
        m_pWave = NULL;
    }

    ~CSDLMusic()
    {
        Free();
    }

//--------------------------------------------------
//  参照
//--------------------------------------------------
public:
    Mix_Music* GetMusic() const 
    {
        return m_pWave;
    }

//--------------------------------------------------
//  操作
//--------------------------------------------------
public:
    bool Load(const char* strFileName)
    {
        m_pWave = Mix_LoadMUS(strFileName);
        return (m_pWave != NULL);
    }

    void Free()
    {
        Mix_FreeMusic(m_pWave);
        m_pWave = NULL;
    }

    bool Play(int iLoopCount)
    {
        return (Mix_PlayMusic(m_pWave, iLoopCount) != -1);
    }
    
    void FadeIn(int iLoopCount, int iMiliSec)
    {
        Mix_FadeInMusic(m_pWave, iLoopCount, iMiliSec);
    }

    static void Stop()
    {
        Mix_HaltMusic();
    }

    static void FadeOut(int iMiliSec)
    {
        Mix_FadeOutMusic(iMiliSec);
    }

    static void Volume(int iVolume)
    {
        Mix_VolumeMusic(iVolume);
    }
    
    static void Pause()
    {
        Mix_PauseMusic();
    }

    static void Resume()
    {
        Mix_ResumeMusic();
    }
};

