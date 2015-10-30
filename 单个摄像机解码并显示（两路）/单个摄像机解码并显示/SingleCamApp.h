
// 单个摄像机解码并显示.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CSingleCamApp:
// 有关此类的实现，请参阅 单个摄像机解码并显示.cpp
//

class CSingleCamApp : public CWinApp
{
public:
	CSingleCamApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CSingleCamApp theApp;