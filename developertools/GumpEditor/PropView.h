#pragma once


// CPropView ���Դϴ�.

#include "proptree/PropTree.h"

class CGumpEditorDoc;
class CDynControl;

class CPropView : public CView
{
	DECLARE_DYNCREATE(CPropView)
public:
	CPropTree m_Tree;
	CDynControl* m_pControl;

	CGumpEditorDoc* GetDocument() { return (CGumpEditorDoc*)m_pDocument; }

protected:
	CPropView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CPropView();

public:
	virtual void OnDraw(CDC* pDC);      // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif



protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	void UpdatePropView(void);
};


