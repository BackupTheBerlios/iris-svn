// GrannyViewer.h : GrannyViewer ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"       // �� ��ȣ


// CGrannyViewerApp:
// �� Ŭ������ ������ ���ؼ��� GrannyViewer.cpp�� �����Ͻʽÿ�.
//


class CGrannyViewerApp : public CWinApp
{
public:
	CGrannyViewerApp();

// ������
public:
	virtual BOOL InitInstance();

	//int m_iModelID;
	//int GetOpenModelID() { return m_iModelID; }
	//void SetOpenModelID(int id) { m_iModelID = id; }

	void ReadConfig();
	void SaveConfig();

// ����
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
public:
	afx_msg void OnLogSetuplog();
};

extern CGrannyViewerApp theApp;
