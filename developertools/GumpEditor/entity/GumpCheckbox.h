#pragma once 

#include "GumpStatic.h"
#include "GumpCheckboxPropertyPage.h"

class CGumpCheckbox : public CGumpStatic
{
	// Construction
public:
	CGumpCheckbox(CGumpPtr pNormal, CGumpPtr pChecked, bool bCheck=false);

	virtual BOOL	FromString( XML::Node* node );
	virtual CString	GetString(BOOL bBegin) const;
	static	CGumpEntity* CreateFromNode( XML::Node* node );

	// Overrides
	virtual CDiagramEntity* Clone();
	virtual void			Draw( CDC* dc, CRect rect );

	enum { TEXT_MARGIN = 10, DEFAULT_SIZE = 100 };
public:
	enum STATE { NORMAL=0, CHECKED, NUM_STATE };
	CGumpPtr m_pGump[NUM_STATE];
	bool m_bChecked;

	bool SetGump(CGumpPtr pNormal, CGumpPtr pChecked);
	void GetGump(CGumpPtr& pNormal, CGumpPtr& pChecked);

	int GetGumpID(STATE state) const;
	CGumpPtr GetGump(STATE state);
	void SetGump(STATE state, CGumpPtr pGump);

	bool IsChecked() const { return m_bChecked; }
	void SetCheck(bool bCheck) { m_bChecked = bCheck; }
	
private:
	CGumpCheckboxPropertyPage	m_page;
};
