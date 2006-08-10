// GumpView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GumpEditor.h"
#include "GumpView.h"
#include ".\gumpview.h"

// CGumpView

IMPLEMENT_DYNCREATE(CGumpView, CView)

CGumpView::CGumpView()
{
}

CGumpView::~CGumpView()
{
}

BEGIN_MESSAGE_MAP(CGumpView, CView)
END_MESSAGE_MAP()


// CGumpView �׸����Դϴ�.

void CGumpView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
	if (!m_pDib) return;

	CRect rect;
	GetClientRect(rect);

	CSize sizeWnd = rect.Size();
	CSize sizeDib = m_pDib->GetDimensions();

	if (sizeDib.cx <= 0 || sizeDib.cy <= 0) return;
	
	double ratio = min(float(sizeWnd.cx) / sizeDib.cx, float(sizeWnd.cy) / sizeDib.cy);

	if (ratio < 1.0) {
		sizeDib = CSize(int(sizeDib.cx * ratio), int(sizeDib.cy * ratio));
	}

	CPoint point;
	point.x = max(0, sizeWnd.cx /2 - sizeDib.cx/2);
	point.y = max(0, sizeWnd.cy /2 - sizeDib.cy/2);

	CRect rcBound(point.x-1, point.y-1, point.x+sizeDib.cx+1, point.y+sizeDib.cy+1);
	
	CPen pen(PS_DOT, 1, RGB(128,128,128));
	CPen* pOld = pDC->SelectObject(&pen);
	pDC->Rectangle(rcBound);
	pDC->SelectObject(pOld);

	m_pDib->Draw(pDC, point, sizeDib);
}


// CGumpView �����Դϴ�.

#ifdef _DEBUG
void CGumpView::AssertValid() const
{
	CView::AssertValid();
}

void CGumpView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CGumpView �޽��� ó�����Դϴ�.

void CGumpView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}

void CGumpView::SetGump(CGumpPtr pDib)
{
	m_pDib = pDib;
	Invalidate();
}
