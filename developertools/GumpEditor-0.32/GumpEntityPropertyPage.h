#pragma once


// CGumpEntityPropertyDlg ��ȭ �����Դϴ�.

class CGumpEntityPropertyDlg : public CDialog
{
	DECLARE_DYNAMIC(CGumpEntityPropertyDlg)

public:
	CGumpEntityPropertyDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CGumpEntityPropertyDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_GUMP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
