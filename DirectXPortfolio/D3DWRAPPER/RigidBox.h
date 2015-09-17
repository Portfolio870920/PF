// RigidBox.h: interface for the CRigidBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIGIDBOX_H__40D98D6B_D8AF_4F69_81B1_25F69F3A481F__INCLUDED_)
#define AFX_RIGIDBOX_H__40D98D6B_D8AF_4F69_81B1_25F69F3A481F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Rigid.h"

class CRigidBox : public CRigid  
{
protected:
	D3DXVECTOR3		m_vSize;
	D3DXVECTOR3		m_vMoment;
	float			m_fMass;

	D3DXVECTOR3		m_vPointPos[8];
	D3DXVECTOR3		m_vPointVel[8];

public:
	CRigidBox();
	virtual ~CRigidBox();

	void	SetBox	( float fSizeX, float fSizeY, float fSizeZ, float fMass );
	void	Update	( float fDelta );
	void	RenderDebug	( void );
};
typedef	CRigidBox	*PRigidBox;

#endif // !defined(AFX_RIGIDBOX_H__40D98D6B_D8AF_4F69_81B1_25F69F3A481F__INCLUDED_)
