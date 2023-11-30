
// GLIM_ServerDlg.h: 헤더 파일
//
#include "CListenSocket.h"
#include "CClientSocket.h"
#include "PyConnectSocket.h"
#include <afxsock.h>
#include <fstream>
#include <string>
#include <vector>

#pragma once

// CGLIMServerDlg 대화 상자
class CGLIMServerDlg : public CDialogEx
{
// 생성입니다.
public:
	CGLIMServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	PyConnectSocket m_Socket;
	CListenSocket m_ListenSocket;
	CListBox* clientList;

	void OnDestroy();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GLIM_SERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void Send_Test_Result(int num);			// 결과값 전송
	void Send_img_Python();					// 파이썬 서버로 이미지 전송

	CListBox m_List;
	CStatic m_Picture;
};
