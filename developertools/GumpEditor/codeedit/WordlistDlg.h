#if !defined(AFX_WORDLISTDLG_H__2C1F390C_D933_4CA8_99C8_5506F0E85B2B__INCLUDED_)
#define AFX_WORDLISTDLG_H__2C1F390C_D933_4CA8_99C8_5506F0E85B2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WordlistDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CWordlistDlg 

class CWordlistDlg : public CDialog
{
// Konstruktion
public:
	CWordlistDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CWordlistDlg)
	enum { IDD = IDD_WORDLIST };
	CString	m_strWordlistNumber;
	CString	m_strKeywords;
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(CWordlistDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CWordlistDlg)
		// HINWEIS: Der Klassen-Assistent f�gt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_WORDLISTDLG_H__2C1F390C_D933_4CA8_99C8_5506F0E85B2B__INCLUDED_
