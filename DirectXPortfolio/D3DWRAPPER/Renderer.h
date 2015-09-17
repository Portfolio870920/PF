// Renderer.h: interface for the CRenderer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERER_H__50D58C4F_0BA4_4577_9C98_2F4FACCFE83E__INCLUDED_)
#define AFX_RENDERER_H__50D58C4F_0BA4_4577_9C98_2F4FACCFE83E__INCLUDED_

#include <list>
#include "D3Dx9.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRenderer  
{
protected:
	D3DXVECTOR3								m_vPos;
	D3DXVECTOR3								m_vRot;
	D3DXVECTOR3								m_vScl;

public:
	CRenderer();
	virtual ~CRenderer();

	virtual	bool			Init		( char* pFileName )	= 0;
	virtual	void			Free		( void )			= 0;
	virtual	void			Render		( void )			= 0;
	void					SetPos		( D3DXVECTOR3 vPos );
	void					SetRot		( D3DXVECTOR3 vRot );
	void					SetScl		( D3DXVECTOR3 vScl );
};
typedef	CRenderer	*PRenderer;

#endif // !defined(AFX_RENDERER_H__50D58C4F_0BA4_4577_9C98_2F4FACCFE83E__INCLUDED_)
