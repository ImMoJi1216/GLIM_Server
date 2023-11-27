#pragma once
#include <afxsock.h>
#include <uchar.h>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
class CClientSocket : public CSocket
{
private:
	CAsyncSocket* m_pListenSocket;
	CCriticalSection m_CriticalSection;
	
public:
	CClientSocket();
	virtual ~CClientSocket();

	void Receive_Streaming();

	void SplitCString(const CString& input, CStringArray& output, TCHAR delimiter);

	void SetListenSocket(CAsyncSocket* pSocket);

	void OnClose(int nErrorCode);
	void OnReceive(int nErrorCode);
	void StartReceiveThread();
};

