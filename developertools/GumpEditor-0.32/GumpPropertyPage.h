#pragma once

#include "DiagramPropertyPage.h"
#include "NumSpinCtrl.h"

class CGumpPropertyPage : public CDiagramPropertyPage
{
	DECLARE_DYNAMIC(CGumpPropertyPage)

public:
	CGumpPropertyPage();
	virtual ~CGumpPropertyPage();
	virtual void SetValues();
	virtual void ApplyValues();
	virtual void ResetValues();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_GUMP_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

	
	CNumSpinCtrl m_spinX;
	CNumSpinCtrl m_spinY;
	CNumSpinCtrl m_spinW;
	CNumSpinCtrl m_spinH;
	CNumSpinCtrl m_spinAlpha;
	int m_xPos;
	int m_yPos;
	int m_iWidth;
	int m_iHeight;
	CString m_strEvClick;
	CString m_strEvClose;
	CString m_strEvMouseUp;
	CString m_strEvMouseDown;
	CString m_strEvKeyPressed;
	CString m_strName;
	CString m_strType;
	int m_iAlpha;
public:
	virtual BOOL OnInitDialog();
	
};
