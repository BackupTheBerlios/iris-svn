// TestPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GumpEditor.h"
#include "TestPage.h"


// CTestPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTestPage, CPropertyPage)
CTestPage::CTestPage()
	: CPropertyPage(CTestPage::IDD)
{
}

CTestPage::~CTestPage()
{
}

void CTestPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestPage, CPropertyPage)
END_MESSAGE_MAP()


// CTestPage �޽��� ó�����Դϴ�.
