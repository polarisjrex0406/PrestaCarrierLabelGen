// SettingsDlg.cpp : implementation file
//

#include "pch.h"
#include "PrestaCarrierLabelGen.h"
#include "afxdialogex.h"
#include "SettingsDlg.h"


// SettingsDlg dialog

IMPLEMENT_DYNAMIC(SettingsDlg, CDialog)

SettingsDlg::SettingsDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SETTINGS_DIALOG, pParent)
{

}

SettingsDlg::~SettingsDlg()
{
}

void SettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_PRINTPREVIEW, m_chkPrintPreview);
	DDX_Control(pDX, IDC_COMBO_WEBSERVICEID, m_cmbWebserviceId);
	DDX_Control(pDX, IDC_EDIT_DPDID, m_edtDpdId);
	DDX_Control(pDX, IDC_EDIT_HOSTNAME, m_edtHostname);
	DDX_Control(pDX, IDC_EDIT_LOCATION, m_edtLocation);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_edtUsername);
	DDX_Control(pDX, IDC_EDIT_WEBSERVICEID, m_edtWebserviceId);
	DDX_Control(pDX, IDC_EDIT_ZIPCODE, m_edtZipcode);
	DDX_Control(pDX, IDC_EDIT_NEWPASSWORD, m_edtNewPassword);
	DDX_Control(pDX, IDC_EDIT_CONFIRM, m_edtConfirm);
	DDX_Control(pDX, IDC_EDIT_CURRENTPASSWORD, m_edtCurrentPassword);
	DDX_Control(pDX, IDC_BUTTON_PRINTER, m_btnPrinter);
}


BEGIN_MESSAGE_MAP(SettingsDlg, CDialog)
	ON_BN_CLICKED(IDOK, &SettingsDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_WEBSERVICEID, &SettingsDlg::OnSelchangeComboWebserviceid)
	ON_EN_KILLFOCUS(IDC_EDIT_LOCATION, &SettingsDlg::OnKillfocusEditLocation)
	ON_BN_CLICKED(IDC_CHECK_PRINTPREVIEW, &SettingsDlg::OnClickedCheckPrintpreview)
	ON_BN_CLICKED(IDC_BUTTON_PRINTER, &SettingsDlg::OnClickedButtonPrinter)
END_MESSAGE_MAP()


// SettingsDlg message handlers


void SettingsDlg::OnBnClickedOk()
{
	// Check values if valid
	// Check if username is not empty
	if (m_edtUsername.GetWindowTextLengthW() == 0) {
		AfxMessageBox(L"Username required.");
		m_edtUsername.SetFocus();
		return;
	}
	m_edtUsername.GetWindowTextW(m_strUsername);

	// Check password
	CString currentPswd, newPswd, confirmPswd;
	m_edtCurrentPassword.GetWindowTextW(currentPswd);
	m_edtNewPassword.GetWindowTextW(newPswd);
	m_edtConfirm.GetWindowTextW(confirmPswd);
	// Check if current password is correct
	if (currentPswd != m_strPassword) {
		AfxMessageBox(L"Current Password not correct." + currentPswd + L":" + m_strPassword);
		m_edtCurrentPassword.SetFocus();
		return;
	}
	// Check if current password and confirm password is same
	if (newPswd != confirmPswd) {
		AfxMessageBox(L"Confirm new password not correct.");
		m_edtConfirm.SetFocus();
		return;
	}
	m_strNewPassword = newPswd;

	// Check if hostname is not empty
	if (m_edtHostname.GetWindowTextLengthW() == 0) {
		AfxMessageBox(L"Hostname required.");
		m_edtHostname.SetFocus();
		return;
	}
	m_edtHostname.GetWindowTextW(m_strHostnames);

	// Check if Zipcode valid
	if (!CheckZipcodes()) {
		AfxMessageBox(L"Input ; after a zipcode");
		m_edtZipcode.SetFocus();
		return;
	}
	m_edtZipcode.GetWindowTextW(m_strZipcode);

	if (m_bPrintPreview) {
		m_strPrinter = L"";
	}

	CDialog::OnOK();
}


BOOL SettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set values of controls(Username, Current password - Fake, Hostname)
	m_edtUsername.SetWindowTextW(m_strUsername);
	m_edtCurrentPassword.SetWindowTextW(L"12345abcde^&*()");
	m_edtHostname.SetWindowTextW(m_strHostnames);
	// Set values of webservice name combo box
	m_strWebserviceName[ADDRESSES_URI] = L"Address";
	m_strWebserviceName[CARRIERS_URI] = L"Carrier";
	m_strWebserviceName[COUNTRIES_URI] = L"Country";
	m_strWebserviceName[CUSTOMERS_URI] = L"Customer";
	m_strWebserviceName[ORDERS_URI] = L"Order";
	m_strWebserviceName[SHOPS_URI] = L"Shop";
	m_strWebserviceName[INVOICES_URI] = L"Order Invoice";
	for (int i = 0; i < WEBSERVICE_COUNT; i++) {
		m_cmbWebserviceId.AddString(m_strWebserviceName[i]);
	}
	if (m_cmbWebserviceId.GetCount() > 0) {
		m_cmbWebserviceId.SetCurSel(0);
		m_edtWebserviceId.SetWindowTextW(m_strWebserviceId[0]);
		m_nPrevIndex = 0;
	}
	// Set values of controls(DPD ID, Location, Zipcode, PrintPreview, Printer Name)
	m_edtDpdId.SetWindowTextW(m_strDpdId);
	m_edtLocation.SetWindowTextW(m_strLocation);
	m_edtZipcode.SetWindowTextW(m_strZipcode);
	if (!m_bPrintPreview) {
		m_btnPrinter.SetWindowTextW(m_strPrinter);
	}
	m_btnPrinter.EnableWindow(!m_bPrintPreview);
	m_chkPrintPreview.SetCheck(m_bPrintPreview ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SettingsDlg::OnSelchangeComboWebserviceid()
{
	CString webserviceId;
	// Change webservice ID when combo box selection changed
	m_edtWebserviceId.GetWindowTextW(webserviceId);
	if (webserviceId != m_strWebserviceId[m_nPrevIndex]) {
		if (IDYES == AfxMessageBox(L"Do you want change Webservice ID of " + m_strWebserviceName[m_nPrevIndex], MB_YESNO)) {
			m_strWebserviceId[m_nPrevIndex] = webserviceId;
		}
	}
	int curIndex = m_cmbWebserviceId.GetCurSel();
	m_edtWebserviceId.SetWindowTextW(m_strWebserviceId[curIndex]);
	m_nPrevIndex = curIndex;
}


void SettingsDlg::OnKillfocusEditLocation()
{
	// Prompt if Location is empty, it means current working directory
	if (m_edtLocation.GetWindowTextLengthW() == 0) {
		AfxMessageBox(L"Location set as current directory.");
	}
	else {
		m_edtLocation.GetWindowTextW(m_strLocation);
	}
}

bool SettingsDlg::CheckZipcodes() {
	bool stringValid = true;
	CString zipCode;
	int semiColonIndex = -1;
	m_edtZipcode.GetWindowTextW(zipCode);
	// Check if zipcode array format is like: 'NN..N;NN..N;'
	for (int i = 0; i < zipCode.GetLength(); i++) {
		if (zipCode[i] == _T(';')) {
			if (i - semiColonIndex > 1) {
				semiColonIndex = i;
			}
			else {
				stringValid = false;
				break;
			}
		}
	}
	if (zipCode.GetLength() > 0) {
		if (zipCode[zipCode.GetLength() - 1] != _T(';')) {
			stringValid = false;
		}
	}
	return stringValid;
}

void SettingsDlg::OnClickedCheckPrintpreview()
{
	// Enable/Disable Printer selection button with status of Print Preview
	m_bPrintPreview = (m_chkPrintPreview.GetCheck() == BST_CHECKED);
	m_btnPrinter.EnableWindow(!m_bPrintPreview);
}

void SettingsDlg::OnClickedButtonPrinter()
{
	// Show Printer settings dialog
	CPrintDialog printDlg(true);
	if (IDOK == printDlg.DoModal()) {
		// Get selected printer name
		m_btnPrinter.SetWindowTextW(printDlg.GetDeviceName());
		m_strPrinter = printDlg.GetDeviceName();
	}
}

void SettingsDlg::SetValues(CString strUsername, CString strPassword, CString strHostnames,
	CString strWebserviceId[], CString strDpdId, CString strLocation, CString strZipcode,
	CString strPrinter, bool bPrintPreview) {
	m_strUsername = strUsername;
	m_strPassword = strPassword;
	m_strHostnames = strHostnames;
	for (int i = 0; i < WEBSERVICE_COUNT; i++) {
		m_strWebserviceId[i] = strWebserviceId[i];
	}
	m_strDpdId = strDpdId;
	m_strLocation = strLocation;
	m_strZipcode = strZipcode;
	m_strPrinter = strPrinter;
	m_bPrintPreview = bPrintPreview;
}
