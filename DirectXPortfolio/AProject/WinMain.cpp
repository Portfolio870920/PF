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
	//int   Width=800,Height=600;			//	��ȣ�� ������ ������� �� ��

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

	// ���ҽ� �Ŵ��� �ʱ�ȭ
	RSCMGR->Init();
	// ����̽� �ʱ�ȭ
	D3DFRAME->Init( hWnd, Width, Height, D3DFMT_X8R8G8B8, FALSE );
	D3DFRAME->SetLightDir( D3DXVECTOR3(0.5f, -1.0f, 0.0f));
	//D3DFRAME->SetLightDir( D3DXVECTOR3(120.0f, 700.0f, 130.0f));
	// ���� �ʱ�ȭ
	SOUNDMGR->Init();
	// �� �ʱ�ȭ
	SCENEMGR->Init(hWnd);
	// �ɼ� �ʱ�ȭ
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
			// 0(�ѹ��е�ƴ�) ������ ����
			if( KEYMGR->StayKeyDown('0')|| SCENEMGR->GetGameEnd() ) 
			{ 
				ActiveFlag	=	FALSE;
				roop		=	FALSE;
			}

			// �� ����κ�
			if ( SCENEMGR->GetbSceneChange() )
			{
				SCENEMGR->SceneChange(SCENEMGR->GetRsvScene());
			}

			TIMEMGR->Tick(60.0f); // float ��ŭ ���Լ����� �����ִٰ� ���� �� ����
			SCENEMGR->Update();
			D3DFRAME->GetDevice()->BeginScene();
			SCENEMGR->Render();
			D3DFRAME->GetDevice()->EndScene();
			D3DFRAME->GetDevice()->Present ( NULL, NULL, NULL, NULL );
		}
	}
	RSCMGR->AllFree();	// ��� �Ŵ��� �ν��Ͻ� �ʱ�ȭ
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