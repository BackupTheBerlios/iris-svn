#pragma once


// CGrannyAniListView ���Դϴ�.

class CGrannyAniListView : public CGrannyListView
{
	DECLARE_DYNCREATE(CGrannyAniListView)

protected:
	CGrannyAniListView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CGrannyAniListView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};


