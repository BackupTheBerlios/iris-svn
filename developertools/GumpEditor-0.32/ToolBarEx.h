#ifndef __TOOLBAREX_H__
#define __TOOLBAREX_H__

/////////////////////////////////////////////////////////////////////////////
// CSmartComboBox

class CSmartComboBox : public CComboBox
{
public:
					CSmartComboBox();

	int				m_nMaxStrings;

	BOOL			CreateFont(LONG lfHeight, LONG lfWeight, LPCTSTR lpszFaceName);
	int				AddString(LPCTSTR str);
	int				InsertString(int index, LPCTSTR str);

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEditPaste();
};

#define USE_SMART_COMBO_BOX

class CToolBarWithCombo : public CToolBar
{
public:

#ifdef USE_SMART_COMBO_BOX
	CSmartComboBox	m_comboBox1;
	CSmartComboBox	m_comboBox2;
#else
	CComboBox		m_comboBox1;
	CComboBox		m_comboBox2;
#endif

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolBarWithCombo)
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	BOOL			CreateComboBox(class CComboBox& comboBox, UINT nIndex, UINT nID, int nWidth, int nDropHeight);

// Generated message map functions
protected:
	//{{AFX_MSG(CToolBarWithCombo)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __TOOLBAREX_H__
