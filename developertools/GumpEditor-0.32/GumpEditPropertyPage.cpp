// GumpEditPropertyDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GumpEditor.h"
#include ".\GumpEditPropertyPage.h"
#include "GumpEditorDoc.h"
#include "GumpEdit.h"
#include "iris/config.h"


IMPLEMENT_DYNAMIC(CGumpEditPropertyPage, CDiagramPropertyPage)

CGumpEditPropertyPage::CGumpEditPropertyPage(CWnd* pParent)
	: CDiagramPropertyPage(CGumpEditPropertyPage::IDD)
	, m_bPassword(FALSE)
{
}

CGumpEditPropertyPage::~CGumpEditPropertyPage()
{
}

void CGumpEditPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_PASSWORD_CHECK, m_bPassword);
}


BEGIN_MESSAGE_MAP(CGumpEditPropertyPage, CDiagramPropertyPage)
END_MESSAGE_MAP()


BOOL CGumpEditPropertyPage::OnInitDialog()
{ 
	CDiagramPropertyPage::OnInitDialog();

	return TRUE;  
}


void CGumpEditPropertyPage::SetValues()
{
	if( GetSafeHwnd() && GetEntity() )
	{
		CGumpEdit* pEdit= (CGumpEdit*)GetEntity();

		m_bPassword = pEdit->IsPasswordMode();
		
		UpdateData(FALSE);
	}
}

void CGumpEditPropertyPage::ApplyValues()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( GetSafeHwnd() && GetEntity() )
	{
		UpdateData();
		CGumpEdit* pEdit = (CGumpEdit*)GetEntity();

		pEdit->SetPasswordMode(m_bPassword);
		
		Redraw();
	}
}

