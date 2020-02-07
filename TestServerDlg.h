
// TestServerDlg.h: 헤더 파일
//

#pragma once

#define MAX_CLIENT_COUNT   10

// CTestServerDlg 대화 상자
class CTestServerDlg : public CDialogEx
{
private:
	SOCKET mh_listen_socket; 
	/* listen_socket을 이용하여 클라이언트의 접속을
	   받아주는 기능을 만듬. */

	// 클라이언트를 10명 까지 관리함.
	SOCKET mh_client_list[MAX_CLIENT_COUNT];

	// IP가 최대 24 자리를 넘을 수 없어서 24라고 함.
	wchar_t m_client_ip[MAX_CLIENT_COUNT][24];
	int m_client_count;

// 생성입니다.
public:
	CTestServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	void AcceptProcess(SOCKET param_h_socket);
	void ClientCloseProcess(SOCKET param_h_socket, char param_force_flag);
	void ReceiveData(SOCKET param_h_socket, char* param_p_buffer, int param_size);
	void AddEventString(CString param_string);
	void SendFrameData(SOCKET param_h_socket, unsigned char param_id, const void* param_p_data, int param_size);
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	CListBox m_event_list;
};
