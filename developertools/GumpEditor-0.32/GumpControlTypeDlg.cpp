// DynCtrlTypeDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GumpEditor.h"
#include "GumpControlTypeDlg.h"
#include ".\gumpcontroltypedlg.h"

// CGumpControlTypeDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CGumpControlTypeDlg, CDialog)
CGumpControlTypeDlg::CGumpControlTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGumpControlTypeDlg::IDD, pParent)
	, m_nControlType(0)
{
}

CGumpControlTypeDlg::~CGumpControlTypeDlg()
{
}

void CGumpControlTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_nControlType);
}


BEGIN_MESSAGE_MAP(CGumpControlTypeDlg, CDialog)
END_MESSAGE_MAP()


// CGumpControlTypeDlg �޽��� ó�����Դϴ�.


