
// prjClip.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// prjClipApp:
// �йش����ʵ�֣������ prjClipApp.cpp
//

class prjClipApp : public CWinApp
{
public:
	prjClipApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern prjClipApp theApp;