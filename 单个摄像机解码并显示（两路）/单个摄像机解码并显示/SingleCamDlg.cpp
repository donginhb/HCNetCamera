 
// SingleCamDlg.cpp : ʵ���ļ�
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


// CSingleCamDlg �Ի���
LONG nPort1 = -1;
HWND hPlayWnd1 = NULL;//���ž��
LONG lUserID1 = -1;
LONG lRealHandle1 = -1;

LONG nPort2 = -1;
HWND hPlayWnd2 = NULL;//���ž��
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
	DDX_Control(pDX, IDC_CAM_IP1, m_ctrlDeviceIP1);//������ַ�ؼ����������
	DDX_Control(pDX, IDC_CAM_IP2, m_ctrlDeviceIP2);//������ַ�ؼ����������
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//�˴���Ҫ��SDK����init��ʼ��
	NET_DVR_Init();

	NET_DVR_SetLogToFile(3, "C:\\SdkLog\\", TRUE);

	isLogin = FALSE;
	isPlaying = FALSE;

	hPlayWnd1 = GetDlgItem(IDC_ViewWindow)->m_hWnd;  //�ڳ����ʼ����ʱ��ͻ�ȡ���Ŵ��ڵľ��
	hPlayWnd2 = GetDlgItem(IDC_ViewWindow2)->m_hWnd;
	m_ctrlDeviceIP1.SetAddress(192, 168, 3, 21);
	m_ctrlDeviceIP2.SetAddress(192, 168, 3, 22);
	GetDlgItem(IDC_BTN_PlayCam)->EnableWindow(FALSE); //��ʼ״̬,����\ֹͣ��ץͼ��ť����ʧ�ܵ� 
	GetDlgItem(IDC_BTN_StopCam)->EnableWindow(FALSE); 
	GetDlgItem(IDC_BTN_SavePicture)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SavePicture2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SaveRealData)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_StopSave)->EnableWindow(FALSE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


void CSingleCamDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
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

/////////////////////////////ȫ�ֺ����Ķ���///////////////////////
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
		case NET_DVR_SYSHEAD: //ϵͳͷ

			if (!PlayM4_GetPort(&nPort1))  //��ȡ���ſ�δʹ�õ�ͨ����
			{
				break;
			}
			//m_iPort = lPort; //��һ�λص�����ϵͳͷ������ȡ�Ĳ��ſ�port�Ÿ�ֵ��ȫ��port���´λص�����ʱ��ʹ�ô�port�Ų���
			if (dwBufSize > 0)
			{
				if (!PlayM4_SetStreamOpenMode(nPort1, STREAME_REALTIME))  //����ʵʱ������ģʽ
				{
					break;
				}

				if (!PlayM4_OpenStream(nPort1, pBuffer, dwBufSize, 1024 * 1024)) //�����ӿ�
				{
					break;
				}
				//���ý���ص����� ֻ���벻��ʾ
				//if (!PlayM4_SetDecCallBack(lPort,DecCBFun))
				//{
				//	//dRet=PlayM4_GetLastError(nPort1);
				//	break;
				//}

				//���ý���ص����� ��������ʾ�ڴ��ھ����
				if (!PlayM4_SetDecCallBackEx(nPort1, DecCBFun, NULL, NULL))
				{
					break;
				}
				if (!PlayM4_SetDisplayBuf(nPort1, 4))//���û�������С,2-5
				{
					break;
				}
				if (!PlayM4_Play(nPort1, hPlayWnd1)) //���ſ�ʼ
				{
					break;
				}
			}
		case NET_DVR_STREAMDATA:   //��������
			if (dwBufSize > 0 && nPort1 != -1)
				if (!PlayM4_InputData(nPort1, pBuffer, dwBufSize))
					break;
		}
	}
	else
	{
		switch (dwDataType)
		{
		case NET_DVR_SYSHEAD: //ϵͳͷ

			if (!PlayM4_GetPort(&nPort2))  //��ȡ���ſ�δʹ�õ�ͨ����
			{
				break;
			}
			//m_iPort = lPort; //��һ�λص�����ϵͳͷ������ȡ�Ĳ��ſ�port�Ÿ�ֵ��ȫ��port���´λص�����ʱ��ʹ�ô�port�Ų���
			if (dwBufSize > 0)
			{
				if (!PlayM4_SetStreamOpenMode(nPort2, STREAME_REALTIME))  //����ʵʱ������ģʽ
				{
					break;
				}

				if (!PlayM4_OpenStream(nPort2, pBuffer, dwBufSize, 1024 * 1024)) //�����ӿ�
				{
					break;
				}
				//���ý���ص����� ֻ���벻��ʾ
				//if (!PlayM4_SetDecCallBack(lPort,DecCBFun))
				//{
				//	//dRet=PlayM4_GetLastError(nPort1);
				//	break;
				//}

				//���ý���ص����� ��������ʾ�ڴ��ھ����
				if (!PlayM4_SetDecCallBackEx(nPort2, DecCBFun, NULL, NULL))
				{
					break;
				}
				if (!PlayM4_SetDisplayBuf(nPort2, 4))//���û�������С,2-5
				{
					break;
				}
				if (!PlayM4_Play(nPort2, hPlayWnd2)) //���ſ�ʼ
				{
					break;
				}
			}
		case NET_DVR_STREAMDATA:   //��������
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




void CSingleCamDlg::OnBnClickedBtnLogin()//��¼
{
	if (isLogin) //����Ѿ���¼,����Ҫִ��ע��
	{
		NET_DVR_Logout(lUserID1);
		NET_DVR_Logout(lUserID2);
		isLogin = FALSE;
		GetDlgItem(IDC_BTN_Login)->SetWindowTextA("��¼"); 
		GetDlgItem(IDC_BTN_PlayCam)->EnableWindow(FALSE);  //���Ű�ť ʧЧ
		GetDlgItem(IDC_BTN_SavePicture)->EnableWindow(FALSE);  // ץͼ��ťʧЧ
		GetDlgItem(IDC_BTN_SavePicture2)->EnableWindow(FALSE);  // ץͼ��ťʧЧ
		GetDlgItem(IDC_BTN_SaveRealData)->EnableWindow(FALSE);  // ץͼ��ťʧЧ
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
		if (lUserID1 < 0 || lUserID2 < 0) //��¼ʧ��ʱ,����������ť����Ҫdisable
		{
			AfxMessageBox("��¼ʧ��!");
			isLogin = FALSE;
		} 
		else
		{
			isLogin = TRUE;
			GetDlgItem(IDC_BTN_Login)->SetWindowTextA("ע��");  //��¼�ɹ�, ���ע��
			GetDlgItem(IDC_BTN_PlayCam)->EnableWindow(TRUE);  //���Ű�ť ��Ч
		}
	}
}

char* itostr(char *str, int i) //��iת��λ�ַ�������str
{
	sprintf(str, "%d", i);
	return str;
}


void CSingleCamDlg::OnBnClickedBtnPlaycam() //����
{

		NET_DVR_PREVIEWINFO struPlayInfo1 = {0};
		struPlayInfo1.hPlayWnd = hPlayWnd1;         //��ҪSDK����ʱ�����Ϊ��Чֵ����ȡ��������ʱ����Ϊ��
		struPlayInfo1.lChannel     = 1;       //Ԥ��ͨ����
		struPlayInfo1.dwStreamType = 0;       //0-��������1-��������2-����3��3-����4���Դ�����
		struPlayInfo1.dwLinkMode   = 0;       //0- TCP��ʽ��1- UDP��ʽ��2- �ಥ��ʽ��3- RTP��ʽ��4-RTP/RTSP��5-RSTP/HTTP

		NET_DVR_PREVIEWINFO struPlayInfo2 = { 0 };
		struPlayInfo2.hPlayWnd = hPlayWnd2;         //��ҪSDK����ʱ�����Ϊ��Чֵ����ȡ��������ʱ����Ϊ��
		struPlayInfo2.lChannel = 1;       //Ԥ��ͨ����
		struPlayInfo2.dwStreamType = 0;       //0-��������1-��������2-����3��3-����4���Դ�����
		struPlayInfo2.dwLinkMode = 0;       //0- TCP��ʽ��1- UDP��ʽ��2- �ಥ��ʽ��3- RTP��ʽ��4-RTP/RTSP��5-RSTP/HTTP


		lRealHandle1 = NET_DVR_RealPlay_V40(lUserID1, &struPlayInfo1, g_RealDataCallBack_V30, NULL);
		lRealHandle2 = NET_DVR_RealPlay_V40(lUserID2, &struPlayInfo2, g_RealDataCallBack_V30, NULL);

		if (lRealHandle1 < 0 || lRealHandle2 < 0)
		{
			AfxMessageBox("����ʧ��!");
			isPlaying = FALSE;//���ű�־λ����Ϊ��
		} 

		else
		{
			GetDlgItem(IDC_BTN_Login)->EnableWindow(FALSE);  //��¼��ťʧЧ
			GetDlgItem(IDC_BTN_PlayCam)->EnableWindow(FALSE);  //���Ű�ťʧЧ
			GetDlgItem(IDC_BTN_StopCam)->EnableWindow(TRUE);  //ֹͣ��ť ��Ч
			GetDlgItem(IDC_BTN_SavePicture)->EnableWindow(TRUE); // ץͼ��ť ��Ч
			GetDlgItem(IDC_BTN_SavePicture2)->EnableWindow(TRUE); // ץͼ2��ť ��Ч
			GetDlgItem(IDC_BTN_SaveRealData)->EnableWindow(TRUE); // ��Ƶ���水ť ��Ч
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
		AfxMessageBox("ץȡͼƬʧ��!");
}

void CSingleCamDlg::OnBnClickedBtnSavepicture2()
{
	BOOL isSave;

	char* filename = new char(20);
	sprintf(filename, "E:\\peng\\cam2.jpg");
	isSave = NET_DVR_CapturePicture(lRealHandle2, filename);
	if (isSave == FALSE)
		AfxMessageBox("ץȡͼƬʧ��!");
}

void CSingleCamDlg::OnBnClickedBtnStopsave()
{
	saveFlag = FALSE;
	GetDlgItem(IDC_BTN_SavePicture)->EnableWindow(TRUE); // ֹͣץͼ��ť ��Ч
	BOOL isSave1, isSave2;
	char* filename1 = new char(20);
	char* filename2 = new char(20);
	sprintf(filename1, "E:\\peng\\1test.wmv");
	isSave1 = NET_DVR_SaveRealData(lRealHandle1, filename1);
	sprintf(filename2, "E:\\peng\\2test.wmv");
	isSave2 = NET_DVR_SaveRealData(lRealHandle2, filename2);
	if (isSave1 == FALSE || isSave2 == FALSE)
	{
		AfxMessageBox("������Ƶ�ļ�ʧ��!");
	}
}


void CSingleCamDlg::OnBnClickedBtnStopcam()//ֹͣ
{

	NET_DVR_StopRealPlay(lRealHandle1);
	NET_DVR_StopRealPlay(lRealHandle2);

	//�ͷŲ��ſ���Դ
	PlayM4_Stop(nPort1);
	PlayM4_CloseStream(nPort1);
	PlayM4_FreePort(nPort1);
	PlayM4_Stop(nPort2);
	PlayM4_CloseStream(nPort2);
	PlayM4_FreePort(nPort2);
	GetDlgItem(IDC_BTN_Login)->EnableWindow(TRUE);  //��¼��ťʧЧ
	GetDlgItem(IDC_BTN_PlayCam)->EnableWindow(TRUE);  //���Ű�ť ��Ч
	GetDlgItem(IDC_BTN_StopCam)->EnableWindow(FALSE);  //ֹͣ��ť ʧЧ
	GetDlgItem(IDC_BTN_SavePicture)->EnableWindow(FALSE); // ץͼ��ť ʧЧ
	GetDlgItem(IDC_BTN_SavePicture2)->EnableWindow(FALSE); // ץͼ��ť ʧЧ
	GetDlgItem(IDC_BTN_StopSave)->EnableWindow(FALSE); // ֹͣץͼ��ť ʧЧ
}



void CSingleCamDlg::OnBnClickedBtnSaveRealData()
{
	saveFlag = FALSE;
	GetDlgItem(IDC_BTN_StopSave)->EnableWindow(TRUE); // ֹͣ���水ť ��Ч
	BOOL isSave1, isSave2;
	char* filename1 = new char(20);
	char* filename2 = new char(20);
	sprintf(filename1, "E:\\peng\\1test.mp4");
	isSave1 = NET_DVR_SaveRealData(lRealHandle1, filename1);
	sprintf(filename2, "E:\\peng\\2test.mp4");
	isSave2 = NET_DVR_SaveRealData(lRealHandle2, filename2);

	if (isSave1 == FALSE)
	{
		AfxMessageBox("����������ͷ������Ƶ�ļ�ʧ��!");
	}

	if (isSave2 == FALSE)
	{
		AfxMessageBox("�Ҵ���������ͷ������Ƶ�ļ�ʧ��!");
	}
}
