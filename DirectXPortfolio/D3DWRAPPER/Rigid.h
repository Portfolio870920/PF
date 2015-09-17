// Rigid.h: interface for the CRigid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIGID_H__698DC0F8_B2BE_4DAF_8162_90F52318EF66__INCLUDED_)
#define AFX_RIGID_H__698DC0F8_B2BE_4DAF_8162_90F52318EF66__INCLUDED_

#include "D3DWRAPPER.h"
#include <list>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct ANGLUAR_PARAM
{
	D3DXVECTOR3	vAngleVelAxis;
	D3DXVECTOR3	vAngleVel;
};

class CRigid  
{
protected:
	float						m_fMoment;
	D3DXMATRIX					m_mTransform;
	D3DXVECTOR3					m_vPosition;
	D3DXMATRIX					m_mRotation;

	D3DXVECTOR3					m_vVelocity;
	std::list<ANGLUAR_PARAM>	m_listAngular;
//	D3DXVECTOR3					m_vAngVelAxis;
//	float						m_fAngVel;

	D3DXVECTOR3			m_vTorque;

	D3DXVECTOR3			m_vEnergy;
	D3DXVECTOR3			m_vForcePoint;

	float				m_fFricAng;
	float				m_fFricPos;
public:
	static	LPD3DXMESH	m_pDebugMesh;
	CRigid();
	virtual ~CRigid();

	__inline	void	SetFriction	( float	fAng, float fPos )							{m_fFricAng = fAng; m_fFricPos = fPos;}
	__inline	void	AddEnergy	( D3DXVECTOR3 vEnergy, D3DXVECTOR3 vForcePoint )	{m_vEnergy = vEnergy; m_vForcePoint = vForcePoint;}
	__inline	void	SetMoment	( float fM )										{m_fMoment = fM;}
	virtual		void	Update		( float fDelta )									= 0;
	virtual		void	RenderDebug	( void )											= 0;
};

typedef	CRigid	*PRigid;
#endif // !defined(AFX_RIGID_H__698DC0F8_B2BE_4DAF_8162_90F52318EF66__INCLUDED_)
