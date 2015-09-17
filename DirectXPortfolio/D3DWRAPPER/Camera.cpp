// Camera.cpp: implementation of the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "Camera.h"

CCamera*	CCamera::m_pInstance = NULL;
CCamera*	CCamera::GetInstance(void)
{
	if ( !m_pInstance )	m_pInstance = new CCamera;
	return m_pInstance;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCamera::CCamera()
{
	m_vCameraPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vCameraTgt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

CCamera::~CCamera()
{
}
