// SkinController.h: interface for the CSkinController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINCONTROLLER_H__3FDF9342_6C0C_417E_B292_558291E63156__INCLUDED_)
#define AFX_SKINCONTROLLER_H__3FDF9342_6C0C_417E_B292_558291E63156__INCLUDED_

#include "SkinRenderer.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ���Ͽ��� �о���� ������ ����Ҷ� ���̴� ���� ������.
struct SBONE_INSTANCE
{
	D3DXQUATERNION		qRotAfterTM;
	D3DXQUATERNION		qRotBeforeTM;
	D3DXMATRIX			mRotAfterTM;
	D3DXMATRIX			mRotBeforeTM;

	bool				bCalculated;
	D3DXMATRIX			dmSumRot;
	D3DXMATRIX			mWorld;
	D3DXVECTOR3			vMov;
	D3DXVECTOR3			vPivot;			//	ȸ�� �� ��ġ
	D3DXQUATERNION		qSumRot;
	D3DXQUATERNION		qCurRot;
};
typedef	SBONE_INSTANCE *PBONE_INSTANCE;

class CSkinController  
{
protected:
	CSkinRenderer*							m_pSkinSource;
	PBONE_INSTANCE							m_pBones;

	float									m_fCurrentFrame;
	float									m_fLastBlendingSpeed;
	D3DXVECTOR3								m_vPos;
	D3DXVECTOR3								m_vRot;
	D3DXVECTOR3								m_vScl;
	E_UPDATE_STATUS							m_eUpdateStatus;

	std::list<SMOTION>						m_listMotion;
	SMOTION									m_sReplaceMotion;
	std::vector<int>*						m_pListBones;

	LPD3DXMATRIX							m_pmWorlds;

	LPD3DXVECTOR3							m_pvSingleMotionPivot;
	LPD3DXQUATERNION						m_pqSingleMotionRotate;

	void									UpdateWorldMatrix		( void );										// ��Ǹ���Ʈ�� ���� ����� ��������� ���� ȸ��������� �̵����͸� ������ �д�.
	void									UpdateSingleMotion		( std::list<SMOTION>::iterator* ppNode = NULL );// ��Ǹ���Ʈ 1���� ����Ѵ�.
	D3DXQUATERNION							UnitedRotate			( int i );										// UpdateWorldMatrix���� ���� ������ ��ǿ��� ȸ������ ����ȸ�������� ��ȯ.
	D3DXVECTOR3								RepositionPivot			( int i );										// ȸ���Ǿ� �̵��� �ǹ��� ����Ѵ�.
	void									TransformObject			( int i );										// �̵��� �ǹ��� ȸ��������� ������ǥ��ķ� ��ȯ��Ų��.
	E_UPDATE_STATUS							UpdateFrameFoward		( PMOTION pMotion );
	E_UPDATE_STATUS							UpdateFrameBackward		( PMOTION pMotion );
	E_UPDATE_STATUS							UpdateFrameNoLoop		( PMOTION pMotion );
	void									ReplaceBoneTransform	( void );
	void									CombineAfterTransform	( void );

public:
	CSkinController();
	virtual ~CSkinController();

	unsigned int			Init			( CSkinRenderer* pSkinObj );
	void					Free			( void );
	void					Render			( bool BeHitCheck, float BuffCheck );

	//Me�߰�
	D3DXVECTOR3				GetPos			( void ) {return m_vPos;} 

	void					SetPos			( D3DXVECTOR3 vPos );
	void					SetRot			( D3DXVECTOR3 vRot );
	void					SetScl			( D3DXVECTOR3 vScl );
	void					ClearMotions	( void );

	void					HideObject			( int nObjNo = -1, bool dHide = false );
	void					ShowObject			( int nObjNo = -1, bool dShow = true );
	E_UPDATE_STATUS			SetRange			( int fFrameS, int fFrameD, float fRepeatInterval, float fBlendingSpeed = 1.0f, bool bReset = true );
	E_UPDATE_STATUS			SetRangeNoLoop		( int fFrameS, int fFrameD, int fFrameSInner, int fFrameDInner, float fRepeatInterval, float fBlendingSpeed = 1.0f );
	E_UPDATE_STATUS			MixRangeBones		( int fFrameS, int fFrameD, float fRepeatInterval, std::vector<int>* pVecBones, float fOverlayRate = 0.5f );
	E_UPDATE_STATUS			MixRangeNoLoopBones	( int fFrameS, int fFrameD, float fRepeatInterval, std::vector<int>* pVecBones, float fOverlayRate = 0.5f );
	void					UpdateFrame			( void );

	int						GetObjNoFromName( char* szName );
	int						GetBoneNoFromName( char* szName );
	__inline	PBONES			GetBoneInfo		( void ){return m_pSkinSource->m_pBone;}
	__inline	PBONE_INSTANCE	GetIBoneInfo	( void ){return m_pBones;}
	__inline	int				GetObjCount		( void ){return m_pSkinSource->nObjCount;}
	__inline	int				GetBoneCount	( void ){return m_pSkinSource->nBoneCount;}
	__inline	CSkinRenderer*	GetSkinSource(void ){return m_pSkinSource;}
	__inline	float			GetCurFrame		( void ){return m_fCurrentFrame;}
	__inline	D3DXMATRIX		GetWorldMatrix	( void ){return m_pSkinSource->mRootWorld;}
};

#endif // !defined(AFX_SKINCONTROLLER_H__3FDF9342_6C0C_417E_B292_558291E63156__INCLUDED_)
