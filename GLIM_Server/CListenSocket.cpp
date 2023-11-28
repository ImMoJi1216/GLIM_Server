#include "pch.h"
#include "GLIM_ServerDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CListenSocket::CListenSocket()
{
	//m_ptrClientSocketList.RemoveAll();
}

CListenSocket::~CListenSocket()
{
}

void CListenSocket::OnAccept(int nErrorCode)
{
	CClientSocket* p_Client = new CClientSocket;
	CString str;

	if (Accept(*p_Client))															 // 클라이언트 접속 요청이 오면 서버-클라이언트를 연결시켜준다
	{
		p_Client->SetListenSocket(this);
		m_ptrClientSocketList.AddTail(p_Client);									 // 리스트에 클라이언트 소켓 저장
		m_clientSockets.Add(p_Client);

		CGLIMServerDlg* pMain = (CGLIMServerDlg*)AfxGetMainWnd();					 // CGLIMServerDlg의 핸들을 가져옴
		str.Format(_T("Client (%d)"), (int)m_ptrClientSocketList.Find(p_Client));	 // 클라이언트 번호(POSITION(주소) 값)
		pMain->clientList->AddString(str);											 // 클라이언트가 접속할때마다 리스트에 클라이언트 이름 추가
	}
	else 
	{
		delete p_Client;
		AfxMessageBox(_T("ERROR : Failed can't accept new Client!"));
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
			// 클라이언트 연결중지후 종료
			pClient->ShutDown();
			pClient->Close();
		}

		CGLIMServerDlg* pMain = (CGLIMServerDlg*)AfxGetMainWnd();
		CString str1, str2;
		UINT indx = 0, posNum;
		pMain->clientList->SetCurSel(0);
		// 접속 종료되는 클라이언트 찾기
		while (indx < pMain->clientList->GetCount()) 
		{
			posNum = (int)m_ptrClientSocketList.Find(pClient);
			pMain->clientList->GetText(indx, str1);
			str2.Format(_T("%d"), posNum);
			// 소켓리스트, 클라이언트리스트를 비교해서 같은 클라이언트 번호(POSITION 값) 있으면 리스트에서 삭제
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
}

void CListenSocket::Send_Test_results(int num)
{
	TRACE(_T("%d\n\n\n"), m_clientSockets.GetSize());

	for (int i = 0; i < m_clientSockets.GetSize(); ++i)
	{
		CClientSocket* pClientSocket = m_clientSockets.GetAt(i);
		CString strMessageToSend;
		strMessageToSend.Format(_T("Results,%d"), num);
		pClientSocket->Send(strMessageToSend, strMessageToSend.GetLength() * sizeof(TCHAR));
	}
}