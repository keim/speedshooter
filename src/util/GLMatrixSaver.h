//--------------------------------------------------------------------------------
//  OpenGL Wrapping Class (GL::CMatrixSaver)
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// Model View / Prokjection çsóÒï€ë∂ÉNÉâÉX
// 
// 
// 
//--------------------------------------------------------------------------------

#ifndef _CORE_GLMATRIXSAVER_INCLUDED
#define _CORE_GLMATRIXSAVER_INCLUDED

#pragma message ("<< CORE_MESSAGE >>  include GL::CMatrixSaver")

namespace GL {
class CMatrixSaver
{
private:
    int     m_aiViewport[4];
	double  m_adModelview[16];
	double  m_adProjection[16];

public:
	CMatrixSaver()
    {
        Save();
	}

	~CMatrixSaver()
    {
	}
	
public:
    void Save()
    {
        glGetIntegerv(GL_VIEWPORT,          m_aiViewport);
        glGetDoublev (GL_MODELVIEW_MATRIX,  m_adModelview);
        glGetDoublev (GL_PROJECTION_MATRIX, m_adProjection);
    }
    
    void Restore()
    {
        int iCurrentMode;
        glGetIntegerv(GL_MATRIX_MODE, &iCurrentMode);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(m_adModelview);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixd(m_adProjection);

        glMatrixMode(iCurrentMode);
    }
    
public:
    double* GetModelViewMatrix()
    {
        return m_adModelview;
    }

    double* GetProjectionMatrix()
    {
        return m_adProjection;
    }

    void GetModelViewMatrix(float* pMat)
    {
        for (int i=0; i<16; i++) pMat[i] = (float)(m_adModelview[i]);
    }

    void GetProjectionMatrix(float* pMat)
    {
        for (int i=0; i<16; i++) pMat[i] = (float)(m_adProjection[i]);
    }
    
	double* GetScreenNormal()
	{
		
	}

public:
    void Project(const float srcx, const float srcy, const float srcz, float &dstx, float &dsty)
    {
        double x, y, z;
        
        gluProject((GLdouble)srcx, (GLdouble)srcy, (GLdouble)srcz, 
                   m_adModelview, m_adProjection, m_aiViewport, 
                   &x, &y, &z);
                   
        dstx = (float)(x-m_aiViewport[2]*0.5);
        dsty = (float)(y-m_aiViewport[3]*0.5);
    }

    void Project(const float* srcv, float* &dstv)
    {
        double x, y, z;
        
        gluProject((GLdouble)srcv[0], (GLdouble)srcv[1], (GLdouble)srcv[2], 
                   m_adModelview, m_adProjection, m_aiViewport, 
                   &x, &y, &z);
                   
        dstv[0] = (float)(x-m_aiViewport[2]*0.5);
        dstv[1] = (float)(y-m_aiViewport[3]*0.5);
        dstv[2] = 0.0f;
    }

    void Project(const double srcx, const double srcy, const double srcz, double &dstx, double &dsty)
    {
        double z;
        
        gluProject(srcx, srcy, srcz, 
                   m_adModelview, m_adProjection, m_aiViewport, 
                   &dstx, &dsty, &z);
    }

    void Project(const double* srcv, double* &dstv)
    {
        gluProject(srcv[0], srcv[1], srcv[2], 
                   m_adModelview,  m_adProjection, m_aiViewport, 
                   &dstv[0], &dstv[1], &dstv[2]);
    }

public:
    void UnProject(const float srcx, const float srcy, const float srcz, float &dstx, float &dsty, float &dstz)
    {
        double x, y, z;
        
        gluUnProject((GLdouble)srcx, (GLdouble)srcy, (GLdouble)srcz, 
                     m_adModelview, m_adProjection, m_aiViewport, 
                     &x, &y, &z);
                     
        dstx = (float)(x-m_aiViewport[2]*0.5);
        dsty = (float)(y-m_aiViewport[3]*0.5);
        dstz = (float)z;
    }

    void UnProject(const float* srcv, float* &dstv)
    {
        double x, y, z;
        
        gluUnProject((GLdouble)srcv[0], (GLdouble)srcv[1], (GLdouble)srcv[2], 
                     m_adModelview,  m_adProjection, m_aiViewport, 
                     &x, &y, &z);
                     
        dstv[0] = (float)(x-m_aiViewport[2]*0.5);
        dstv[1] = (float)(y-m_aiViewport[3]*0.5);
        dstv[2] = (float)z;
    }

    void UnProject(const double srcx, const double srcy, const double srcz, double &dstx, double &dsty, double &dstz)
    {
        gluUnProject(srcx, srcy, srcz, 
                     m_adModelview,  m_adProjection, m_aiViewport, 
                     &dstx, &dsty, &dstz);
    }

    void UnProject(const double* srcv, double* &dstv)
    {
        gluUnProject(srcv[0], srcv[1], srcv[2], 
                     m_adModelview,  m_adProjection, m_aiViewport, 
                     &dstv[0], &dstv[1], &dstv[2]);
    }
};

}; // namespace GL

#endif //_CORE_GLMATRIXSAVER_INCLUDED
