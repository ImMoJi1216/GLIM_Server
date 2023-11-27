#include "pch.h"
#include "PyConnectSocket.h"
#include "GLIM_ServerDlg.h"

void PyConnectSocket::OnClose(int nErrorCode)
{
    ShutDown();
    Close();

    CSocket::OnClose(nErrorCode);
    
    CGLIMServerDlg* pMain = (CGLIMServerDlg*)AfxGetMainWnd();
    pMain->m_List.AddString(_T("ERROR:Disconnected from Python server!"));
}

void PyConnectSocket::OnReceive(int nErrorCode)
{
    char strBuffer[1024];
    ::ZeroMemory(strBuffer, sizeof(strBuffer));

    int nReceived = Receive(strBuffer, sizeof(strBuffer));
    CString str;

    if (nReceived > 0)
    {
        // UTF-8 디코딩
        CStringA strUTF8(strBuffer, nReceived);
        int nChars = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);

        if (nChars > 0)
        {
            CString strDecoded;
            MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, strDecoded.GetBuffer(nChars), nChars);
            strDecoded.ReleaseBuffer();

            CGLIMServerDlg* pMain = (CGLIMServerDlg*)AfxGetMainWnd();
            str.Format(_T("[ PYTHON ] : %s"), strDecoded);
            pMain->m_List.AddString(str);
            pMain->m_List.SetCurSel(pMain->m_List.GetCount() - 1);
        }
        else
        {
            // 디코딩 실패
            AfxMessageBox(_T("UTF-8 디코딩에 실패했습니다."));
        }
    }
    CSocket::OnReceive(nErrorCode);
}