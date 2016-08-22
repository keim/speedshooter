//--------------------------------------------------------------------------------
//  SDL Key Configuration Class
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// class CSDLControlManager;  
//   �L�[�R���t�B�O�N���X�CCSDLJoystick���Ǘ�����D
//   CSDLCore �������o�ϐ��Ƃ��Ď��̂������Ă���D
//   �e�L�X�g�t�@�C���ɂ���Đݒ�̓��o�͂��s���D
//--------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------
#include "SDLJoystick.h"

// �A�T�C���ł���L�[
enum _SDLKEYCONF_ASSIGN {
    SKC_XUP    = 0,
    SKC_XDOWN,
    SKC_YUP,
    SKC_YDOWN,
    SKC_BTN0,
    SKC_BTN1,
    SKC_BTN2,
    SKC_BTN3,
    SKC_BTN4,
    SKC_BTN5,
    SKC_BTN6,
    SKC_BTN7,
    SKC_BTN8,
    SKC_BTN9,
    SKC_BTN10,
    SKC_BTN11,
    SKC_ESCAPE,
    _SDLKEYCONF_ASSIGN_MAX
};

// SDLKey���g�����āCJoyStick Button ���A�T�C������
enum _SDLKEYCONF_SDLK_EXT {
    SDLK_JSBTN0 = SDLK_LAST,
    SDLK_JSBTN1,
    SDLK_JSBTN2,
    SDLK_JSBTN3,
    SDLK_JSBTN4,
    SDLK_JSBTN5,
    SDLK_JSBTN6,
    SDLK_JSBTN7,
    SDLK_JSBTN8,
    SDLK_JSBTN9,
    SDLK_JSBTN10,
    SDLK_JSBTN11
};

// ����\�� JoyStick Button �̍ő�l
#define _JOYSTICK_BUTTON_MAX        _SDLKEYCONF_ASSIGN_MAX - 5

// JoyStick Button <=> Default Assign �̕ϊ�
#define _DEFASSIGN_TO_BUTTON_NO(asg)   ((int)(asg)-SKC_BTN0)
#define _BUTTON_NO_TO_DEFASSIGN(btn)    (_SDLKEYCONF_ASSIGN)((btn) +SKC_BTN0)
// JoyStick Button <=> �g�� SDLKey �̕ϊ�
#define _SDLK_TO_BUTTON_NO(key)     ((key)-SDLK_JSBTN0)
#define _BUTTON_NO_TO_SDLK(btn)     ((btn)+SDLK_JSBTN0)

// �w�b�_���
enum _SDLKEYCONF_HEADER_INFO {
    SKCH_VERSION = 0,
    SKCH_FREERANGE,
    _SDLKEYCONF_HEADER_INFO_MAX
};

// �w�b�_������
#define _SDLKEYCONF_HEADER          "#SDLKEYCONF_FILE"
#define _SDLKEYCONF_HEADER_VERSION  "1.0"

// �e�[�u����
#define _SDLKEYCONF_KEYTABLE_MAX    4

// �R���t�B�O�t�@�C����؂蕶��
#define _SDLKEYCONF_DELIMITER       " ,;:=\t\n\r"

// ������o�b�t�@�T�C�Y
#define _SDLKEYCONF_BUFFER_SIZE     256

//--------------------------------------------------------------------------------
// CSDLControlManager
//--------------------------------------------------------------------------------
class CSDLControlManager
{
//--------------------------------------------------
// �����o�ϐ�
//--------------------------------------------------
private:
    // �ݒ�
    SDLKey              m_enSDLKeyTable[_SDLKEYCONF_KEYTABLE_MAX][_SDLKEYCONF_ASSIGN_MAX];    // �L�[�{�[�h�A�T�C���e�[�u��
    _SDLKEYCONF_ASSIGN  m_JoystickButton[_SDLKEYCONF_KEYTABLE_MAX][_JOYSTICK_BUTTON_MAX];     // JoyStick -> KeyBoard �ϊ��e�[�u��
	int				    m_iJoystickFreeRange;   // �W���C�X�e�B�b�N�̗V�є͈�
	int				    m_iButtonMax;		    // �g�p����{�^����

    // SDL ����擾���� KeyFlag
    unsigned char*      m_abyKeyFlag;

    // ���s�t���O
    bool                m_bFlag[_SDLKEYCONF_KEYTABLE_MAX][_SDLKEYCONF_ASSIGN_MAX];          // �A�T�C�����ꂽ�L�[�̃t���O
    bool                m_bDiffFlag[_SDLKEYCONF_KEYTABLE_MAX][_SDLKEYCONF_ASSIGN_MAX];      // �L�[�������ꂽ�ŏ��̈�񂾂�true�̃t���O
    bool                m_bAlive[_SDLKEYCONF_KEYTABLE_MAX];                                 // �g�p�\�R���g���[���̃t���O

    // Joystick �{��
    CSDLJoystick        m_Joystick[_SDLKEYCONF_KEYTABLE_MAX];

	// Buffer Area
    char                strbuf[_SDLKEYCONF_BUFFER_SIZE];

//--------------------------------------------------
// ����/�j��
//--------------------------------------------------
public:
    CSDLControlManager()
    {
        Reset();
    }

    ~CSDLControlManager()
    {
    }

//--------------------------------------------------
// ����
//--------------------------------------------------
public:
    // ���Z�b�g
    void Reset()
    {
        int i, j;
        for (i=0; i<_SDLKEYCONF_KEYTABLE_MAX; i++) {
            for (j=0; j<_SDLKEYCONF_ASSIGN_MAX; j++) {
                m_enSDLKeyTable[i][j] = SDLK_UNKNOWN;
                m_bFlag[i][j] = false;
                m_bDiffFlag[i][j] = false;
            }
            for (j=0; j<_JOYSTICK_BUTTON_MAX; j++) {
                m_JoystickButton[i][j] = _BUTTON_NO_TO_DEFASSIGN(j);
            }
            m_bAlive[i] = false;
        }

        m_bAlive[i] = true;
        m_iJoystickFreeRange = 10000;	// �W���C�X�e�B�b�N�̗V�є͈�
        m_iButtonMax = 5;	            // �g�p����ő�{�^����

        m_abyKeyFlag = NULL;

        UpdateJoystickInfo();

        // Default Setting
        SetAssignKey(SDLK_RIGHT,  0, SKC_XUP);
        SetAssignKey(SDLK_LEFT,   0, SKC_XDOWN);
        SetAssignKey(SDLK_UP,     0, SKC_YDOWN);
        SetAssignKey(SDLK_DOWN,   0, SKC_YUP);
        SetAssignKey(SDLK_z,      0, SKC_BTN0);
        SetAssignKey(SDLK_x,      0, SKC_BTN1);
        SetAssignKey(SDLK_c,      0, SKC_BTN2);
        SetAssignKey(SDLK_ESCAPE, 0, SKC_ESCAPE);
    }

    // �L���� Joystick �̏����擾����
	//   ���s���� Joystick ���}���ꂽ�ꍇ�C
	//   ���̊֐����Ăяo���āCJoyStick��F��������K�v������D
    void UpdateJoystickInfo()
    {
        int i;
        for (i=0; i<_SDLKEYCONF_KEYTABLE_MAX; i++) {
            m_Joystick[i].Close();
            m_Joystick[i].Open(i);
        }
    }

//--------------------------------------------------
// �ݒ�
//--------------------------------------------------
public:
    // �L�[�A�T�C���̐ݒ�ύX
	//   enNewKey; ���蓖�Ă�L�[
	//   iCtrlIdx; �R���g���[���ԍ�
	//   enAssign; ���蓖�Ă�ʒu
    SDLKey SetAssignKey(SDLKey enNewKey, int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        SDLKey enOldKey = m_enSDLKeyTable[iCtrlIdx][enAssign];
        m_enSDLKeyTable[iCtrlIdx][enAssign] = enNewKey;
        m_bAlive[iCtrlIdx] = true;
        if (enAssign < SKC_ESCAPE && m_iButtonMax <= _DEFASSIGN_TO_BUTTON_NO(enAssign) + 1) {
            m_iButtonMax = _DEFASSIGN_TO_BUTTON_NO(enAssign) + 1;
        }
        return enOldKey;
    }

    // JoyStick �� Button �� ���蓖�Ă�
    int SetAssignJSButton(int iJSButtonNo, int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        int oldasg = GetAssignedJoystickButton(iCtrlIdx, enAssign, 0);
        if (iJSButtonNo<0 || iJSButtonNo>=_JOYSTICK_BUTTON_MAX) return oldasg;
        m_JoystickButton[iCtrlIdx][iJSButtonNo] = enAssign;
        return oldasg;
    }

    // JoyStick �ݒ�
    //   iJoystickCount; �K�v�ȃR���g���[����
    //   iButtonMax;     �K�v�ȃ{�^����
    bool SetJoystick(int iJoystickCount, int iButtonMax)
    {
        int i;
        for (i=0; i<iJoystickCount; i++) m_bAlive[i] = true;
        // �g�p����ő�{�^����
        m_iButtonMax = (_JOYSTICK_BUTTON_MAX < iButtonMax) ? _JOYSTICK_BUTTON_MAX : iButtonMax;
    }
    
    // JoyStick�̗V�є͈͂̐ݒ� (0-1)
    void SetJoystickFreeRange(float fJoystickFreeRange)
    {
        m_iJoystickFreeRange = int(32765.0f * fJoystickFreeRange);
    }

    // JoyStick�̗V�є͈͂̎擾 (0-1)
    float GetJoystickFreeRange()
    {
        return (float)m_iJoystickFreeRange / 32765.0f;
    }

//--------------------------------------------------
// �Q��
//   �A�T�C�����ꂽ�L�[�̏�Ԃ��Q�Ƃ���
//--------------------------------------------------
public:
    // ������Ă��邩
    inline bool IsPressed(int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        return m_bFlag[iCtrlIdx][enAssign];
    }

    // �����ꂽ���i��x�����擾������C�t���O�̓��Z�b�g�����j
    inline bool IsHitted(int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        bool bRet = m_bDiffFlag[iCtrlIdx][enAssign];
        m_bDiffFlag[iCtrlIdx][enAssign] = false;
        return bRet;
    }

    // �L�[�e�[�u���̎擾
    inline unsigned char* GetKeyboardTable()
    {
        return m_abyKeyFlag;
    }

    // �L�[�A�T�C�������擾
    inline SDLKey GetAssignKey(int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        return m_enSDLKeyTable[iCtrlIdx][enAssign];
    }

    // �L�[�A�T�C���e�[�u�����擾
    inline SDLKey* GetAssignTable(int iCtrlIdx)
    {
        return m_enSDLKeyTable[iCtrlIdx];
    }

    // Joystick���擾(�������NULL)
    inline CSDLJoystick* GetJoystick(int iCtrlIdx)
    {
        return (m_Joystick[iCtrlIdx].IsOpened()) ? &(m_Joystick[iCtrlIdx]) : NULL;
    }

    // ���� enAssign �����蓖�Ă��Ă���CiButtonNoAfter����� JoyStick�{�^���ԍ� ��Ԃ�
    // ���蓖�Ă��ĂȂ���΁C-1 ��Ԃ�
    inline int GetAssignedJoystickButton(int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign, int iButtonNoAfter=0)
    {
        int btnmax = m_iButtonMax;
        for (int i=iButtonNoAfter; i<btnmax; i++) {
            if (m_JoystickButton[iCtrlIdx][i] == enAssign) return i;
        }
        return -1;
    }

//--------------------------------------------------
// ���s
//--------------------------------------------------
public:
	// �L�[��Ԃ��X�V�iSDLCore������Ă΂��j
    inline void UpdateFlag()
    {
        int  i, j, x, y;
        bool bPrevFlag[_SDLKEYCONF_ASSIGN_MAX];
        int  tblmax = m_iButtonMax+SKC_BTN0;
        int  btnmax = m_iButtonMax;
        m_abyKeyFlag = SDL_GetKeyState(NULL);
        SDL_JoystickUpdate();

        for (i=0; i<_SDLKEYCONF_KEYTABLE_MAX; i++) {
            if (m_bAlive[i]) {
                // �t���O���ꎞ�ۑ�
                for (j=0; j<tblmax; j++) bPrevFlag[j] = m_bFlag[i][j];

                // �L�[�{�[�h ������擾
                m_bFlag[i][SKC_XUP]   = (m_abyKeyFlag[m_enSDLKeyTable[i][SKC_XUP]]   == SDL_PRESSED);
                m_bFlag[i][SKC_XDOWN] = (m_abyKeyFlag[m_enSDLKeyTable[i][SKC_XDOWN]] == SDL_PRESSED);
                m_bFlag[i][SKC_YUP]   = (m_abyKeyFlag[m_enSDLKeyTable[i][SKC_YUP]]   == SDL_PRESSED);
                m_bFlag[i][SKC_YDOWN] = (m_abyKeyFlag[m_enSDLKeyTable[i][SKC_YDOWN]] == SDL_PRESSED);
                for (j=0; j<btnmax; j++) {
                    m_bFlag[i][j+SKC_BTN0] = (m_abyKeyFlag[m_enSDLKeyTable[i][j+SKC_BTN0]] == SDL_PRESSED);
                }

                // Joystick ������擾
                if (m_Joystick[i].IsOpened()) {
		            x = m_Joystick[i].GetXAxis();
		            y = m_Joystick[i].GetYAxis();
                    m_bFlag[i][SKC_XDOWN] |= (x < -m_iJoystickFreeRange);
                    m_bFlag[i][SKC_XUP]   |= (x >  m_iJoystickFreeRange);
                    m_bFlag[i][SKC_YDOWN] |= (y < -m_iJoystickFreeRange);
                    m_bFlag[i][SKC_YUP]   |= (y >  m_iJoystickFreeRange);
                    for (j=0; j<btnmax; j++) {
                        m_bFlag[i][m_JoystickButton[i][j]] |= (m_Joystick[i].IsPressed(j));
                    }
                }

                // �����t���O
                for (j=0; j<tblmax; j++) m_bDiffFlag[i][j] = (bPrevFlag[j] ^ m_bFlag[i][j]) & m_bFlag[i][j];
            }
        }
    }

//--------------------------------------------------
// �t�@�C������
//--------------------------------------------------
public:
    // ���[�h
    bool Load(char* strFileName)
    {
    	char*               tkn;
    	int                 iControlerIndex;
        int                 iSDLKey;
        _SDLKEYCONF_ASSIGN  enAssign;
        bool                bVersionCheck = false;
        FILE*   pFile = fopen(strFileName, "rt");
        if (pFile == NULL) return false;

        try {
            // �w�b�_�Ǎ���
            tkn = GetHeadToken(pFile);
            if (strcmp(tkn, _SDLKEYCONF_HEADER) != 0) throw false;
            // �P�s�P�A�T�C��
        	for (tkn = GetHeadToken(pFile); tkn != NULL; tkn = GetHeadToken(pFile)) {
                if (tkn[0] == '#') {
                    // �w�b�_�Ǎ���
                    switch (CheckHeader(&(tkn[1]))) {
                    case SKCH_VERSION:
                        if ((tkn = GetNextToken()) == NULL) throw false;
                        if (strcmp(tkn, _SDLKEYCONF_HEADER_VERSION) != 0) throw false;
                        bVersionCheck = true;
                        break;
                    case SKCH_FREERANGE:
                        if ((tkn = GetNextToken()) == NULL) throw false;
                        SetJoystickFreeRange((float)atof(tkn));
                        break;
                    default:
                        throw false;
                    }
                } else {
                    // �f�[�^�Ǎ���
        		    iControlerIndex = tkn[0] - '0';
        		    if (iControlerIndex<0 || iControlerIndex>=_SDLKEYCONF_KEYTABLE_MAX)         throw false;
                    if ((enAssign = StringToAssign(GetNextToken())) == _SDLKEYCONF_ASSIGN_MAX)  throw false;
                    // �ݒ�
                    iSDLKey = StringToSDLKey(GetNextToken());
                    // iSDLKey >= SDLK_JSBTN0 �Ȃ�CJoyStick Button Assign
                    if (iSDLKey >= SDLK_JSBTN0) SetAssignJSButton(_SDLK_TO_BUTTON_NO(iSDLKey), iControlerIndex, enAssign);
                    else                        SetAssignKey((SDLKey)iSDLKey, iControlerIndex, enAssign);
                }
        	}
            // �I�[�`�F�b�N
            throw (feof(pFile)!=0);
        }

        catch(bool bReturnValue)
        {
            fclose(pFile);
            return bReturnValue;
        }
    }

    // �Z�[�u
    bool Save(char* strFileName)
    {
    	int     iControlerIndex;
        int     iAssign;
        SDLKey  enKey;
        int     iButtonNo;

        FILE*   pFile = fopen(strFileName, "wt");
        if (pFile == NULL) return false;

        try {
            // �w�b�_��������
            if (fputs(MakeHeaderString(), pFile) == EOF) throw false;
            for (iControlerIndex=0; iControlerIndex<_SDLKEYCONF_KEYTABLE_MAX; iControlerIndex++) {
                // �g�p���Ă���R���g���[���̂ݏ������݂��s��
                if (m_bAlive[iControlerIndex]) {
                    for (iAssign=SKC_XUP; iAssign<_SDLKEYCONF_ASSIGN_MAX; iAssign++) {
                        enKey = GetAssignKey(iControlerIndex, (_SDLKEYCONF_ASSIGN)iAssign);
                        // SDLK_UNKNOWN �łȂ���΁C��������
                        if (enKey != SDLK_UNKNOWN) {
                            MakeLineString(enKey, iControlerIndex, (_SDLKEYCONF_ASSIGN)iAssign);
                            if (fputs(strbuf, pFile) == EOF) throw false;
                        }
                    }
                    for (iButtonNo=0; iButtonNo<m_iButtonMax; iButtonNo++) {
                        if (m_JoystickButton[iControlerIndex][iButtonNo] != _BUTTON_NO_TO_DEFASSIGN(iButtonNo)) {
                            MakeLineString(_BUTTON_NO_TO_SDLK(iButtonNo), iControlerIndex, (_SDLKEYCONF_ASSIGN)iAssign);
                            if (fputs(strbuf, pFile) == EOF) throw false;
                        }
                    }
                }
            }
            throw true;
        }

        catch(bool bReturnValue)
        {
            fclose(pFile);
            return bReturnValue;
        }
    }

private:
    // File ����P�s�ǂݍ���ŁC�ŏ��̃g�[�N����Ԃ�
	char* GetHeadToken(FILE* pFile)
	{
		if (fgets(strbuf, _SDLKEYCONF_BUFFER_SIZE, pFile)==NULL) return NULL;
		return strtok(strbuf, _SDLKEYCONF_DELIMITER);
	}

    // ���g�[�N����Ԃ�
	char* GetNextToken()
	{
		return strtok(NULL, _SDLKEYCONF_DELIMITER);
	}

    // �t�@�C���ɏ������ޕ�������쐬����
    char* MakeLineString(int iSDLKey, int iCtrlIdx, _SDLKEYCONF_ASSIGN enAssign)
    {
        if (enAssign == _SDLKEYCONF_ASSIGN_MAX) return NULL;
        char strLine[256];
        sprintf(strLine, "%d; %s = ", iCtrlIdx, AssignToString(enAssign));
        strcat(strLine, SDLKeyToString(iSDLKey));
        strcat(strLine, "\n");
        strcpy(strbuf, strLine);
        return strbuf;
    }

    // Header ������̍쐬
    char* MakeHeaderString()
    {
        char strLF[]="\n";
        char strLine[64];

        sprintf(strLine, "%s%s", _SDLKEYCONF_HEADER, strLF);
        strcpy(strbuf, strLine);
        sprintf(strLine, "#VERSION    %s%s", _SDLKEYCONF_HEADER_VERSION, strLF);
        strcat(strbuf, strLine);
        sprintf(strLine, "#FREERANGE  %f%s", GetJoystickFreeRange(), strLF);
        strcat(strbuf, strLine);
        return strbuf;
    }

//--------------------------------------------------
// ������ϊ�
//--------------------------------------------------
public:
#define _STR2KEY_MCR(key)   if (stricmp(strSDLKey, #key) == 0) return SDLK_##key;
	// ������ -> enum SDLKey
	int StringToSDLKey(char* strSDLKey)
	{
        if (strSDLKey == NULL) return SDLK_UNKNOWN;

        _STR2KEY_MCR(UNKNOWN)
        else _STR2KEY_MCR(ESCAPE)
        else _STR2KEY_MCR(BACKSPACE)
        else _STR2KEY_MCR(TAB)
        else _STR2KEY_MCR(RETURN)
        else _STR2KEY_MCR(PAUSE)
        else _STR2KEY_MCR(SPACE)
        else _STR2KEY_MCR(DELETE)
        else _STR2KEY_MCR(KP0)
        else _STR2KEY_MCR(KP1)
        else _STR2KEY_MCR(KP2)
        else _STR2KEY_MCR(KP3)
        else _STR2KEY_MCR(KP4)
        else _STR2KEY_MCR(KP5)
        else _STR2KEY_MCR(KP6)
        else _STR2KEY_MCR(KP7)
        else _STR2KEY_MCR(KP8)
        else _STR2KEY_MCR(KP9)
        else _STR2KEY_MCR(KP_PERIOD)
        else _STR2KEY_MCR(KP_DIVIDE)
        else _STR2KEY_MCR(KP_MULTIPLY)
        else _STR2KEY_MCR(KP_MINUS)
        else _STR2KEY_MCR(KP_PLUS)
        else _STR2KEY_MCR(KP_ENTER)
        else _STR2KEY_MCR(KP_EQUALS)
        else _STR2KEY_MCR(UP)
        else _STR2KEY_MCR(DOWN)
        else _STR2KEY_MCR(RIGHT)
        else _STR2KEY_MCR(LEFT)
        else _STR2KEY_MCR(INSERT)
        else _STR2KEY_MCR(HOME)
        else _STR2KEY_MCR(END)
        else _STR2KEY_MCR(PAGEUP)
        else _STR2KEY_MCR(PAGEDOWN)
        else _STR2KEY_MCR(F1)
        else _STR2KEY_MCR(F2)
        else _STR2KEY_MCR(F3)
        else _STR2KEY_MCR(F4)
        else _STR2KEY_MCR(F5)
        else _STR2KEY_MCR(F6)
        else _STR2KEY_MCR(F7)
        else _STR2KEY_MCR(F8)
        else _STR2KEY_MCR(F9)
        else _STR2KEY_MCR(F10)
        else _STR2KEY_MCR(F11)
        else _STR2KEY_MCR(F12)
        else _STR2KEY_MCR(F13)
        else _STR2KEY_MCR(F14)
        else _STR2KEY_MCR(F15)
        else _STR2KEY_MCR(NUMLOCK)
        else _STR2KEY_MCR(CAPSLOCK)
        else _STR2KEY_MCR(SCROLLOCK)
        else _STR2KEY_MCR(RSHIFT)
        else _STR2KEY_MCR(LSHIFT)
        else _STR2KEY_MCR(RCTRL)
        else _STR2KEY_MCR(LCTRL)
        else _STR2KEY_MCR(RALT)
        else _STR2KEY_MCR(LALT)
        else _STR2KEY_MCR(RSUPER)
        else _STR2KEY_MCR(LSUPER)
	    else _STR2KEY_MCR(JSBTN0)
	    else _STR2KEY_MCR(JSBTN1)
	    else _STR2KEY_MCR(JSBTN2)
	    else _STR2KEY_MCR(JSBTN3)
	    else _STR2KEY_MCR(JSBTN4)
	    else _STR2KEY_MCR(JSBTN5)
	    else _STR2KEY_MCR(JSBTN6)
	    else _STR2KEY_MCR(JSBTN7)
	    else _STR2KEY_MCR(JSBTN8)
	    else _STR2KEY_MCR(JSBTN9)
	    else _STR2KEY_MCR(JSBTN10)
	    else _STR2KEY_MCR(JSBTN11)

		return (SDLKey)strSDLKey[0];
	}
#undef _STR2KEY_MCR

#define _KEY2STR_MCR(key)   case SDLK_##key:   strcpy(strbuf, #key);  break;
	// enum SDLKey -> ������
	char* SDLKeyToString(int iSDLKey)
	{
		switch (iSDLKey) {
        _KEY2STR_MCR(UNKNOWN)
        _KEY2STR_MCR(ESCAPE)
        _KEY2STR_MCR(BACKSPACE)
        _KEY2STR_MCR(TAB)
        _KEY2STR_MCR(RETURN)
        _KEY2STR_MCR(PAUSE)
        _KEY2STR_MCR(SPACE)
        _KEY2STR_MCR(DELETE)
        _KEY2STR_MCR(KP0)
        _KEY2STR_MCR(KP1)
        _KEY2STR_MCR(KP2)
        _KEY2STR_MCR(KP3)
        _KEY2STR_MCR(KP4)
        _KEY2STR_MCR(KP5)
        _KEY2STR_MCR(KP6)
        _KEY2STR_MCR(KP7)
        _KEY2STR_MCR(KP8)
        _KEY2STR_MCR(KP9)
        _KEY2STR_MCR(KP_PERIOD)
        _KEY2STR_MCR(KP_DIVIDE)
        _KEY2STR_MCR(KP_MULTIPLY)
        _KEY2STR_MCR(KP_MINUS)
        _KEY2STR_MCR(KP_PLUS)
        _KEY2STR_MCR(KP_ENTER)
        _KEY2STR_MCR(KP_EQUALS)
        _KEY2STR_MCR(UP)
        _KEY2STR_MCR(DOWN)
        _KEY2STR_MCR(RIGHT)
        _KEY2STR_MCR(LEFT)
        _KEY2STR_MCR(INSERT)
        _KEY2STR_MCR(HOME)
        _KEY2STR_MCR(END)
        _KEY2STR_MCR(PAGEUP)
        _KEY2STR_MCR(PAGEDOWN)
        _KEY2STR_MCR(F1)
        _KEY2STR_MCR(F2)
        _KEY2STR_MCR(F3)
        _KEY2STR_MCR(F4)
        _KEY2STR_MCR(F5)
        _KEY2STR_MCR(F6)
        _KEY2STR_MCR(F7)
        _KEY2STR_MCR(F8)
        _KEY2STR_MCR(F9)
        _KEY2STR_MCR(F10)
        _KEY2STR_MCR(F11)
        _KEY2STR_MCR(F12)
        _KEY2STR_MCR(F13)
        _KEY2STR_MCR(F14)
        _KEY2STR_MCR(F15)
        _KEY2STR_MCR(NUMLOCK)
        _KEY2STR_MCR(CAPSLOCK)
        _KEY2STR_MCR(SCROLLOCK)
        _KEY2STR_MCR(RSHIFT)
        _KEY2STR_MCR(LSHIFT)
        _KEY2STR_MCR(RCTRL)
        _KEY2STR_MCR(LCTRL)
        _KEY2STR_MCR(RALT)
        _KEY2STR_MCR(LALT)
        _KEY2STR_MCR(RSUPER)
        _KEY2STR_MCR(LSUPER)
	    _KEY2STR_MCR(JSBTN0)
	    _KEY2STR_MCR(JSBTN1)
	    _KEY2STR_MCR(JSBTN2)
	    _KEY2STR_MCR(JSBTN3)
	    _KEY2STR_MCR(JSBTN4)
	    _KEY2STR_MCR(JSBTN5)
	    _KEY2STR_MCR(JSBTN6)
	    _KEY2STR_MCR(JSBTN7)
	    _KEY2STR_MCR(JSBTN8)
	    _KEY2STR_MCR(JSBTN9)
	    _KEY2STR_MCR(JSBTN10)
	    _KEY2STR_MCR(JSBTN11)
		default:
            strbuf[0] = (char)iSDLKey;
            strbuf[1] = '\0';
		}
    	return strbuf;
	}
#undef _KEY2STR_MCR

#define _STR2ASG_MCR(key)   if (stricmp(strAssign, #key) == 0) return SKC_##key;
	// ������ -> _SDLKEYCONF_ASSIGN
	_SDLKEYCONF_ASSIGN StringToAssign(char* strAssign)
	{
        if (strAssign == NULL) return _SDLKEYCONF_ASSIGN_MAX;

        _STR2ASG_MCR(XUP)
        else _STR2ASG_MCR(XDOWN)
        else _STR2ASG_MCR(YUP)
        else _STR2ASG_MCR(YDOWN)
        else _STR2ASG_MCR(BTN0)
        else _STR2ASG_MCR(BTN1)
        else _STR2ASG_MCR(BTN2)
        else _STR2ASG_MCR(BTN3)
        else _STR2ASG_MCR(BTN4)
        else _STR2ASG_MCR(BTN5)
        else _STR2ASG_MCR(BTN6)
        else _STR2ASG_MCR(BTN7)
        else _STR2ASG_MCR(BTN8)
        else _STR2ASG_MCR(BTN9)
        else _STR2ASG_MCR(BTN10)
        else _STR2ASG_MCR(BTN11)
		else _STR2ASG_MCR(ESCAPE)
		return _SDLKEYCONF_ASSIGN_MAX;
    }
#undef _STR2ASG_MCR
	
#define _ASG2STR_MCR(key)   case SKC_##key:   strcpy(strbuf, #key);  break;
	// _SDLKEYCONF_ASSIGN -> ������
	char* AssignToString(_SDLKEYCONF_ASSIGN enAssign)
	{
		switch (enAssign) {
        _ASG2STR_MCR(XUP)
        _ASG2STR_MCR(XDOWN)
        _ASG2STR_MCR(YUP)
        _ASG2STR_MCR(YDOWN)
        _ASG2STR_MCR(BTN0)
        _ASG2STR_MCR(BTN1)
        _ASG2STR_MCR(BTN2)
        _ASG2STR_MCR(BTN3)
        _ASG2STR_MCR(BTN4)
        _ASG2STR_MCR(BTN5)
        _ASG2STR_MCR(BTN6)
        _ASG2STR_MCR(BTN7)
        _ASG2STR_MCR(BTN8)
        _ASG2STR_MCR(BTN9)
        _ASG2STR_MCR(BTN10)
        _ASG2STR_MCR(BTN11)
		_ASG2STR_MCR(ESCAPE)
		default:
			return NULL;
		}
		return strbuf;
	}
#undef _ASG2STR_MCR

#define _STR2HDR_MCR(key)   if (stricmp(strHead, #key) == 0) return SKCH_##key;
    // �w�b�_������̃`�F�b�N
    _SDLKEYCONF_HEADER_INFO CheckHeader(char* strHead)
    {
        _STR2HDR_MCR(VERSION)
        else _STR2HDR_MCR(FREERANGE)
        return _SDLKEYCONF_HEADER_INFO_MAX;
    }
#undef _STR2HDR_MCR
};
