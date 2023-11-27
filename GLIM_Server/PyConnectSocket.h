#pragma once
#include <afxsock.h>

class PyConnectSocket : public CSocket
{
public:
	void OnClose(int nErrorCode);
	void OnReceive(int nErrorCode);
};