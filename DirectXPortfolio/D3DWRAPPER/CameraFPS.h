// CameraFPS.h: interface for the CCameraFPS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERAFPS_H__1A95E51E_B03E_466B_81AC_75E91CB79F78__INCLUDED_)
#define AFX_CAMERAFPS_H__1A95E51E_B03E_466B_81AC_75E91CB79F78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Camera.h"

class CCameraFPS : public CCamera  
{
public:
	CCameraFPS();
	virtual ~CCameraFPS();

	D3DXMATRIX	SetCamera	( D3DXVECTOR3 vPos, float fAngleY, float fAngleX, float fDist, float fFov, float fNear, float fFar, float fDamper = 0.1f );
};
typedef	CCameraFPS	*PCameraFPS;

#endif // !defined(AFX_CAMERAFPS_H__1A95E51E_B03E_466B_81AC_75E91CB79F78__INCLUDED_)
