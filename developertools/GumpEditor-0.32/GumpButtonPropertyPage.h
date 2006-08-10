#pragma once


// CGumpButtonPropertyPage ��ȭ �����Դϴ�.

#include "DiagramPropertyPage.h"
#include "HoverButton.h"
#include "numspinctrl.h"

class CGumpButtonPropertyPage : public CDiagramPropertyPage
{
	DECLARE_DYNAMIC(CGumpButtonPropertyPage)

public:
	CGumpButtonPropertyPage(CWnd* pParent=NULL);
	virtual ~CGumpButtonPropertyPage();

	CHoverButton m_btnTest;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_BUTTON_DIALOG };

	virtual void SetValues();
	virtual void ApplyValues();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();


	CString m_strNormalID;
	CString m_strHoverID;
	CString m_strPressedID;
	CNumSpinCtrl m_spinNormal;
	CNumSpinCtrl m_spinHover;
	CNumSpinCtrl m_spinPressed;
};
