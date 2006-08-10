#pragma once

#include "HueWnd.h"



class CHuePickerDlg : public CDialog
{
	DECLARE_DYNAMIC(CHuePickerDlg)

public:
	CHuePickerDlg(DWORD hueId=1,CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CHuePickerDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_HUE_DIALOG };
	CHueWnd m_wndHue;
	COLORREF m_color;
	DWORD m_hueId;

	COLORREF GetColor() { return m_color; }
	DWORD GetHueId() { return m_hueId; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strHueID;
	CString m_strRGB;
	BOOL m_bRGBHex;
	void UpdateRGB();
	void SelectHueId(DWORD hueId);
	void SelectColor(CString strColor);

	afx_msg LRESULT OnHueChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedCheck1();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL m_bHueIDHex;
	afx_msg void OnBnClickedCheck2();
};
