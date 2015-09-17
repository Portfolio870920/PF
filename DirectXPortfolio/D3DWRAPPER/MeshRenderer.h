// MeshRenderer.h: interface for the CMeshRenderer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHRENDERER_H__63C5A35B_5C46_4F9B_AB63_06EAF3EC8950__INCLUDED_)
#define AFX_MESHRENDERER_H__63C5A35B_5C46_4F9B_AB63_06EAF3EC8950__INCLUDED_

#include "D3Dx9.h"
#include "../MeshLoader/MeshLoader.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMeshRenderer  
{
protected:
	D3DXVECTOR3								m_vPos;
	D3DXVECTOR3								m_vRot;
	D3DXVECTOR3								m_vScl;

	LPD3DXVECTOR3							m_pvPos;
	LPD3DXVECTOR3							m_pvRot;
	LPD3DXVECTOR3							m_pvScl;
public:
	static	CMeshLoader						s_cMeshLoader;
	static	LPD3DXEFFECT					s_pEffect;
	static	LPDIRECT3DVERTEXDECLARATION9	s_pVDecl;

	static	bool			LoadEffect ( char* pFileName );

	bool					Init		( char* pFileName );
	void					Free		( void );

	void					Render		( void );
	void					SetPos		( D3DXVECTOR3 vPos, int n = -1 );
	void					SetRot		( D3DXVECTOR3 vRot, int n = -1 );
	void					SetScl		( D3DXVECTOR3 vScl, int n = -1 );
public:
	CMeshRenderer();
	virtual ~CMeshRenderer();

	int	nObjCount;
	int	nMatCount;

	LPDIRECT3DVERTEXBUFFER9*	m_ppVB;
	LPDIRECT3DINDEXBUFFER9*		m_ppIB;
	LPDIRECT3DTEXTURE9*			m_ppTB;

	int*						m_pnVNum;
	int*						m_pnINum;
	int*						m_pnMatID;
	LPD3DXMATRIX				m_pmMatrix;
    LPD3DXVECTOR3				m_pvPivot;
	D3DXMATRIX					m_mWorld;
};

#endif // !defined(AFX_MESHRENDERER_H__63C5A35B_5C46_4F9B_AB63_06EAF3EC8950__INCLUDED_)
