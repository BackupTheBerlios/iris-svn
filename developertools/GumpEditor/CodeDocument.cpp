// CodeDocument.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GumpEditor.h"
#include "CodeDocument.h"


// CCodeDocument

IMPLEMENT_DYNCREATE(CCodeDocument, CDocument)

CCodeDocument::CCodeDocument()
{
}

BOOL CCodeDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CCodeDocument::~CCodeDocument()
{
}


BEGIN_MESSAGE_MAP(CCodeDocument, CDocument)
END_MESSAGE_MAP()


// CCodeDocument �����Դϴ�.

#ifdef _DEBUG
void CCodeDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CCodeDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCodeDocument serialization�Դϴ�.

void CCodeDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CCodeDocument ����Դϴ�.
