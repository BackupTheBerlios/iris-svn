#pragma once
#include "afxcmn.h"

#include "SortListCtrl.h"

class CGumpEditorView;

class CGumpControlListDlg : public CDialog
{
	DECLARE_DYNAMIC(CGumpControlListDlg)

public:
	CGumpControlListDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CGumpControlListDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CTRL_LIST_DIALOG };

	CGumpEditorView* m_pView;

	enum UPDATE_TYPE { SELECT, VISIBLE, FREEZE };
	void UpdateEntity(UPDATE_TYPE updateType, BOOL bValue);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CSortListCtrl m_lsControl;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedUnselect();
	afx_msg void OnBnClickedUnfreeze();
	afx_msg void OnBnClickedHide();
	afx_msg void OnBnClickedUnhide();
	afx_msg void OnBnClickedAll();
	afx_msg void OnBnClickedNone();
	afx_msg void OnBnClickedInvert();
	afx_msg void OnBnClickedFreeze();
	CString m_strName;
	afx_msg void OnBnClickedFind();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
