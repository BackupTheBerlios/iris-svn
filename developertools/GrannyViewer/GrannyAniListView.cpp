// GrannyAniListView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GrannyViewer.h"
#include "GrannyViewerDoc.h"
#include "GrannyListView.h"
#include "GrannyAniListView.h"
#include ".\grannyanilistview.h"
#include "debug.h"



// CGrannyAniListView

IMPLEMENT_DYNCREATE(CGrannyAniListView, CGrannyListView)

CGrannyAniListView::CGrannyAniListView()
{
}

CGrannyAniListView::~CGrannyAniListView()
{
}

BEGIN_MESSAGE_MAP(CGrannyAniListView, CGrannyListView)
END_MESSAGE_MAP()


// CGrannyAniListView �����Դϴ�.

#ifdef _DEBUG
void CGrannyAniListView::AssertValid() const
{
	CGrannyListView::AssertValid();
}

void CGrannyAniListView::Dump(CDumpContext& dc) const
{
	CGrannyListView::Dump(dc);
}
#endif //_DEBUG


// CGrannyAniListView �޽��� ó�����Դϴ�.

void CGrannyAniListView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	DEBUG_TIMER_START("CGrannyAniListView::OnInitialUpdate()");

	CTreeCtrl& tree=GetTreeCtrl();
	tree.DeleteAllItems();

	CGrannyViewerDoc* pDoc = GetDocument(); ASSERT(pDoc);
	cGrannyLoader* pLoader = pDoc->GetGrannyLoader(); 
	if (!pLoader) return;
	
	HTREEITEM htAnim   = tree.InsertItem("Animations");

	InsertAnimation(tree, htAnim, pLoader);

	tree.Expand(htAnim, TVE_EXPAND);

	DEBUG_TIMER_STOP ("CGrannyAniListView::OnInitialUpdate()");
}
