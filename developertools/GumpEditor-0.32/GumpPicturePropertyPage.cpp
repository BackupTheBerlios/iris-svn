// DynPropPicturePage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GumpEditor.h"
#include ".\GumpPicturePropertyPage.h"
#include "GumpPicture.h"
#include "GumpEditorDoc.h"


// CGumpPicturePropertyPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CGumpPicturePropertyPage, CDiagramPropertyPage)

CGumpPicturePropertyPage::CGumpPicturePropertyPage(CWnd* pParent)
	: CDiagramPropertyPage(CGumpPicturePropertyPage::IDD)
	, m_strGumpID("0")
{
}

CGumpPicturePropertyPage::~CGumpPicturePropertyPage()
{
}

void CGumpPicturePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CDiagramPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_GUMP_ID, m_strGumpID);
	DDX_Control(pDX, IDC_GUMP_SPIN, m_spinGump);
}


BEGIN_MESSAGE_MAP(CGumpPicturePropertyPage, CDiagramPropertyPage)
END_MESSAGE_MAP()


// CGumpPicturePropertyPage �޽��� ó�����Դϴ�.


BOOL CGumpPicturePropertyPage::OnInitDialog()
{
	CDiagramPropertyPage::OnInitDialog();

	m_spinGump.SetRange32(0,0xffff);


	return TRUE; 
}

void CGumpPicturePropertyPage::SetValues()
{
	if( GetSafeHwnd() && GetEntity())
	{
		CGumpPicture* pPicture = (CGumpPicture*)GetEntity();
		m_strGumpID = GfxXtoA(pPicture->GetGumpID());
		UpdateData(FALSE);
	}
}

void CGumpPicturePropertyPage::ApplyValues()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (GetSafeHwnd() && GetEntity())
	{
		UpdateData();
		CGumpPicture* pPicture = (CGumpPicture*)GetEntity();

		int iGumpID = GfxAtoX(m_strGumpID);

		if (pPicture->GetGumpID() != iGumpID) {	
			ASSERT(GfxGetGumpDocument());
			CGumpPtr pGump = GfxGetGumpDocument()->LoadGump(iGumpID);
			if (!pGump) return;
			pPicture->SetGump(pGump);
			Redraw();
		} 
					
	}
}

void CGumpPicturePropertyPage::ResetValues()
{
	if( GetSafeHwnd() && GetEntity() )
	{	
		UpdateData();
		CGumpPicture* pPicture = (CGumpPicture*)GetEntity();

		pPicture->SetGump(pPicture->GetGump());
			
		Redraw();			
	}	
}


