//--------------------------------------------------
// シーン列挙子 
//--------------------------------------------------
enum SCENE {
    SCENE_TITLE = 0,
    SCENE_START,
    SCENE_MAIN,
    SCENE_OVER
};


//--------------------------------------------------
// シーンセレクタ
//--------------------------------------------------
#define _CASE_SCENE(prefix, scene) \
    case SCENE_##scene: prefix##scene(); break;

// ※必要シーン数分追記
#define _SWITCH_SCENE(prefix) \
    switch(CSDLCore::GetScene()) { \
    _CASE_SCENE(prefix, MAIN) \
    _CASE_SCENE(prefix, START) \
    _CASE_SCENE(prefix, TITLE) \
    _CASE_SCENE(prefix, OVER) \
    }


//--------------------------------------------------
// コールバック
//--------------------------------------------------
#define _ADD_SCENE_FUNCTION(scene) \
    virtual void Init##scene(){} \
    virtual void Run##scene() {} \
    virtual void Draw##scene(){} \
    virtual void Exit##scene(){} 
    
// ※必要シーン数分追記
#define _VIRTUAL_FUNCTIONS \
    _ADD_SCENE_FUNCTION(TITLE) \
    _ADD_SCENE_FUNCTION(START) \
    _ADD_SCENE_FUNCTION(MAIN) \
    _ADD_SCENE_FUNCTION(OVER) 


//--------------------------------------------------
// Manager 基本クラス
//--------------------------------------------------
class IManager : public IThread
{
public:
    IManager()
    {
    }

    virtual ~IManager()
    {
    }


public:
    virtual bool InitScene()
    {
        _SWITCH_SCENE(Init)
        return true;
    }
    
    virtual bool Run()
    {
        _SWITCH_SCENE(Run)
        return true;
    }
    
    virtual bool Draw()
    {
        _SWITCH_SCENE(Draw)
        return true;
    }

    virtual bool ExitScene()
    {
        _SWITCH_SCENE(Exit)
        return true;
    }


protected:
    _VIRTUAL_FUNCTIONS
    
};


#undef _CASE_SCENE
#undef _SWITCH_SCENE
#undef _ADD_VIRTUAL_FUNCTION
#undef _VIRTUAL_FUNCTIONS
