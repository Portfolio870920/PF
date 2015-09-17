#include "MainHeader.h"

int ActiveFlag = TRUE;	

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass=TEXT("AProject");

int APIENTRY WinMain (HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	
	MSG msg;
	int roop=1;

	WNDCLASS WndClass;
	g_hInst=hInstance;
	int   Width=SCREENSIZE_X,Height=SCREENSIZE_Y;
	//int   Width=800,Height=600;			//	준호네 집에서 디버그할 때 씀

	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=(WNDPROC)WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW;x`
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass, lpszClass, WS_SYSMENU/*WS_EX_TOPMOST | WS_POPUP*/, (GetSystemMetrics(SM_CXSCREEN)-Width)/2, (GetSystemMetrics(SM_CYSCREEN)-Height)/2, Width, Height, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd,nCmdShow);

	// 리소스 매니저 초기화
	RSCMGR->Init();
	// 디바이스 초기화
	D3DFRAME->Init( hWnd, Width, Height, D3DFMT_X8R8G8B8, FALSE );
	D3DFRAME->SetLightDir( D3DXVECTOR3(0.5f, -1.0f, 0.0f));
	//D3DFRAME->SetLightDir( D3DXVECTOR3(120.0f, 700.0f, 130.0f));
	// 사운드 초기화
	SOUNDMGR->Init();
	// 씬 초기화
	SCENEMGR->Init(hWnd);
	// 옵션 초기화
	COPTION->Init(hWnd);
	while(roop)
	{
		while(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)!=0)
		{
			if(GetMessage(&msg,NULL,0,0)==0) roop=0;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if(ActiveFlag==TRUE)
		{
			// 0(넘버패드아님) 누를시 종료
			if( KEYMGR->StayKeyDown('0')|| SCENEMGR->GetGameEnd() ) 
			{ 
				ActiveFlag	=	FALSE;
				roop		=	FALSE;
			}

			// 씬 변경부분
			if ( SCENEMGR->GetbSceneChange() )
			{
				SCENEMGR->SceneChange(SCENEMGR->GetRsvScene());
			}

			TIMEMGR->Tick(60.0f); // float 만큼 이함수에서 멈춰있다가 다음 줄 진행
			SCENEMGR->Update();
			D3DFRAME->GetDevice()->BeginScene();
			SCENEMGR->Render();
			D3DFRAME->GetDevice()->EndScene();
			D3DFRAME->GetDevice()->Present ( NULL, NULL, NULL, NULL );
		}
	}
	RSCMGR->AllFree();	// 모든 매니저 인스턴스 초기화
	PostQuitMessage(0);
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch(iMessage) {
	case WM_CREATE:
		ActiveFlag =TRUE;
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;	
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}