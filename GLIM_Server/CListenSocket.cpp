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

	if (Accept(*p_Client))															 // Ŭ���̾�Ʈ ���� ��û�� ���� ����-Ŭ���̾�Ʈ�� ��������ش�
	{
		p_Client->SetListenSocket(this);
		m_ptrClientSocketList.AddTail(p_Client);									 // ����Ʈ�� Ŭ���̾�Ʈ ���� ����
		m_clientSockets.Add(p_Client);

		CGLIMServerDlg* pMain = (CGLIMServerDlg*)AfxGetMainWnd();					 // CGLIMServerDlg�� �ڵ��� ������
		str.Format(_T("Client (%d)"), (int)m_ptrClientSocketList.Find(p_Client));	 // Ŭ���̾�Ʈ ��ȣ(POSITION(�ּ�) ��)
		pMain->clientList->AddString(str);											 // Ŭ���̾�Ʈ�� �����Ҷ����� ����Ʈ�� Ŭ���̾�Ʈ �̸� �߰�
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
			// Ŭ���̾�Ʈ ���������� ����
			pClient->ShutDown();
			pClient->Close();
		}

		CGLIMServerDlg* pMain = (CGLIMServerDlg*)AfxGetMainWnd();
		CString str1, str2;
		UINT indx = 0, posNum;
		pMain->clientList->SetCurSel(0);
		// ���� ����Ǵ� Ŭ���̾�Ʈ ã��
		while (indx < pMain->clientList->GetCount()) 
		{
			posNum = (int)m_ptrClientSocketList.Find(pClient);
			pMain->clientList->GetText(indx, str1);
			str2.Format(_T("%d"), posNum);
			// ���ϸ���Ʈ, Ŭ���̾�Ʈ����Ʈ�� ���ؼ� ���� Ŭ���̾�Ʈ ��ȣ(POSITION ��) ������ ����Ʈ���� ����
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