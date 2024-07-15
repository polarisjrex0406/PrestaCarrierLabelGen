#pragma once
#include "afxdialogex.h"


// CPrestaInputDialog dialog

class CPrestaInputDialog : public CDialog
{
	DECLARE_DYNAMIC(CPrestaInputDialog)

public:
	CPrestaInputDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPrestaInputDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CPrestaInputDialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	CString GetContent() { return m_strContent; }

private:
	CString m_strContent;
};
