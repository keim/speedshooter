//--------------------------------------------------------------------------------
//  SDL Joystick Wrapping Class
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
//  SDL ä÷êîÇÃñºëOÇ™í∑Ç¢ÇÃÇ≈ÅDÇªÇÍÇæÇØ
//--------------------------------------------------------------------------------
class CSDLJoystick
{
private:
    SDL_Joystick*   m_pJoystick;
    
public:
    CSDLJoystick()
    {
        m_pJoystick = NULL;
    }
    
    CSDLJoystick(int idx)
    {
        Open(idx);
    }

    ~CSDLJoystick()
    {
        Close();
    }

public:
    bool Open(int idx)
    {
        m_pJoystick = SDL_JoystickOpen(idx);
        return (m_pJoystick != NULL);
    }
    
    void Close()
    {
        if (m_pJoystick == NULL) SDL_JoystickClose(m_pJoystick);
        m_pJoystick = NULL;
    }
    
    inline bool IsOpened()
    {
        return (m_pJoystick != NULL);
    }
    
    inline int GetXAxis()
    {
        return SDL_JoystickGetAxis(m_pJoystick, 0);
    }

    inline int GetYAxis()
    {
        return SDL_JoystickGetAxis(m_pJoystick, 1);
    }

    inline int GetZAxis()
    {
        return SDL_JoystickGetAxis(m_pJoystick, 2);
    }
    
    inline bool IsPressed(int button_idx)
    {
        return (SDL_JoystickGetButton(m_pJoystick, button_idx) != 0);
    }
	
	inline SDL_Joystick* GetPointer()
	{
		return m_pJoystick;
	}
};

