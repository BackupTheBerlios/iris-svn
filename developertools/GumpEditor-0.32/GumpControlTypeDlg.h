#pragma once

class CGumpControlTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CGumpControlTypeDlg)

public:
	CGumpControlTypeDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CGumpControlTypeDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CTRL_TYPE_DIALOG };
	enum { PICTURE=0, BORDER, PAPERDOLL, BUTTON, CHECKBOX, RADIO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int m_nControlType;
	int GetControlType() { return m_nControlType; }
};
 