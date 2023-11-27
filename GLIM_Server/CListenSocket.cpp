#include "pch.h"
#include "GLIM_ServerDlg.h"
#include "CClientSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CListenSocket::CListenSocket()
{
}

CListenSocket::~CListenSocket()
{
}

void CListenSocket::OnAccept(int nErrorCode)
{
	CClientSocket* pClient = new CClientSocket;
	CString str;

	if (Accept(*pClient))															// if(접속 요청이 오면)
	{
		pClient->SetListenSocket(this);												//
		m_ptrClientSocketList.AddTail(pClient);										// 리스트의 뒷부분에 클라이언트 소켓을 저장

		CGLIMServerDlg* pMain = (CGLIMServerDlg*)AfxGetMainWnd();					// CGLIMServerDlg의 핸들을 가져온다
		str.Format(_T("Client (%d)")), (int)m_ptrClientSocketList.Find(pClient);	// 클라이언트 번호(POSITION(주소) 값)
		pMain->clientList->AddString(str);
	}
	else
	{
		delete pClient;
		AfxMessageBox(_T("ERROR : Faild can't accept new Client"));
	}
	CAsyncSocket::OnAccept(nErrorCode);
}

void CListenSocket::CloseClientSocket(CSocket* pClient)
{
	POSITION pos;
	pos = m_ptrClientSocketList.Find(pClient);
	
	if (pos != NULL)
	{
		if (pClient != NULL)
		{
			// 클라이언트 연결 중지후 종료
			pClient->ShutDown();
			pClient->Close();
		}
	}

	CGLIMServerDlg* pMain = (CGLIMServerDlg*)AfxGetMainWnd();
	CString str1, str2;
	UINT indx = 0, posNum;
	pMain->clientList->SetCurSel(0);
	// 접속이 종료된 클라이언트 찾기
	while (indx < pMain->clientList->GetCount())
	{
		posNum = (int)m_ptrClientSocketList.Find(pClient);
		pMain->clientList->GetText(indx, str1);
		str2.Format(_T("%d"), posNum);

		if (str1.Find(str2) != -1)
		{
			AfxMessageBox(str1 + str2);
			pMain->clientList->DeleteString(indx);
			break;
		}
		indx++;
	}
	m_ptrClientSocketList.RemoveAt(pos);
	delete pClient;
}