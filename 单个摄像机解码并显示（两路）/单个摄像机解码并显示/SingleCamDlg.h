
// SingleCamDlg.h : 头文件
//

#pragma once


// CSingleCamDlg 对话框
class CSingleCamDlg : public CDialogEx
{
// 构造
public:
	CSingleCamDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CIPAddressCtrl m_ctrlDeviceIP1;
	CIPAddressCtrl m_ctrlDeviceIP2;
	BOOL isLogin ;//是否已登录
	BOOL isPlaying;//是否正在播放


public:
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnPlaycam();
	afx_msg void OnBnClickedBtnStopcam();
	afx_msg void OnBnClickedBtnSavePicture();
	afx_msg void OnBnClickedBtnStopsave();
	afx_msg void OnBnClickedBtnSavepicture2();
	afx_msg void OnStnClickedViewwindow();
	afx_msg void OnIpnFieldchangedCamIp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStnClickedViewwindow2();
	afx_msg void OnBnClickedBtnSaveRealData();
	//afx_msg void OnIpnFieldchangedCamIp2(NMHDR *pNMHDR, LRESULT *pResult);
};
