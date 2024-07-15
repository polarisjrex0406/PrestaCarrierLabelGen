// PrestaInputDialog.cpp : implementation file
//

#include "pch.h"
#include "PrestaCarrierLabelGen.h"
#include "afxdialogex.h"
#include "PrestaInputDialog.h"


// CPrestaInputDialog dialog

IMPLEMENT_DYNAMIC(CPrestaInputDialog, CDialog)

CPrestaInputDialog::CPrestaInputDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CPrestaInputDialog, pParent)
{

}

CPrestaInputDialog::~CPrestaInputDialog()
{
}

void CPrestaInputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPrestaInputDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CPrestaInputDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CPrestaInputDialog message handlers


void CPrestaInputDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_EDIT_CONTENT, m_strContent);
	if (m_strContent.GetLength() == 0) {
		return;
	}
	CDialog::OnOK();
}
