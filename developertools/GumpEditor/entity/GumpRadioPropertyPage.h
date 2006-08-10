#pragma once


// CGumpRadioPropertyPage ��ȭ �����Դϴ�.

#include "DiagramPropertyPage.h"
#include "numspinctrl.h"
#include "afxcmn.h"

class CGumpRadioPropertyPage : public CDiagramPropertyPage
{
	DECLARE_DYNAMIC(CGumpRadioPropertyPage)

public:
	CGumpRadioPropertyPage(CWnd* pParent=NULL);
	virtual ~CGumpRadioPropertyPage();

	CHoverButton m_btnTest;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_RADIO_DIALOG };

	virtual void SetValues();
	virtual void ApplyValues();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	CNumSpinCtrl m_spinGroupID;
	int m_iGroupID;
};
