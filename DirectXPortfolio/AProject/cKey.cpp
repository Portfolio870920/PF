#include "MainHeader.h"

cKey		*cKey::m_pInstance	=	NULL;

cKey::cKey(void)
{
	ZeroMemory( m_bKeyDown, KEYMAX );
	ZeroMemory( m_bKeyUp, KEYMAX );
}

cKey::~cKey(void)
{
}

cKey*	cKey::GetInst(VOID)
{
	if( m_pInstance == NULL )
	{
		m_pInstance	=	new cKey;
		RSCMGR->m_bKEY_MGR	=	true;
	}

	return m_pInstance;
}
VOID	cKey::FreeInst(VOID)
{
	if( m_pInstance != NULL )
	{
		delete m_pInstance;
		m_pInstance	=	NULL;
		RSCMGR->m_bKEY_MGR	=	false;
	}
}

//	지속 눌림
bool	cKey::StayKeyDown( INT nKey )
{
	if( GetAsyncKeyState( nKey ) & 0x8000 )
	{
		return TRUE;
	}

	return FALSE;
}

//	한번만 다운 키
bool	cKey::OnceKeyDown( INT nKey )
{
	if( GetAsyncKeyState( nKey ) & 0x8000 )
	{
		//	한번 눌렀으면?
		if( m_bKeyDown[nKey] == false )
		{
			m_bKeyDown[nKey]	=	true;
			return TRUE;
		}
	}

	else
	{
		//	클리어
		m_bKeyDown[nKey]	=	false;
	}	

	return FALSE;
}

//	한번만 업 키
bool	cKey::OnceKeyUp( INT nKey )
{
	if( GetAsyncKeyState( nKey ) & 0x8000 )
	{	
		m_bKeyUp[nKey]	=	true;
	}

	else
	{
		//	누르고 있었냐?
		if( m_bKeyUp[nKey] == true )
		{
			//	클리어
			m_bKeyUp[nKey]	=	false;

			return TRUE;
		}
	}	

	return FALSE;
}

//	토글키
bool	cKey::IsToggleKey( INT nKey )
{
	if( GetKeyState(nKey) & 0x0001 )
	{
		return TRUE;
	}

	return FALSE;
}