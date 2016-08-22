//--------------------------------------------------------------------------------
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
#include "std_include.h"
#include "include_opengl.h"

namespace GL {
CArray::GL_ARRAY_VERTEX_INFO CArray::s_info[CArray::GL_ARRAY_FORMAT_MAX] =
{
    {GL_V3F,          4,  0,  0,  0,  0},
    {GL_T2F_V3F,      6,  0,  0,  0,  2},
    {GL_N3F_V3F,      7,  0,  0,  0,  3},
    {GL_T2F_N3F_V3F,  9,  0,  2,  0,  5},
    {GL_C4UB_V3F,     5,  0,  0,  0,  1},
    {GL_T2F_C4UB_V3F, 7,  0,  0,  2,  3}
};
} //namespace GL 
