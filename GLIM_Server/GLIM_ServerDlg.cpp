
// GLIM_ServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "GLIM_Server.h"
#include "GLIM_ServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGLIMServerDlg 대화 상자



CGLIMServerDlg::CGLIMServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GLIM_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGLIMServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_PIC, m_Picture);
}

BEGIN_MESSAGE_MAP(CGLIMServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CGLIMServerDlg 메시지 처리기

BOOL CGLIMServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 클라이언트 접속 Dlg 초기화
	clientList = (CListBox*)GetDlgItem(IDC_CLIENT_LIST);
	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// 소켓생성 (서버 시작)
	if (m_ListenSocket.Create(25000, SOCK_STREAM))
	{
		if (!m_ListenSocket.Listen()) 
		{
			AfxMessageBox(_T("ERROR:Listen() return False"));
		}
		else
		{
			m_List.AddString(_T("[ 서버가 시작되었습니다 ]"));
		}
	}
	else 
	{
		AfxMessageBox(_T("ERROR:Failed to create server socket!"));
	}

	// 파이썬 서버 연결
	m_Socket.Create();
	if (m_Socket.Connect(_T("10.10.21.118"), 30000) == FALSE)
	{
		m_List.AddString(_T("[ ERROR : Failed to connect Python Server ]"));
	}
	else
	{
		m_List.AddString(_T("[ AI서버와 연결되었습니다 ]"));
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CGLIMServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CGLIMServerDlg::OnPaint()
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

// 내가 추가함
void CGLIMServerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	POSITION pos;

	pos = m_ListenSocket.m_ptrClientSocketList.GetHeadPosition();
	CClientSocket* pClient = NULL;

	// 생성되어있는 클라이언트 소켓이 없을때까지 체크하여 소켓닫기
	while (pos != NULL) 
	{
		pClient = (CClientSocket*)m_ListenSocket.m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL) 
		{
			pClient->ShutDown(); // 연결된 상대방 소켓에 연결이 종료됨을 알린다. 
			pClient->Close(); // 소켓을 닫는다

			delete pClient;
		}
	}
	m_ListenSocket.ShutDown();
	m_ListenSocket.Close();
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CGLIMServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGLIMServerDlg::Send_img_Python()
{
	CString filePath = _T("C:/Users/LMS23/Desktop/GLIM_Server_fileSave/Temp_Streaming_image.jpg");	// 파일 경로
	CString fileName = _T("Temp_Streaming_image.jpg");					   				     		// 파일 이름

	CFile file;
	if (file.Open(filePath, CFile::modeRead))		// 읽기전용
	{
		ULONGLONG fileSize = file.GetLength();		// 파일크기
		char* fileData = new char[fileSize];
		file.Read(fileData, (UINT)fileSize);
		file.Close();

		CString strMessageToSend;
		strMessageToSend.Format(_T("Type:File,%I64u,%s"), fileSize, fileName);

		// CString을 UTF-8로 인코딩된 문자열로 변환
		std::string utf8Message = CT2CA(strMessageToSend, CP_UTF8);
		m_Socket.Send(utf8Message.c_str(), utf8Message.size());

		Sleep(2000);																			// 1초동안 딜레이 (서버에서의 버퍼 준비시간)
		m_Socket.Send((LPVOID)(LPCTSTR)fileData, (UINT)fileSize);								// 서버로 파일 전송

		delete[] fileData;
	}
}

void CGLIMServerDlg::Send_Test_Result(int num)
{
	POSITION pos;
	pos = m_ListenSocket.m_ptrClientSocketList.GetHeadPosition();
	CClientSocket* pClient = NULL;

	// 생성되어있는 클라이언트 소켓이 없을때까지 체크하여 소켓닫기
	while (pos != NULL)
	{
		pClient = (CClientSocket*)m_ListenSocket.m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL)
		{
			CString strMessageToSend;
			strMessageToSend.Format(_T("Results,%d"), num);
			pClient->Send(strMessageToSend, strMessageToSend.GetLength() * sizeof(TCHAR));
		}
	}
}