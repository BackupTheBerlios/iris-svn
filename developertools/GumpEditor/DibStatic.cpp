// DibStatic.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GumpEditor.h"
#include "DibStatic.h"
#include ".\dibstatic.h"


// CDibStatic

IMPLEMENT_DYNAMIC(CDibStatic, CStatic)
CDibStatic::CDibStatic(CGumpPtr pDib) : m_pDib(pDib)
{
}

CDibStatic::~CDibStatic()
{
}


BEGIN_MESSAGE_MAP(CDibStatic, CStatic)
	ON_WM_DRAWITEM()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CDibStatic �޽��� ó�����Դϴ�.


BOOL CDibStatic::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CRect rc(rect);
	if (m_pDib)
	{
		CSize size = m_pDib->GetDimensions();
		rc.right  = rc.left + size.cx;
		rc.bottom = rc.top + size.cy;
	}

	return CStatic::Create(lpszText, dwStyle, rc, pParentWnd, nID);
}

void CDibStatic::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	
	//CStatic::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CDibStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CStatic::OnPaint()��(��) ȣ������ ���ʽÿ�.
	if (!m_pDib) return;

	m_pDib->Draw(&dc, CPoint(0,0));
	//m_pDib->DrawTransparent(&dc,CPoint(0,0),RGB(255,255,255));
}

int CDibStatic::GetGumpID(void)
{
	if (m_pDib) return m_pDib->GetGumpID();
	
	return -1;
}

void CDibStatic::SetGump(CGumpPtr pDib)
{
	ASSERT(pDib);
	m_pDib = pDib;

	CRect rc;
	GetWindowRect(rc);
	GetParent()->ScreenToClient(rc);

	CSize size = m_pDib->GetDimensions();
	rc.right  = rc.left + size.cx;
	rc.bottom = rc.top + size.cy;
	
	MoveWindow(rc);
}
