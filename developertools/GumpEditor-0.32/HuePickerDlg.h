#pragma once

#include "HueWnd.h"

class CHuePickerDlg : public CDialog
{
	DECLARE_DYNAMIC(CHuePickerDlg)

public:
	CHuePickerDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CHuePickerDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG1 };
	CHueWnd m_wndHue;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
