#if !defined(AFX_GOTOPOSDLG_H__413AA17B_038D_4F60_867D_6AE551F2E49F__INCLUDED_)
#define AFX_GOTOPOSDLG_H__413AA17B_038D_4F60_867D_6AE551F2E49F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GotoPosDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CGotoPosDlg 

class CGotoPosDlg : public CDialog
{
// Konstruktion
public:
	CGotoPosDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CGotoPosDlg)
	enum { IDD = IDD_POS };
	int		m_nPos;
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(CGotoPosDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CGotoPosDlg)
		// HINWEIS: Der Klassen-Assistent f�gt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_GOTOPOSDLG_H__413AA17B_038D_4F60_867D_6AE551F2E49F__INCLUDED_
