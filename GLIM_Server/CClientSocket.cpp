#include "pch.h"
#include "GLIM_ServerDlg.h"
#include "CListenSocket.h"
#include "CClientSocket.h"

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}

void CClientSocket::SetListenSocket(CAsyncSocket* pSocket)
{
	m_pListenSocket = pSocket;
}

void CClientSocket::OnClose(int nErrorCode)
{
	CSocket::OnClose(nErrorCode);

	CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
	pServerSocket->CloseClientSocket(this);
}

void CClientSocket::OnReceive(int nErrorCode)
{
	CGLIMServerDlg* pMain = (CGLIMServerDlg*)AfxGetMainWnd();
	CString strTmp = _T(""), strIPAddress = _T("");
	UINT uPortNumber = 0;
	TCHAR strBuffer[1024];
	::ZeroMemory(strBuffer, sizeof(strBuffer));
	// ::은 범위지정 연산자  strBuffer의 배열을 0으로 지정

	GetPeerName(strIPAddress, uPortNumber);
	if (Receive(strBuffer, sizeof(strBuffer)) > 0) // 전달된 데이터(문자열)가 있을 경우
	{
		TRACE(_T("Received data: %s\n"), strBuffer);
		CStringArray tokens;
		SplitCString(strBuffer, tokens, _T(','));
		if (tokens.GetSize() >= 2) 
		{
			CString Type_Token = tokens.GetAt(0);

			if (Type_Token == "Type:Message")			// 구분자 타입이 메세지일때
			{
				CString Message_Token = tokens.GetAt(1);
				strTmp.Format(_T("[%s:%d]: %s"), strIPAddress, uPortNumber, Message_Token);
				CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
			}
			else if (Type_Token == "Type:File")			// 구분자 타입이 파일일때
			{
				CString Filesize_Token = tokens.GetAt(1);
				CString Filename_Token = tokens.GetAt(2);
				strTmp.Format(_T("Received File: Size=%s, Name=%s\n"), Filesize_Token, Filename_Token);
				ULONGLONG fileSize = _ttoi64(Filesize_Token);
				char* fileBuffer = new char[fileSize];

				// 파일 데이터를 서버로부터 수신
				if (Receive(fileBuffer, (UINT)fileSize) > 0) 
				{
					// 파일을 저장할 경로 설정 (원하는 경로로 변경 필요)
					CString saveFilePath = _T("C:/Users/LMS23/Desktop/GLIM_Server_fileSave/") + Filename_Token;

					// 파일을 저장
					CFile file;
					if (file.Open(saveFilePath, CFile::modeCreate | CFile::modeWrite)) 
					{
						file.Write(fileBuffer, (UINT)fileSize);
						file.Close();
						TRACE(_T("File saved to: %s\n"), saveFilePath);
						Receive_Streaming();
						pMain->Send_img_Python();
					}
					else 
					{
						TRACE(_T("Failed to open file for writing: %s\n"), saveFilePath);
					}
				}
				// 할당한 버퍼 해제
				delete[] fileBuffer;
			}
		}
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

void CClientSocket::SplitCString(const CString& input, CStringArray& output, TCHAR delimiter)
{
	int start = 0;
	int end = 0;

	while ((end = input.Find(delimiter, start)) != -1)
	{
		CString token = input.Mid(start, end - start);
		output.Add(token);
		start = end + 1;
	}

	CString lastToken = input.Mid(start);
	output.Add(lastToken);
}

void CClientSocket::Receive_Streaming()
{
	CString filePath = _T("C:/Users/LMS23/Desktop/GLIM_Server_fileSave/Temp_Streaming_image.jpg");

	CGLIMServerDlg* pMain = (CGLIMServerDlg*)AfxGetMainWnd();

	// 이미지를 CImage로 로드
	CImage image;
	HRESULT hr = image.Load(filePath);
	if (SUCCEEDED(hr))
	{
		// 픽쳐 컨트롤의 DC를 얻어옵니다.
		CDC* pDC = pMain->m_Picture.GetDC();

		// 이미지를 픽쳐 컨트롤의 DC에 그립니다.
		image.BitBlt(pDC->m_hDC, 0, 0);

		// 픽쳐 컨트롤의 DC를 해제합니다.
		pMain->m_Picture.ReleaseDC(pDC);
	}
	else
	{
		TRACE(_T("야 너 오류남 좆됨!!!!!!!!!!!"));
	}
}