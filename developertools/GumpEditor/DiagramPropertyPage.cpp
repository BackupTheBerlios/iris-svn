// DiagramPropertyPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GumpEditor.h"
#include "DiagramPropertyPage.h"
#include ".\diagrampropertypage.h"


// CDiagramPropertyPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDiagramPropertyPage, CPropertyPage)
CDiagramPropertyPage::CDiagramPropertyPage( UINT nIDTemplate) : CPropertyPage(nIDTemplate)
{
	m_entity = NULL;
	m_redrawWnd = NULL;
}

CDiagramPropertyPage::~CDiagramPropertyPage()
{
}

void CDiagramPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDiagramPropertyPage, CPropertyPage)
END_MESSAGE_MAP()



BOOL CDiagramPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SetValues();

	return TRUE;  
}
