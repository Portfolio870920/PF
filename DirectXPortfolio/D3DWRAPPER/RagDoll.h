// RagDoll.h: interface for the CRagDoll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAGDOLL_H__064AAFAA_B4E4_4661_958E_2BFB93FB1436__INCLUDED_)
#define AFX_RAGDOLL_H__064AAFAA_B4E4_4661_958E_2BFB93FB1436__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct	SPartial
{
	D3DXVECTOR3	vPos;
	float		fWeight;	// kg.
};
typedef	SPartial	*PPartial;

class CJoint
{
protected:
	PPartial	pChild;
	PPartial	pParent;

	CJoint*		pOverChild;
	CJoint*		pOverParent;

	D3DXVECTOR3	vLimitAng;

public:
	CJoint();
	virtual	~CJoint();
	D3DXQUATERNION	GetLocalRotation ( void );
};
typedef	CJoint	*CJoint;

class CRagDoll  
{
public:
	CRagDoll();
	virtual ~CRagDoll();
};

#endif // !defined(AFX_RAGDOLL_H__064AAFAA_B4E4_4661_958E_2BFB93FB1436__INCLUDED_)
