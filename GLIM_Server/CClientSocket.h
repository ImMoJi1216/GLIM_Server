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
	
public:
	CClientSocket();
	virtual ~CClientSocket();

	void Receive_Streaming();
	void SplitCString(const CString& input, CStringArray& output, TCHAR delimiter);
	void SetListenSocket(CAsyncSocket* pSocket);
	void Send_Test_results(int num);

	void OnClose(int nErrorCode);
	void OnReceive(int nErrorCode);
};

