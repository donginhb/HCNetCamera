
// SingleCamDlg.h : ͷ�ļ�
//

#pragma once


// CSingleCamDlg �Ի���
class CSingleCamDlg : public CDialogEx
{
// ����
public:
	CSingleCamDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CIPAddressCtrl m_ctrlDeviceIP1;
	CIPAddressCtrl m_ctrlDeviceIP2;
	BOOL isLogin ;//�Ƿ��ѵ�¼
	BOOL isPlaying;//�Ƿ����ڲ���


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
