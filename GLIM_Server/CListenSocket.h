#pragma once
#include <afxsock.h>
#include "CClientSocket.h"

class CListenSocket : public CAsyncSocket
{
public:
	CListenSocket();
	virtual ~CListenSocket();

	CPtrList m_ptrClientSocketList;
	CArray<CClientSocket*, CClientSocket*> m_clientSockets;

	void OnAccept(int nErrorCode);
	void CloseClientSocket(CSocket* pClient);
	void Send_Test_results(int num);
};