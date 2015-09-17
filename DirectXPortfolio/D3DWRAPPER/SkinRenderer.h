// SkinRenderer.h: interface for the CSkinRenderer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINRENDERER_H__D3A3F1F2_AA9B_44A0_8814_57EF1FAA076C__INCLUDED_)
#define AFX_SKINRENDERER_H__D3A3F1F2_AA9B_44A0_8814_57EF1FAA076C__INCLUDED_

#include <list>
#include <vector>
#include "D3Dx9.h"
#include "../SkinLoader/SkinLoader.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum	E_UPDATE_STATUS
{
	UPDATE_PLAYING,
	UPDATE_RETURN,
	UPDATE_PAUSE,
	UPDATE_RATE_IS_ZERO,
};

struct SBONES
{
	std::string			strName;

	D3DXVECTOR3			vPivot0;
	D3DXMATRIX			mBasis;

	int					nParent;

	LPD3DXQUATERNION	pqRot;
	LPD3DXVECTOR3		pvMov;

// 파일에서 읽지는 않지만 계산상 필요한 정보들.
	D3DXMATRIX			mInvBasis;
	D3DXQUATERNION		qInvBasis;
	D3DXQUATERNION		qBasis;

// 실제 파일에서 불러오지는 않지만 애니메이션시 필요한 인자들.
/*	D3DXQUATERNION		qRotAfterTM;
	D3DXQUATERNION		qRotBeforeTM;
	D3DXMATRIX			mRotAfterTM;
	D3DXMATRIX			mRotBeforeTM;

	bool				bCalculated;
	D3DXMATRIX			dmSumRot;
	D3DXMATRIX			mWorld;
	D3DXVECTOR3			vMov;
	D3DXVECTOR3			vPivot;
	D3DXQUATERNION		qSumRot;
	D3DXQUATERNION		qCurRot;
*/
};
typedef	SBONES *PBONES;

struct	SMOTION
{
	bool			m_bIncrease;
	bool			m_bLooped;
	float			m_dwStart;
	float			m_fCurFrame;
	float			m_fSlerp;
	int				m_nThisFrame;
	int				m_nNextFrame;

	int				m_nFrameS;
	int				m_nFrameD;
	int				m_nFrameSIn;
	int				m_nFrameDIn;
	float			m_fSpeed;

	float			m_fBlendingRate;
	float			m_fBlendingSpeed;
	E_UPDATE_STATUS	CalcCurrentFrame ( void );
};
typedef SMOTION *PMOTION;

class CSkinRenderer
{
protected:
	static	CSkinLoader						s_cSkinLoader;
	float									m_fLastBlendingSpeed;
	D3DXVECTOR3								m_vPos;
	D3DXVECTOR3								m_vRot;
	D3DXVECTOR3								m_vScl;
	D3DXVECTOR3								m_vColor;	//	피격시 모델의 색상을 바꾸기 위해
	E_UPDATE_STATUS							m_eUpdateStatus;

	std::list<SMOTION>						m_listMotion;
	SMOTION									m_sReplaceMotion;
	std::vector<int>*						m_pListBones;

	LPD3DXMATRIX							m_pmWorlds;

	void									CreateTangentSpace		( int nVCount, PCUSTOMSKINVERTEX pVIn, int nICount, LPWORD pIIn, LPD3DXVECTOR3 pTan );
	void									DevideBonesByObject		( int i );										// 실제 오브젝트에 할당된 본들만 추려낸다.

public:
	D3DXMATRIX		mRootWorld;
	static	LPD3DXEFFECT	s_pEffect;
	static	LPDIRECT3DVERTEXDECLARATION9	s_pVDecl;
	static	bool	LoadEffect ( char* pFileName );

	bool					Init			( char* pFileName );
	void					Free			( void );

	void					Render			( bool BeHitCheck, float BuffCheck );
	void					SetPos			( D3DXVECTOR3 vPos );
	void					SetRot			( D3DXVECTOR3 vRot );
	void					SetScl			( D3DXVECTOR3 vScl );
	void					ClearMotions	( void );

	void					HideObject			( int nObjNo = -1, bool dHide = false );
	void					ShowObject			( int nObjNo = -1, bool dShow = true );
//	E_UPDATE_STATUS			SetRange			( int fFrameS, int fFrameD, float fRepeatInterval, float fBlendingSpeed = 1.0f, bool bReset = true );
//	E_UPDATE_STATUS			SetRangeNoLoop		( int fFrameS, int fFrameD, int fFrameSInner, int fFrameDInner, float fRepeatInterval, float fBlendingSpeed = 1.0f );
//	E_UPDATE_STATUS			MixRangeBones		( int fFrameS, int fFrameD, float fRepeatInterval, std::vector<int>* pVecBones, float fOverlayRate = 0.5f );
//	E_UPDATE_STATUS			MixRangeNoLoopBones	( int fFrameS, int fFrameD, float fRepeatInterval, std::vector<int>* pVecBones, float fOverlayRate = 0.5f );
//	void					UpdateFrame			( void );

	int						GetObjNoFromName( char* szName );
	int						GetBoneNoFromName( char* szName );
	__inline	PBONES		GetObjInfos		( void ){return m_pBone;}
	__inline	int			GetObjCount		( void ){return nObjCount;}
	__inline	int			GetBoneCount	( void ){return nBoneCount;}
	__inline	void		SetWorldMatrix	( LPD3DXMATRIX mMatrix ){m_pmWorlds = mMatrix;}

public:
	CSkinRenderer();
	virtual ~CSkinRenderer();

	int	nObjCount;
	int	nMatCount;
	int	nTexCount;
	int	nBoneCount;
	int	nKeyCount;

	LPDIRECT3DVERTEXBUFFER9*	m_ppVB;
	LPDIRECT3DVERTEXBUFFER9*	m_ppVT;

	LPDIRECT3DINDEXBUFFER9*		m_ppIB;
	LPDIRECT3DTEXTURE9*			m_ppTB;
	PSKINMATERIAL				m_pMat;

	// Mesh.
	bool*						m_pbShow;
	int*						m_pnVNum;
	int*						m_pnINum;
	int*						m_pnMatID;
	D3DXMATRIX					m_mWorld;

	std::vector<byte>*			m_pVecLocalBoneIndex;

	// Bone Animations.
	PBONES						m_pBone;
//	LPD3DXVECTOR3				m_pvSingleMotionPivot;
//	LPD3DXQUATERNION			m_pqSingleMotionRotate;
};

#endif // !defined(AFX_SKINRENDERER_H__D3A3F1F2_AA9B_44A0_8814_57EF1FAA076C__INCLUDED_)
