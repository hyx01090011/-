#define _CRT_SECURE_NO_WARNINGS 1
#include <Windows.h>
#include <tchar.h>
#include <fstream>
#include"UIlib.h"
using namespace DuiLib;
#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_ud.lib")
#   else
#       pragma comment(lib, "DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_u.lib")
#   else
#       pragma comment(lib, "DuiLib.lib")
#   endif
#endif
class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const 
	{
		return _T("DUIMainFrame");
	}
	virtual CDuiString GetSkinFile()
	{
		return _T(".\\..\\duilib.xml"); 
	}
	virtual CDuiString GetSkinFolder() 
	{ 
		return _T(""); 
	}
	virtual void Notify(TNotifyUI& msg)
	{
		CDuiString strControlName = msg.pSender->GetName();
		if (msg.sType == _T("click"))
		{
			if (strControlName == _T("closebtn"))
			{
				UINT i;
				i = MessageBox(NULL, _T("确认要退出程序吗?"), _T("提示！"), MB_YESNO | MB_ICONQUESTION);
				if (i == IDNO)
				{
					return;
				}
				else
					Close();
			}
			else if (strControlName == _T("minbtn"))
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
			else if (strControlName == _T("maxbtn"))
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
			else if (strControlName == _T("restbtn"))
				SendMessage(WM_SYSCOMMAND, SC_RESTORE);
			else if (strControlName == _T("loadbtn"))
				LoadFile();
			else if (strControlName == _T("cutbtn"))
				CutVideo();
			else if (strControlName == _T("outassbtn"))
				GenerateAssfile();
			else if (strControlName == _T("srtbtn"))
				LoadAssfile();
			else if (strControlName == _T("outgifbtn"))
			{	
				//根据方式选择：图片|视频
				CComboBoxUI* pCombBox = (CComboBoxUI*)m_PaintManager.FindControl(_T("typeCombo"));
				if(pCombBox->GetCurSel()==0)
					GenerateGifWithPic();
				else
					GenerateGifWithVedio();
			}
		}
		else if (msg.sType == _T("windowinit"))
		{
			m_pEditStart = (CEditUI*)m_PaintManager.FindControl(_T("startedit"));
			m_pEditStart->SetMaxChar(8);
			m_pEditFinish = (CEditUI*)m_PaintManager.FindControl(_T("finishedit"));
			m_pEditFinish->SetMaxChar(8);

			m_pRichEditUI = (CRichEditUI*)m_PaintManager.FindControl(_T("changetxtedit"));
			m_pListUI = (CListUI*)m_PaintManager.FindControl(_T("wordlist"));
		}
// 		else if (msg.sType==_T("itemselect"))
// 		{
// 			if (strControlName==_T("wordlist"))
// 			{
// 				m_iSelect = m_pListUI->GetCurSel();
// 			}
// 		}
	}
	void LoadFile()
	{
		OPENFILENAME ofn;
		TCHAR FileName[MAX_PATH];
		memset(&ofn, 0, sizeof(OPENFILENAME));
		memset(FileName, 0, sizeof(char)*MAX_PATH);
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.lpstrFilter = _T("");
		ofn.lpstrFile = FileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;
		if (GetOpenFileName(&ofn))
		{
			CEditUI *pPathEdit = (CEditUI*)m_PaintManager.FindControl(_T("pathedit"));
			pPathEdit->SetText(FileName);
		}
	}
	void CutVideo()
	{
		CDuiString strStartTime = m_pEditStart->GetText();
		if (!IsValidTime(strStartTime))
		{
			MessageBox(m_hWnd, _T("起始时间有误"),_T("GIFF"),IDOK);
			return;
		}
		CDuiString strFinishTime = m_pEditFinish->GetText();
		if (!IsValidTime(strFinishTime))
		{
			MessageBox(m_hWnd, _T("起始时间有误"), _T("GIFF"), IDOK);
			return;
		}

		//获取视频的路径
		CEditUI *pEditpath = (CEditUI *)m_PaintManager.FindControl(_T("pathedit"));
		CDuiString strvideoPath = pEditpath->GetText();
		if (strvideoPath.IsEmpty())
		{
			MessageBox(m_hWnd,_T("视频文件不存在！"),_T("提示"),IDOK);
			return;
		}

		//构造截取视频的命令
		CDuiString strCMD(_T("/c "));
		strCMD += CPaintManagerUI::GetInstancePath();
		strCMD += _T("ffmpeg\\ffmpeg -ss");
		strCMD += strStartTime;
		strCMD += _T(" -to ");
		strCMD += strFinishTime;
		strCMD += _T(" -i ");
		strCMD += strvideoPath;
		strCMD += _T(" -vcodec copy -acodec copy output.mp4 ");
	}
	//用cmd发命令
	
	void GenerateGifWithPic() 
	{
		//构造命令
		CDuiString strPath = CPaintManagerUI::GetInstancePath();
		CDuiString strCMD;
		strCMD += _T("/c ");
		strCMD += strPath;
		strCMD += _T("ffmpeg\\ffmpeg -r 3 -i ");
		strCMD += strPath;
		strCMD+=_T("ffmpeg\\Pictrue\\%d.jpg .\\debug\\ffmpeg\\output.gif");
		SendCMD(strCMD);
	}
	void GenerateGifWithVedio()
	{
		//ffmpeg -r 50 -i 1.flv 1.gif
		CDuiString strPath = CPaintManagerUI::GetInstancePath();
		CDuiString strCMD;
		strCMD += _T("/c ");
		strCMD += strPath;
		strCMD += (_T("ffmpeg\\ffmpeg -r 50 -i "));
		strCMD += strPath;
		strCMD += _T("ffmpeg\\output.mp4 .\\debug\\ffmpeg\\vedio.gif");
		SendCMD(strCMD);
	}
	//00:00:15
	bool IsValidTime(const CDuiString& strTime)
	{
		if (8!=strTime.GetLength())
			return false;
		for (int i=0;i<8;i++)
		{
			if (':' == strTime[i])
				continue;
			else if (isdigit(strTime[i]))
				continue;
			else
				return false;
		}
		return true;
	}
	void SendCMD(const CDuiString& strCMD)
	{
		CDuiString ststrFFmpegPathrPath = CPaintManagerUI::GetInstancePath();
		//1. 初始化结构体
		SHELLEXECUTEINFO strSEInfo;
		memset(&strSEInfo, 0, sizeof(SHELLEXECUTEINFO));
		strSEInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		strSEInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		strSEInfo.lpFile = _T("C:/Windows/System32/cmd.exe");

// 		CDuiString strFFMpegCmd;
// 		strFFMpegCmd += _T("/c");
// 		strFFMpegCmd += strFFmpegPath;
// 		strFFMpegCmd += strCMD;
		strSEInfo.lpParameters = strCMD;
		strSEInfo.nShow = SW_HIDE; //隐藏cmd窗口

		//2. 发送cmd命令
		ShellExecuteEx(&strSEInfo);
		WaitForSingleObject(strSEInfo.hProcess, INFINITE);
		MessageBox(m_hWnd, _T("当前命令响应完成！"), _T("生成确认"), IDOK);
	}
	void GenerateAssfile()
	{

	}
	void LoadAssfile()
	{
		CDuiString strPath = CPaintManagerUI::GetInstancePath();
		strPath += _T("ffmpeg\\word.srt");
		ifstream fIn(strPath.GetData());

		char wordInfo[256];
		CListUI* pListUI = (CListUI*)m_PaintManager.FindControl(_T("wordlist"));
		pListUI->RemoveAll();
		while (!fIn.eof())
		{
			CListTextElementUI * pListItem = new CListTextElementUI;
			pListUI->Add(pListItem);
			//时间
			fIn.getline(wordInfo, 256);
			wstring strTime = ANSIToUnicode(wordInfo);
			pListItem->SetText(0, strTime.c_str());//需转unicode

			//文本信息
			fIn.getline(wordInfo, 256);
			wstring strContent = ANSIToUnicode(wordInfo);
			pListItem->SetText(0, strContent.c_str());//需转unicode

		}
		fIn.close();
	}
// 	void InitWindow()
// 	{
// 		CDuiString str;
// 		CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("wordlist")));
// 
// 		// 添加List列表内容，必须先Add(pListElement)，再SetText
// 		for (int i = 0; i < 100; i++)
// 		{
// 			CListTextElementUI* pListElement = new CListTextElementUI;
// 			pListElement->SetTag(i);
// 			pList->Add(pListElement);
// 
// 			str.Format(_T("%d"), i);
// 			pListElement->SetText(0, str);
// 			pListElement->SetText(1, _T("loadbtn"));
// 		}
// 	}

	wstring ANSIToUnicode(const string& str)
	{
		int len = 0;
		len = str.length();
		int unicodeLen = ::MultiByteToWideChar(CP_ACP,
			0,
			str.c_str(),
			-1,
			NULL,
			0);

		wchar_t * pUnicode;
		pUnicode = new wchar_t[unicodeLen + 1];

		memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
		::MultiByteToWideChar(CP_ACP,
			0,
			str.c_str(),
			-1,
			(LPWSTR)pUnicode,
			unicodeLen);

		wstring rt(pUnicode);
		delete pUnicode;
		return rt;
	}

private:
	CEditUI * m_pEditStart;
	CEditUI * m_pEditFinish;
	CRichEditUI * m_pRichEditUI;
	CListUI * m_pListUI;
};

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();
	duiFrame.ShowModal();//模态对话框
	return 0;
}