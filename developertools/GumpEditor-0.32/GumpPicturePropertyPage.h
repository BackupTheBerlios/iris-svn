#pragma once

#include "DiagramPropertyPage.h"
#include "numspinctrl.h"

class CGumpPicturePropertyPage : public CDiagramPropertyPage
{
	DECLARE_DYNAMIC(CGumpPicturePropertyPage)

public:
	CGumpPicturePropertyPage(CWnd* pParent=NULL);
	virtual ~CGumpPicturePropertyPage();
	
	enum { IDD = IDD_PICTURE_DIALOG };

	virtual void SetValues();
	virtual void ApplyValues();
	virtual void ResetValues();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strGumpID;
	CNumSpinCtrl m_spinGump;

	virtual BOOL OnInitDialog();
};
