#pragma once


// CGumpView ���Դϴ�.

class CDib;

class CGumpView : public CView
{
	DECLARE_DYNCREATE(CGumpView)
public:
	CGumpPtr m_pDib;
	CGumpPtr GetGump() { return m_pDib; }
	void SetGump(CGumpPtr pDIb);

protected:
	CGumpView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CGumpView();

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
	
};


