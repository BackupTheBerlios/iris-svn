#pragma once


// CTestPage ��ȭ �����Դϴ�.

class CTestPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CTestPage)

public:
	CTestPage();
	virtual ~CTestPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PROPPAGE_LARGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
