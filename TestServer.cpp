
// TestServer.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "TestServer.h"
#include "TestServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestServerApp

BEGIN_MESSAGE_MAP(CTestServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestServerApp 생성

CTestServerApp::CTestServerApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CTestServerApp 개체입니다.

CTestServerApp theApp;


// CTestServerApp 초기화

BOOL CTestServerApp::InitInstance()
{
	CWinApp::InitInstance();
	
	// 지금부터 소켓을 사용함.
	// WSA(Window Socket API의 약자)
	WSADATA temp; // WSADATA 구조체 변수 선언
	WSAStartup(0x0202, &temp);

	CTestServerDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	
	// 소켓을 더이상 사용하지 않을 때 호출함.
	WSACleanup();

	return FALSE;
}

