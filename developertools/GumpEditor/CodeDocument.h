#pragma once


// CCodeDocument �����Դϴ�.

class CCodeDocument : public CDocument
{
	DECLARE_DYNCREATE(CCodeDocument)

public:
	CCodeDocument();
	virtual ~CCodeDocument();
	virtual void Serialize(CArchive& ar);   // ���� ��/����� ���� �����ǵǾ����ϴ�.
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
