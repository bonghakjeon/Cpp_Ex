
// TestServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "TestServer.h"
#include "TestServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestServerDlg 대화 상자



CTestServerDlg::CTestServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_client_count = 0;
}

void CTestServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_event_list);
}

BEGIN_MESSAGE_MAP(CTestServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CTestServerDlg 메시지 처리기

BOOL CTestServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// AF_INET이라는 주소체계를 사용하는 소켓을 생성함.
	// SOCK_STREAM : TCP 통신의 한 종류
	mh_listen_socket = socket(AF_INET, SOCK_STREAM, 0); // TCP

	struct sockaddr_in srv_addr;
	srv_addr.sin_family = AF_INET;
	// 127.0.0.1은 테스트할 때 쓰는 ip 주소임.
	// 실제로 서비스 하려면 본인의 컴퓨터 ip 주소를 입력해야함.
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // ipconfig
	srv_addr.sin_port = htons(20001);

	bind(mh_listen_socket, (LPSOCKADDR)&srv_addr, sizeof(srv_addr));
	// bind(mh_listen_socket, (SOCKADDR *)&srv_addr, sizeof(srv_addr));
	// 와도 동일함.

	// 외부에서 접속할 수 있도록 기능이 추가됨.
	// , 1의 의미 : 한번에 받을 수 있는 대기자의 수를 1로 함. 
	listen(mh_listen_socket, 1);
	AddEventString(L"클라이언트 접속을 허가합니다. . .");

	// FD_ACCEPT : 어떤 클라이언트가 서버로 접속한다는 이벤트를 의미함. 
	WSAAsyncSelect(mh_listen_socket, m_hWnd, 27001, FD_ACCEPT); 

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTestServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTestServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestServerDlg::AcceptProcess(SOCKET param_h_socket)
{
	if (MAX_CLIENT_COUNT > m_client_count) 
	{
		
		// 클라이언트 주소체계를 받기 위하여 구조체 변수 client_addr 선언
		struct sockaddr_in client_addr;
		int sockaddr_in_size = sizeof(client_addr);

		mh_client_list[m_client_count] = accept(param_h_socket, (SOCKADDR*)&client_addr, &sockaddr_in_size);

		WSAAsyncSelect(mh_client_list[m_client_count], m_hWnd, 27002, FD_READ | FD_CLOSE);

		CString ip_address;

		// 클라이언트의 주소를 ip_address에 저장함.
		ip_address = inet_ntoa(client_addr.sin_addr);
		
		// m_clinet_ip라는 배열에 현재 접속한 사용자의 ip를 보관함.
		wcscpy(m_client_ip[m_client_count], ip_address);

		AddEventString(L"새로운 클라이언트가 접속했습니다 : " + ip_address);
	
		m_client_count++;
	}
	else
	{

	}

}

void CTestServerDlg::AddEventString(CString param_string)
{
	int index = m_event_list.InsertString(-1, param_string);
	m_event_list.SetCurSel(index);
}

void CTestServerDlg::ClientCloseProcess(SOCKET param_h_socket, char param_force_flag)
{
	if (param_force_flag == 1)
	{
		LINGER temp_linger = { TRUE, 0 };

		// 소켓에 걸려 있는 여러가지 옵션을 바꾸는 함수 호출
        // 닫을려고 하는 소켓(param_h_socket)의 linger 옵션을 바꾼다는 의미임.
		setsockopt(param_h_socket, SOL_SOCKET, SO_LINGER, (char *)&temp_linger, sizeof(temp_linger));
	}

	closesocket(param_h_socket);

	for (int i = 0; i < m_client_count; i++)
	{
		if (mh_client_list[i] == param_h_socket)
		{
			m_client_count--;
			if (i != m_client_count)
			{
				mh_client_list[i] = mh_client_list[m_client_count];
				wcscpy(m_client_ip[i], m_client_ip[m_client_count]);
			}

		}
	}
}

void CTestServerDlg::ReceiveData(SOCKET param_h_socket, char * param_p_buffer, int param_size)
{
	int current_size, total_size = 0, retry_count = 0;
	while (total_size < param_size)
	{
		current_size = recv(param_h_socket, param_p_buffer + total_size, param_size - total_size, 0);

		if (current_size == SOCKET_ERROR)
		{
			retry_count++;
			Sleep(50); // 0.05초
			if (retry_count > 5) break;
		}
		else
		{
			retry_count = 0;
			total_size = total_size + current_size;
		}
	}
}

void CTestServerDlg::SendFrameData(SOCKET param_h_socket, unsigned char param_id, const void* param_p_data, int param_size)
{
	char* p_send_data = new char[param_size + 4];

	*p_send_data = 27;
	*(unsigned short int*)(p_send_data + 1) = param_size;
	*(p_send_data + 3) = param_id;

	memcpy(p_send_data + 4, param_p_data, param_size);

	send(param_h_socket, p_send_data, param_size + 4, 0);

	delete[] p_send_data;
}

LRESULT CTestServerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (27001 == message)
	{
		AcceptProcess((SOCKET)wParam);
	}
	else if (27002 == message) 
    // FD_READ, FD_CLOSE 두 가지 이벤트 중 누가 발생 했는지 확인.
	{
		// SOCKET 자료형 : unsigned int 형(32비트 부호 없는 정수형)
		SOCKET h_socket = (SOCKET)wParam;
		// mh_listen_socket 핸들값과 동일

		if (WSAGETSELECTEVENT(lParam) == FD_READ)
		{
			WSAAsyncSelect(h_socket, m_hWnd, 27002, FD_CLOSE);

			char key, network_message_id;
			recv(h_socket, &key, 1, 0);
			if (key == 27)
			{
				int current_size, total_size = 0, retry_count = 0;
				unsigned short int body_size;
				recv(h_socket, (char *)&body_size, sizeof(body_size), 0);
				recv(h_socket, &network_message_id, 1, 0);

				if (body_size > 0)
				{
					char * p_body_data = new char[body_size];

					ReceiveData(h_socket, p_body_data, body_size);

					if (network_message_id == 1)
					{
						CString str;

						for (int i = 0; i < m_client_count; i++)
						{
							if (h_socket == mh_client_list[i])
							{
								/* 사용자가 "abc" 문자열을 보낼 경우
								   사용자가 누구인지 m_clinet_ip 배열에서
								   사용자와 동일한 이름을 가진 사용자를 찾는다. */
								str.Format(L"%s : %s", m_client_ip[i], p_body_data);
								break;
							}
						}
						// "192.168.0.1" : "abc" 이런식으로 바뀌어서 화면상에 출력됨.
						AddEventString(str);

						for (int i = 0; i < m_client_count; i++)
						{
							/* 유니코드는 문자 하나당 2bytes 씩 사용하기 때문에
							   GetLength 함수 호출한 후 널문자를 포함하기 위해 1을 더한 후에
							   곱하기 2를 해야한다. */
							SendFrameData(mh_client_list[i], 1, (const wchar_t *)str, (str.GetLength() + 1) * 2);
						}
					}

					delete[] p_body_data;
				}

				WSAAsyncSelect(h_socket, m_hWnd, 27002, FD_CLOSE | FD_READ);
			}
			else
			{
				ClientCloseProcess(h_socket, 1);
			}
		}
		else // FD_CLOSE 이벤트가 발생할 경우
		{
			ClientCloseProcess(h_socket, 0);
			AddEventString(L"클라이언트가 접속을 해제했습니다.");
		}
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}
