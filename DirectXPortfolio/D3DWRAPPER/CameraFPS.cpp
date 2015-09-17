// CameraFPS.cpp: implementation of the CCameraFPS class.
//
//////////////////////////////////////////////////////////////////////

#include "CameraFPS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameraFPS::CCameraFPS()
{
}

CCameraFPS::~CCameraFPS()
{
}

D3DXMATRIX	CCameraFPS::SetCamera	( D3DXVECTOR3 vPos, float fAngleY, float fAngleX, float fDist, float fFov, float fNear, float fFar, float fDamper )
{
	D3DXVECTOR3						vCamTgt = vPos, vCamPos, vOffset;
	D3DXMATRIX						mRot, mRotX, mRotY;
	D3DDEVICE_CREATION_PARAMETERS	Param;
	RECT							rect;

	float		fAspect;
	D3DFRAME->GetDevice()->GetCreationParameters( &Param );
	GetClientRect ( Param.hFocusWindow, &rect );

	fAspect = (float)(rect.right-rect.left)/(float)(rect.bottom-rect.top);

	D3DXMatrixRotationX( &mRotX, fAngleX );
	D3DXMatrixRotationY( &mRotY, fAngleY );
	mRot		= mRotX * mRotY;

	D3DXVec3TransformCoord( &vOffset, &D3DXVECTOR3(0.0f, 0.0f, fDist), &mRot );

	D3DXVECTOR3	vCamPosDest = vCamTgt + vOffset;
	D3DXVECTOR3	vCamTgtDest = vCamTgt;

	m_vCameraPos	+= (vCamPosDest-m_vCameraPos)*fDamper;
	m_vCameraTgt	+= (vCamTgtDest-m_vCameraTgt)*fDamper;
	D3DXMatrixLookAtLH			( &m_mCamMatrix, &(m_vCameraPos), &(m_vCameraTgt), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	D3DXMatrixPerspectiveFovLH	( &m_mLensMatrix, fFov, fAspect, fNear, fFar );

	D3DFRAME->SetCamera		((m_vCameraPos), (m_vCameraTgt), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	D3DFRAME->SetPerspective(fFov, fAspect, fNear, fFar );
	return m_mCamMatrix;
}
