 
// SingleCamDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SingleCamApp.h"
#include "SingleCamDlg.h"
#include "afxdialogex.h"
#include "HCNetSDK.h"
#include "plaympeg4.h"
#include <stdlib.h> 
#include <stdio.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSingleCamDlg 对话框
LONG nPort1 = -1;
HWND hPlayWnd1 = NULL;//播放句柄
LONG lUserID1 = -1;
LONG lRealHandle1 = -1;

LONG nPort2 = -1;
HWND hPlayWnd2 = NULL;//播放句柄
LONG lUserID2 = -1;
LONG lRealHandle2 = -1;

CString IPToStr(DWORD dwIP);
void CALLBACK DecCBFun(long nPort1,char * pBuf,long nSize,FRAME_INFO * pFrameInfo, long nReserved1,long nReserved2);
void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,void* dwUser);

CSingleCamDlg::CSingleCamDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSingleCamDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSingleCamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAM_IP1, m_ctrlDeviceIP1);//界面网址控件与类变量绑定
	DDX_Control(pDX, IDC_CAM_IP2, m_ctrlDeviceIP2);//界面网址控件与类变量绑定
}

BEGIN_MESSAGE_MAP(CSingleCamDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_Login, &CSingleCamDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_PlayCam, &CSingleCamDlg::OnBnClickedBtnPlaycam)
	ON_BN_CLICKED(IDC_BTN_StopCam, &CSingleCamDlg::OnBnClickedBtnStopcam)
	ON_BN_CLICKED(IDC_BTN_SavePicture, &CSingleCamDlg::OnBnClickedBtnSavePicture)
	ON_BN_CLICKED(IDC_BTN_StopSave, &CSingleCamDlg::OnBnClickedBtnStopsave)
	ON_BN_CLICKED(IDC_BTN_SavePicture2, &CSingleCamDlg::OnBnClickedBtnSavepicture2)
	//ON_STN_CLICKED(IDC_ViewWindow, &CSingleCamDlg::OnStnClickedViewwindow)
	//ON_NOTIFY(IPN_FIELDCHANGED, IDC_CAM_IP, &CSingleCamDlg::OnIpnFieldchangedCamIp)
	//ON_STN_CLICKED(IDC_ViewWindow2, &CSingleCamDlg::OnStnClickedViewwindow2)
	ON_BN_CLICKED(IDC_BTN_SaveRealData, &CSingleCamDlg::OnBnClickedBtnSaveRealData)
	//ON_NOTIFY(IPN_FIELDCHANGED, IDC_CAM_IP2, &CSingleCamDlg::OnIpnFieldchangedCamIp2)
END_MESSAGE_MAP()


BOOL CSingleCamDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//此处需要对SDK进行init初始化
	NET_DVR_Init();

	NET_DVR_SetLogToFile(3, "C:\\SdkLog\\", TRUE);

	isLogin = FALSE;
	isPlaying = FALSE;

	hPlayWnd1 = GetDlgItem(IDC_ViewWindow)->m_hWnd;  //在程序初始化的时候就获取播放窗口的句柄
	hPlayWnd2 = GetDlgItem(IDC_ViewWindow2)->m_hWnd;
	m_ctrlDeviceIP1.SetAddress(192, 168, 3, 21);
	m_ctrlDeviceIP2.SetAddress(192, 168, 3, 22);
	GetDlgItem(IDC_BTN_PlayCam)->EnableWindow(FALSE); //初始状态,播放\停止和抓图按钮都是失能的 
	GetDlgItem(IDC_BTN_StopCam)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BTN_SavePicture)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SavePicture2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SaveRealData)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_StopSave)->EnableWindow(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CSingleCamDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CSingleCamDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////全局函数的定义///////////////////////
CString IPToStr(DWORD dwIP)
{
	CString strIP = _T("");
	WORD add1,add2,add3,add4;

	add1=(WORD)(dwIP&255);
	add2=(WORD)((dwIP>>8)&255);
	add3=(WORD)((dwIP>>16)&255);
	add4=(WORD)((dwIP>>24)&255);
	strIP.Format("%d.%d.%d.%d",add4,add3,add2,add1);
	return strIP;
}

void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* dwUser)
{
	if (lRealHandle == lRealHandle1)
	{

		switch (dwDataType)
		{
		case NET_DVR_SYSHEAD: //系统头

			if (!PlayM4_GetPort(&nPort1))  //获取播放库未使用的通道号
			{
				break;
			}
			//m_iPort = lPort; //第一次回调的是系统头，将获取的播放库port号赋值给全局port，下次回调数据时即使用此port号播放
			if (dwBufSize > 0)
			{
				if (!PlayM4_SetStreamOpenMode(nPort1, STREAME_REALTIME))  //设置实时流播放模式
				{
					break;
				}

				if (!PlayM4_OpenStream(nPort1, pBuffer, dwBufSize, 1024 * 1024)) //打开流接口
				{
					break;
				}
				//设置解码回调函数 只解码不显示
				//if (!PlayM4_SetDecCallBack(lPort,DecCBFun))
				//{
				//	//dRet=PlayM4_GetLastError(nPort1);
				//	break;
				//}

				//设置解码回调函数 解码且显示在窗口句柄中
				if (!PlayM4_SetDecCallBackEx(nPort1, DecCBFun, NULL, NULL))
				{
					break;
				}
				if (!PlayM4_SetDisplayBuf(nPort1, 4))//设置缓冲区大小,2-5
				{
					break;
				}
				if (!PlayM4_Play(nPort1, hPlayWnd1)) //播放开始
				{
					break;
				}
			}
		case NET_DVR_STREAMDATA:   //码流数据
			if (dwBufSize > 0 && nPort1 != -1)
				if (!PlayM4_InputData(nPort1, pBuffer, dwBufSize))
					break;
		}
	}
	else
	{
		switch (dwDataType)
		{
		case NET_DVR_SYSHEAD: //系统头

			if (!PlayM4_GetPort(&nPort2))  //获取播放库未使用的通道号
			{
				break;
			}
			//m_iPort = lPort; //第一次回调的是系统头，将获取的播放库port号赋值给全局port，下次回调数据时即使用此port号播放
			if (dwBufSize > 0)
			{
				if (!PlayM4_SetStreamOpenMode(nPort2, STREAME_REALTIME))  //设置实时流播放模式
				{
					break;
				}

				if (!PlayM4_OpenStream(nPort2, pBuffer, dwBufSize, 1024 * 1024)) //打开流接口
				{
					break;
				}
				//设置解码回调函数 只解码不显示
				//if (!PlayM4_SetDecCallBack(lPort,DecCBFun))
				//{
				//	//dRet=PlayM4_GetLastError(nPort1);
				//	break;
				//}

				//设置解码回调函数 解码且显示在窗口句柄中
				if (!PlayM4_SetDecCallBackEx(nPort2, DecCBFun, NULL, NULL))
				{
					break;
				}
				if (!PlayM4_SetDisplayBuf(nPort2, 4))//设置缓冲区大小,2-5
				{
					break;
				}
				if (!PlayM4_Play(nPort2, hPlayWnd2)) //播放开始
				{
					break;
				}
			}
		case NET_DVR_STREAMDATA:   //码流数据
			if (dwBufSize > 0 && nPort2 != -1)
				if (!PlayM4_InputData(nPort2, pBuffer, dwBufSize))
					break;
		}
	}
}

void CALLBACK DecCBFun(long nPort1,char * pBuf,long nSize,FRAME_INFO * pFrameInfo, long nReserved1,long nReserved2)
{
 	
}

////////////////////////////////////////////////////////////////




void CSingleCamDlg::OnBnClickedBtnLogin()//登录
{
	if (isLogin) //如果已经登录,就需要执行注销
	{
		NET_DVR_Logout(lUserID1);
		NET_DVR_Logout(lUserID2);
		isLogin = FALSE;
		GetDlgItem(IDC_BTN_Login)->SetWindowTextA("登录"); 
		GetDlgItem(IDC_BTN_PlayCam)->EnableWindow(FALSE);  //播放按钮 失效
		GetDlgItem(IDC_BTN_SavePicture)->EnableWindow(FALSE);  // 抓图按钮失效
		GetDlgItem(IDC_BTN_SavePicture2)->EnableWindow(FALSE);  // 抓图按钮失效
		GetDlgItem(IDC_BTN_SaveRealData)->EnableWindow(FALSE);  // 抓图按钮失效
	} 
	else
	{
		char DeviceIP[16] = {0};
		char DeviceIP2[16] = {0};
		DWORD dwDeviceIP = 0;
		m_ctrlDeviceIP1.GetAddress(dwDeviceIP);
		CString csTemp = IPToStr(dwDeviceIP);
		sprintf_s(DeviceIP, "%s", csTemp.GetBuffer(0));

		m_ctrlDeviceIP2.GetAddress(dwDeviceIP);
		csTemp = IPToStr(dwDeviceIP);
		sprintf_s(DeviceIP2, "%s", csTemp.GetBuffer(0));

		NET_DVR_DEVICEINFO_V30 struDeviceInfo, struDeviceInfo2;
		lUserID1 = NET_DVR_Login_V30(DeviceIP, 8000, "admin", "12345", &struDeviceInfo);
		lUserID2 = NET_DVR_Login_V30(DeviceIP2, 8000, "admin", "12345", &struDeviceInfo2);
		if (lUserID1 < 0 || lUserID2 < 0) //登录失败时,其他几个按钮都需要disable
		{
			AfxMessageBox("登录失败!");
			isLogin = FALSE;
		} 
		else
		{
			isLogin = TRUE;
			GetDlgItem(IDC_BTN_Login)->SetWindowTextA("注销");  //登录成功, 变成注销
			GetDlgItem(IDC_BTN_PlayCam)->EnableWindow(TRUE);  //播放按钮 有效
		}
	}
}

char* itostr(char *str, int i) //将i转化位字符串存入str
{
	sprintf(str, "%d", i);
	return str;
}


void CSingleCamDlg::OnBnClickedBtnPlaycam() //播放
{

		NET_DVR_PREVIEWINFO struPlayInfo1 = {0};
		struPlayInfo1.hPlayWnd = hPlayWnd1;         //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
		struPlayInfo1.lChannel     = 1;       //预览通道号
		struPlayInfo1.dwStreamType = 0;       //0-主码流，1-子码流，2-码流3，3-码流4，以此类推
		struPlayInfo1.dwLinkMode   = 0;       //0- TCP方式，1- UDP方式，2- 多播方式，3- RTP方式，4-RTP/RTSP，5-RSTP/HTTP

		NET_DVR_PREVIEWINFO struPlayInfo2 = { 0 };
		struPlayInfo2.hPlayWnd = hPlayWnd2;         //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
		struPlayInfo2.lChannel = 1;       //预览通道号
		struPlayInfo2.dwStreamType = 0;       //0-主码流，1-子码流，2-码流3，3-码流4，以此类推
		struPlayInfo2.dwLinkMode = 0;       //0- TCP方式，1- UDP方式，2- 多播方式，3- RTP方式，4-RTP/RTSP，5-RSTP/HTTP


		lRealHandle1 = NET_DVR_RealPlay_V40(lUserID1, &struPlayInfo1, g_RealDataCallBack_V30, NULL);
		lRealHandle2 = NET_DVR_RealPlay_V40(lUserID2, &struPlayInfo2, g_RealDataCallBack_V30, NULL);

		if (lRealHandle1 < 0 || lRealHandle2 < 0)
		{
			AfxMessageBox("播放失败!");
			isPlaying = FALSE;//播放标志位设置为假
		} 

		else
		{
			GetDlgItem(IDC_BTN_Login)->EnableWindow(FALSE);  //登录按钮失效
			GetDlgItem(IDC_BTN_PlayCam)->EnableWindow(FALSE);  //播放按钮失效
			GetDlgItem(IDC_BTN_StopCam)->EnableWindow(TRUE);  //停止按钮 有效
			GetDlgItem(IDC_BTN_SavePicture)->EnableWindow(TRUE); // 抓图按钮 有效
			GetDlgItem(IDC_BTN_SavePicture2)->EnableWindow(TRUE); // 抓图2按钮 有效
			GetDlgItem(IDC_BTN_SaveRealData)->EnableWindow(TRUE); // 视频保存按钮 有效
			isPlaying = TRUE;
		}
		NET_DVR_GetLastError();
} 


void a(int b, int SIZE, LONG lRealHandle) // LONG lRealHandle,char *sPicFileName
{
	if (b > SIZE) return;

	++b;
	char* filename = new char(20);
	sprintf(filename, "E:\\peng\\B%d.jpg", b);
	// if (k = m - 1)
	NET_DVR_CapturePicture(lRealHandle, filename);
	a(b, SIZE, lRealHandle);

}


long long k = 0, m;
BOOL saveFlag = FALSE;
void CSingleCamDlg::OnBnClickedBtnSavePicture()
{
	// a(0, 100, lRealHandle1);
	BOOL isSave;

	char* filename = new char(20);
	sprintf(filename, "E:\\peng\\cam1.jpg");
	isSave = NET_DVR_CapturePicture(lRealHandle1, filename);
	if (isSave == FALSE)
		AfxMessageBox("抓取图片失败!");
}

void CSingleCamDlg::OnBnClickedBtnSavepicture2()
{
	BOOL isSave;

	char* filename = new char(20);
	sprintf(filename, "E:\\peng\\cam2.jpg");
	isSave = NET_DVR_CapturePicture(lRealHandle2, filename);
	if (isSave == FALSE)
		AfxMessageBox("抓取图片失败!");
}

void CSingleCamDlg::OnBnClickedBtnStopsave()
{
	saveFlag = FALSE;
	GetDlgItem(IDC_BTN_SavePicture)->EnableWindow(TRUE); // 停止抓图按钮 有效
	BOOL isSave1, isSave2;
	char* filename1 = new char(20);
	char* filename2 = new char(20);
	sprintf(filename1, "E:\\peng\\1test.wmv");
	isSave1 = NET_DVR_SaveRealData(lRealHandle1, filename1);
	sprintf(filename2, "E:\\peng\\2test.wmv");
	isSave2 = NET_DVR_SaveRealData(lRealHandle2, filename2);
	if (isSave1 == FALSE || isSave2 == FALSE)
	{
		AfxMessageBox("保存视频文件失败!");
	}
}


void CSingleCamDlg::OnBnClickedBtnStopcam()//停止
{

	NET_DVR_StopRealPlay(lRealHandle1);
	NET_DVR_StopRealPlay(lRealHandle2);

	//释放播放库资源
	PlayM4_Stop(nPort1);
	PlayM4_CloseStream(nPort1);
	PlayM4_FreePort(nPort1);
	PlayM4_Stop(nPort2);
	PlayM4_CloseStream(nPort2);
	PlayM4_FreePort(nPort2);
	GetDlgItem(IDC_BTN_Login)->EnableWindow(TRUE);  //登录按钮失效
	GetDlgItem(IDC_BTN_PlayCam)->EnableWindow(TRUE);  //播放按钮 有效
	GetDlgItem(IDC_BTN_StopCam)->EnableWindow(FALSE);  //停止按钮 失效
	GetDlgItem(IDC_BTN_SavePicture)->EnableWindow(FALSE); // 抓图按钮 失效
	GetDlgItem(IDC_BTN_SavePicture2)->EnableWindow(FALSE); // 抓图按钮 失效
	GetDlgItem(IDC_BTN_StopSave)->EnableWindow(FALSE); // 停止抓图按钮 失效
}



void CSingleCamDlg::OnBnClickedBtnSaveRealData()
{
	saveFlag = FALSE;
	GetDlgItem(IDC_BTN_StopSave)->EnableWindow(TRUE); // 停止保存按钮 有效
	BOOL isSave1, isSave2;
	char* filename1 = new char(20);
	char* filename2 = new char(20);
	sprintf(filename1, "E:\\peng\\1test.mp4");
	isSave1 = NET_DVR_SaveRealData(lRealHandle1, filename1);
	sprintf(filename2, "E:\\peng\\2test.mp4");
	isSave2 = NET_DVR_SaveRealData(lRealHandle2, filename2);

	if (isSave1 == FALSE)
	{
		AfxMessageBox("左窗所属摄像头保存视频文件失败!");
	}

	if (isSave2 == FALSE)
	{
		AfxMessageBox("右窗所属摄像头保存视频文件失败!");
	}
}
