//윈도우 콘솔창 띄우기
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//
//
#define	CUSTOM_VERTEX2D		D3DFVF_XYZRHW|D3DFVF_TEX1
//
// 라이브러리
#pragma comment ( lib, "./SOUND/FMOD/fmodex_vc.lib") // fmod 라이브러리
#pragma comment ( lib, "winmm.lib")
#ifdef	_DEBUG
#pragma comment ( lib, "../D3DWRAPPER/Debug/D3DWRAPPER.lib" )
#else
#pragma comment ( lib, "../D3DWRAPPER/release/D3DWRAPPER.lib" )
#endif

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4244 )
//
// 기본헤더
#include <Windows.h>
#include <MMSystem.h>
#include <Time.h>
#include <vector>
#include <stdlib.h>
using namespace	std;

//
//사운드
#include "./SOUND/FMOD/inc/fmod.hpp"
using namespace FMOD;
#include "cSound.h"
//
#include "D3Dx9.h"
// 강사님 헤더파일
#include "../D3DWRAPPER/Terrain.h"
#include "../D3DWRAPPER/D3DWRAPPER.H"
#include "../D3DWRAPPER/SkinRenderer.H"
#include "../D3DWRAPPER/SkinController.H"
//
//	파티클
#include "../D3DWRAPPER/Particle.h"
#include "../D3DWRAPPER/ParticleSprite.h"
#include "pSystem.h"
//
// x파일
#include "cFileLoader.h"
//
// 카메라
#include "cCamera.h"
//
// 시간
#include "cTime.h"
//
// 키 매니저
#include "cKey.h"
//
// 충돌
#include "cCollision.h"
//
// 캐릭터 관련
//#include "character.h"
#include "cplayer.h"
#include "cMonster.h"
#include "cMobKnight.h"
#include "cMobMgr.h"
#include "MobAlduin.h"
//
// UI
#include "cUiBase.h"
//
// 씬 관련
#include "cScene.h"
#include "cIntro.h"
#include "cMenu.h"
#include "cGame.h"
//
// 옵션
#include "cOption.h"
//
// 씬 매니저
#include "cScene_Mgr.h"
//
// 텍스트 매니저 필요없을때는 주석처리
#include "cText.h"
//
// 리소스 매니저
#include "cResourceMgr.h"
// 데이터 메니저
#include "cDataMgr.h"
//

// 매니저 선언부
#define		SOUNDMGR	( cSound::GetInst() )		// 사운드 매니저
#define		KEYMGR		( cKey::GetInst() )			// 키 매니저
#define		SCENEMGR	( cScene_Mgr::GetInst() )	// 씬 매니저
#define		TIMEMGR		( cTimer::GetInst() )		// 시간 매니저
#define		CAMMGR		( CCamera::GetInst())		// 카메라 매니저
#define		MOBMGR		( cMobMgr::GetInst() )		// 몹 매니저
#define		CHARMGR		( Cplayer::GetInst() )		// 캐릭터 매니저
#define		MAPMGR		( CTerrain::GetInst() )		// 맵 매니저
#define		TEXTMGR		( cText::GetInst() )		// 텍스트 매니저 필요없을때는 주석처리
#define		DATAMGR		( CDataMgr::GetInst() )		// 데이터 매니저
#define		COPTION		( cOption::GetInst() )		// 옵션
#define		RSCMGR		( cResourceMgr::GetInst() )	// ReSourCe 매니저

//	STATUS

#define STATUS_STANCE				0x00000000		//	0
#define STATUS_MOVE					0x00000001		//	1
#define STATUS_VERTICAL				0x00000010		//	2
#define STATUS_BREATH				0x00000011		//	3
#define STATUS_HORIZON_LEFT			0x00000100		//	4
#define STATUS_LEFT_BREATH			0x00000101		//	5
#define STATUS_HORIZON_RIGHT		0x00001000		//	8
#define STATUS_RIGHT_BREATH			0x00001001		//	9
#define STATUS_JUMPCUT				0x00010000		//	16
#define STATUS_DOUBLE_STAB			0x00010001		//	17
#define STATUS_DOUBLE_CUT_BACKDASH	0X00010010		//	18
#define STATUS_JUMP_DRAGON_BREATH	0x00010100		//	20
#define STATUS_DIE					0x00100000		//	32
#define STATUS_BEHIT				0x00100001		//	34
#define	 STATUS_RUN					0x01000000		//  64
#define STATUS_DASH					0x01000001		//	65
#define STATUS_FIRST_SHOW			0x10000000		//	128



// 화면 크기 / 중점
#define		SCREENSIZE_X	1280
#define		SCREENSIZE_Y	960
#define 	CENTERPOS_X		( GetSystemMetrics( SM_CXSCREEN ) - SCREENSIZE_X ) / 2 + SCREENSIZE_X / 2	// X 중점
#define		CENTERPOS_Y		( GetSystemMetrics( SM_CYSCREEN ) - SCREENSIZE_Y ) / 2 + SCREENSIZE_Y / 2 	// Y 중점

// 마우스 회전 민감도
#define		MOUSE_SENSE		4