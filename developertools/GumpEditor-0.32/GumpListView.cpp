// LeftView.cpp : implementation of the CGumpListView class
//

#include "stdafx.h"
#include "GumpEditor.h"

#include "GumpEditorDoc.h"
#include "GumpListView.h"

#include <afxadv.h>
#include ".\gumplistview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGumpListView

IMPLEMENT_DYNCREATE(CGumpListView, CListView)

BEGIN_MESSAGE_MAP(CGumpListView, CListView)
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnLvnBegindrag)
	ON_WM_LBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_POPUP_COPYGUMPID, OnPopupCopygumpid)
	ON_COMMAND(ID_POPUP_INSERTGUMP, OnPopupInsertgump)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CGumpListView construction/destruction

CGumpListView::CGumpListView()
: m_bInit(false)
{
	// TODO: add construction code here
	m_nIDClipFormat = RegisterClipboardFormat("GumpEditor");
}

CGumpListView::~CGumpListView()
{
}

BOOL CGumpListView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;
	return CListView::PreCreateWindow(cs);
}

void CGumpListView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
	if (m_bInit) return;
	m_bInit = true;

	CListCtrl& ctrl = GetListCtrl();

	ctrl.ModifyStyle(0, LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS);
	ctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	HGDIOBJ hFont = ::GetStockObject(OEM_FIXED_FONT);
	::GetObject(hFont, sizeof(lf), &lf);

	lf.lfHeight = 100;
	lf.lfPitchAndFamily = FIXED_PITCH;

	CFont font;

	font.CreatePointFontIndirect(&lf);

	ctrl.SetFont(&font);
	font.Detach();

	ctrl.InsertColumn(0, "Gump ID", LVCFMT_RIGHT,60);
	ctrl.InsertColumn(1, "Size", LVCFMT_RIGHT,120);

	CString strText;
	cGumpLoader* pGumpLoader = GetDocument()->GetGumpLoader();

	int w=0,h=0,iItem=0;
	for (int i = 0; i < pGumpLoader->GetGumpCount(); i++)
	{
		pGumpLoader->GetGumpSize(i, w, h);
		if (w==0 || h==0) continue;
		
		strText.Format("0x%04X", i);
		iItem = ctrl.InsertItem(i, strText);
		strText.Format("%dx%d", w,h);
		ctrl.SetItemText(iItem, 1, strText);
		ctrl.SetItemData(iItem, i);

		//if (i > 100) break;
	}
	
}


// CGumpListView diagnostics

#ifdef _DEBUG
void CGumpListView::AssertValid() const
{
	CListView::AssertValid();
}

void CGumpListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CGumpEditorDoc* CGumpListView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGumpEditorDoc)));
	return (CGumpEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CGumpListView message handlers

void CGumpListView::OnSize(UINT nType, int cx, int cy)
{
	CListView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CGumpListView::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	UpdateGump();
}

int CGumpListView::GetSelectedGumpID(void)
{
	int iItem = GetSelectedItem();
	if (iItem < 0) return -1;

	return GetListCtrl().GetItemData(iItem);
}

void CGumpListView::UpdateGump(void)
{
	int iGumpID = GetSelectedGumpID();
	if (iGumpID < 0) return;

	GetDocument()->SelectGump(iGumpID);
}

void CGumpListView::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	
	COleDataSource srcItem;
	CString sType = _T("");
	HGLOBAL hTextData = 0;	

	CSharedFile clipb (GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT);

	dynDropSource.nControlType = DYN_PICTURE;

	CString strText;
	strText.Format("%d",0);
	sType = strText;

	clipb.Write(sType, sType.GetLength()*sizeof(TCHAR));
	hTextData = clipb.Detach();

	srcItem.CacheGlobalData(m_nIDClipFormat, hTextData);
	srcItem.DoDragDrop(DROPEFFECT_COPY,NULL,&dynDropSource);
}

void CGumpListView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CListView::OnLButtonUp(nFlags, point);
}

void CGumpListView::SelectGump(int iGumpID)
{
	LVFINDINFO info={LVFI_PARAM, NULL, iGumpID, 0 };
	CListCtrl& ctrl = GetListCtrl();
	int iItem = ctrl.FindItem(&info);
	if (iItem == -1) return;
	ctrl.SetItemState(iItem, LVNI_SELECTED, LVNI_SELECTED);
	ctrl.EnsureVisible(iItem, FALSE);
}

void CGumpListView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (GetSelectedGumpID() < 0) return;

	CMenu menu;
	menu.LoadMenu(IDR_GUMPLIST_MENU);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, point.x,point.y,this);
}

void CGumpListView::OnPopupCopygumpid()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	int iGumpID = GetSelectedGumpID();
	CString strText = GfxXtoA(iGumpID);
	
	if  (!OpenClipboard()) return;
	
	EmptyClipboard();
	HGLOBAL hClipboardData;
	hClipboardData = GlobalAlloc(GMEM_DDESHARE, strText.GetLength()+1);

	char * pchData = (char*)GlobalLock(hClipboardData);
	strcpy(pchData, strText);
	GlobalUnlock(hClipboardData);
	SetClipboardData(CF_TEXT,hClipboardData);

	CloseClipboard();

}

void CGumpListView::OnPopupInsertgump()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	int iGumpID = GetSelectedGumpID();

	GetDocument()->InsertGump(iGumpID);
}

int CGumpListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

