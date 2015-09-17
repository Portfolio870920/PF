// Flame.h: interface for the CFlame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLAME_H__AB493F76_444A_41DC_8909_2D0EC35BB7D0__INCLUDED_)
#define AFX_FLAME_H__AB493F76_444A_41DC_8909_2D0EC35BB7D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFlame  
{
protected:
	static	LPD3DXEFFECT					s_pEffect;
	static	LPDIRECT3DVERTEXDECLARATION9	s_pDecl;
	static	LPDIRECT3DVERTEXBUFFER9			s_pVBuffer;
	LPDIRECT3DTEXTURE9		m_pTexDiffuse;
	LPDIRECT3DTEXTURE9		m_pTexNormal;

	D3DXVECTOR3				m_vPos;
	D3DXVECTOR3				m_vScl;
public:
	CFlame();
	virtual ~CFlame();

	static	void	FxInit	( LPSTR szFxName );
	bool			Init	( LPSTR szDifName, LPSTR NorName );
	void			Free	( void );
	void			SetPos	( D3DXVECTOR3 vPos );
	void			SetScl	( D3DXVECTOR3 vScl );

	void			Draw	( void );
};

#endif // !defined(AFX_FLAME_H__AB493F76_444A_41DC_8909_2D0EC35BB7D0__INCLUDED_)
