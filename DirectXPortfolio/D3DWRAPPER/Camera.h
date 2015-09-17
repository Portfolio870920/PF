// Camera.h: interface for the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERA_H__D2EC5D93_459E_4D6C_8386_09DE5C99B593__INCLUDED_)
#define AFX_CAMERA_H__D2EC5D93_459E_4D6C_8386_09DE5C99B593__INCLUDED_

#include "D3DWRAPPER.h"
#include "D3Dx9.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCamera  
{
protected:
	D3DXVECTOR3	m_vCameraPos;
	D3DXVECTOR3	m_vCameraTgt;

	D3DXMATRIX	m_mCamMatrix;
	D3DXMATRIX	m_mLensMatrix;
public:
	CCamera();
	virtual ~CCamera();

	static	CCamera*	m_pInstance;
	static	CCamera*	GetInstance ( void );

	__inline	D3DXMATRIX			GetViewMatrix	( void ){return m_mCamMatrix;}
	__inline	D3DXMATRIX			GetProjMatrix	( void ){return m_mLensMatrix;}
	__inline	D3DXVECTOR3			GetCameraPos	( void ){return m_vCameraPos;}
	__inline	D3DXVECTOR3			GetCameraTgt	( void ){return m_vCameraTgt;}
};

typedef	CCamera	*PCamera;
#define CAMERA	(CCamera::GetInstance())

#endif // !defined(AFX_CAMERA_H__D2EC5D93_459E_4D6C_8386_09DE5C99B593__INCLUDED_)
