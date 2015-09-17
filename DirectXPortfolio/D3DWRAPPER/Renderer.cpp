// Renderer.cpp: implementation of the CRenderer class.
//
//////////////////////////////////////////////////////////////////////

#include "Renderer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderer::CRenderer()
{
}

CRenderer::~CRenderer()
{
}

void	CRenderer::SetPos		( D3DXVECTOR3 vPos )
{
	m_vPos = vPos;
}

void	CRenderer::SetRot		( D3DXVECTOR3 vRot )
{
	m_vRot = vRot;
}

void	CRenderer::SetScl		( D3DXVECTOR3 vScl )
{
	m_vScl = vScl;
}
