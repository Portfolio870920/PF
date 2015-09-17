//������ �ܼ�â ����
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//
//
#define	CUSTOM_VERTEX2D		D3DFVF_XYZRHW|D3DFVF_TEX1
//
// ���̺귯��
#pragma comment ( lib, "./SOUND/FMOD/fmodex_vc.lib") // fmod ���̺귯��
#pragma comment ( lib, "winmm.lib")
#ifdef	_DEBUG
#pragma comment ( lib, "../D3DWRAPPER/Debug/D3DWRAPPER.lib" )
#else
#pragma comment ( lib, "../D3DWRAPPER/release/D3DWRAPPER.lib" )
#endif

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4244 )
//
// �⺻���
#include <Windows.h>
#include <MMSystem.h>
#include <Time.h>
#include <vector>
#include <stdlib.h>
using namespace	std;

//
//����
#include "./SOUND/FMOD/inc/fmod.hpp"
using namespace FMOD;
#include "cSound.h"
//
#include "D3Dx9.h"
// ����� �������
#include "../D3DWRAPPER/Terrain.h"
#include "../D3DWRAPPER/D3DWRAPPER.H"
#include "../D3DWRAPPER/SkinRenderer.H"
#include "../D3DWRAPPER/SkinController.H"
//
//	��ƼŬ
#include "../D3DWRAPPER/Particle.h"
#include "../D3DWRAPPER/ParticleSprite.h"
#include "pSystem.h"
//
// x����
#include "cFileLoader.h"
//
// ī�޶�
#include "cCamera.h"
//
// �ð�
#include "cTime.h"
//
// Ű �Ŵ���
#include "cKey.h"
//
// �浹
#include "cCollision.h"
//
// ĳ���� ����
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
// �� ����
#include "cScene.h"
#include "cIntro.h"
#include "cMenu.h"
#include "cGame.h"
//
// �ɼ�
#include "cOption.h"
//
// �� �Ŵ���
#include "cScene_Mgr.h"
//
// �ؽ�Ʈ �Ŵ��� �ʿ�������� �ּ�ó��
#include "cText.h"
//
// ���ҽ� �Ŵ���
#include "cResourceMgr.h"
// ������ �޴���
#include "cDataMgr.h"
//

// �Ŵ��� �����
#define		SOUNDMGR	( cSound::GetInst() )		// ���� �Ŵ���
#define		KEYMGR		( cKey::GetInst() )			// Ű �Ŵ���
#define		SCENEMGR	( cScene_Mgr::GetInst() )	// �� �Ŵ���
#define		TIMEMGR		( cTimer::GetInst() )		// �ð� �Ŵ���
#define		CAMMGR		( CCamera::GetInst())		// ī�޶� �Ŵ���
#define		MOBMGR		( cMobMgr::GetInst() )		// �� �Ŵ���
#define		CHARMGR		( Cplayer::GetInst() )		// ĳ���� �Ŵ���
#define		MAPMGR		( CTerrain::GetInst() )		// �� �Ŵ���
#define		TEXTMGR		( cText::GetInst() )		// �ؽ�Ʈ �Ŵ��� �ʿ�������� �ּ�ó��
#define		DATAMGR		( CDataMgr::GetInst() )		// ������ �Ŵ���
#define		COPTION		( cOption::GetInst() )		// �ɼ�
#define		RSCMGR		( cResourceMgr::GetInst() )	// ReSourCe �Ŵ���

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



// ȭ�� ũ�� / ����
#define		SCREENSIZE_X	1280
#define		SCREENSIZE_Y	960
#define 	CENTERPOS_X		( GetSystemMetrics( SM_CXSCREEN ) - SCREENSIZE_X ) / 2 + SCREENSIZE_X / 2	// X ����
#define		CENTERPOS_Y		( GetSystemMetrics( SM_CYSCREEN ) - SCREENSIZE_Y ) / 2 + SCREENSIZE_Y / 2 	// Y ����

// ���콺 ȸ�� �ΰ���
#define		MOUSE_SENSE		4