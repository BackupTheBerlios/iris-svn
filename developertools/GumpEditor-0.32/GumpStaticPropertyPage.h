#pragma once

#include "DiagramPropertyPage.h"
#include "colorpicker/ColourPickerXP.h"

class CGumpStaticPropertyPage :public CDiagramPropertyPage
{
	DECLARE_DYNAMIC(CGumpStaticPropertyPage)

public:
	CGumpStaticPropertyPage(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CGumpStaticPropertyPage();

	enum { IDD = IDD_STATIC_DIALOG };
	
	virtual void SetValues();
	virtual void ApplyValues();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

	CColourPickerXP m_btnColor;
	int GetFontId();
public:
	virtual BOOL OnInitDialog();
	int m_iTextAlign;
	CString m_strText;
	CComboBox m_cbFont;
	CStatic m_strSample;
	CStatic m_stSample;
	CString m_strFontId;
	afx_msg void OnCbnSelchangeFontCombo();
	afx_msg void OnPaint();
	afx_msg void OnEnChangeText();
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg LRESULT OnColorChange(WPARAM,LPARAM);
};
