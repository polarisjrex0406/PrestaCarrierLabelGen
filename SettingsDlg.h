#pragma once
#include "afxdialogex.h"
#include "winspool.h"

#define WEBSERVICE_COUNT 7
#define ADDRESSES_URI 0
#define CARRIERS_URI 1
#define COUNTRIES_URI 2
#define CUSTOMERS_URI 3
#define ORDERS_URI 4
#define SHOPS_URI 5
#define INVOICES_URI 6

// SettingsDlg dialog

class SettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(SettingsDlg)
// Constructor & destructor
public:
	SettingsDlg(CWnd* pParent = nullptr);   // Standard constructor
	virtual ~SettingsDlg();

// Dialog data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTINGS_DIALOG };
#endif

// Generated message map functions 
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnSelchangeComboWebserviceid();
	afx_msg void OnKillfocusEditLocation();
	afx_msg void OnClickedCheckPrintpreview();
	afx_msg void OnClickedButtonPrinter();
	DECLARE_MESSAGE_MAP()

// Generated control variables
private:
	CButton m_chkPrintPreview;	// Checkbox for print preview or direct print
	CComboBox m_cmbWebserviceId;	// List for API endpoints - web service IDs
	CEdit m_edtDpdId;	// Edit for DPD ID
	CEdit m_edtHostname;	// Edit for web service host (base url)
	CEdit m_edtLocation;	// Edit for location to move csv of DPD
	CEdit m_edtUsername;	// Edit for username to login web service
	CEdit m_edtWebserviceId;	// Edit for uri of selected web service
	CEdit m_edtZipcode;	// Edit for isolated zipcodes
	CEdit m_edtNewPassword;	// Edit for new password
	CEdit m_edtConfirm;	// Edit for password confirm
	CEdit m_edtCurrentPassword;	// Edit for current password
	CButton m_btnPrinter;	// Button for browsing available printers

// Implementation
public:
	// Set all data
	void SetValues(CString strUsername, CString strPassword, CString strHostnames,
		CString strWebserviceId[], CString strDpdId, CString strLocation,
		CString strZipcode, CString strPrinter, bool bPrintPreview);
	// Get username
	CString GetUsername() { return m_strUsername; }
	// Get password
	CString GetPassword() { return m_strNewPassword; }
	// Get hostname
	CString GetHostname() { return m_strHostnames; }
	// Get webservice ID
	CString GetWebserviceId(int nWebservice) { return m_strWebserviceId[nWebservice]; }
	// Get DPD ID
	CString GetDpdId() { return m_strDpdId; }
	// Get csv location
	CString GetLocation() { return m_strLocation; }
	// Get zipcode
	CString GetZipcode() { return m_strZipcode; }
	// Get printer
	CString GetPrinterName() { return m_strPrinter; }
	// Get print preview
	bool GetPrintPreview() { return m_bPrintPreview; }

	// Check format of zipcode array
	bool CheckZipcodes();

// Variables for store data to get
private:
	CString m_strWebserviceName[WEBSERVICE_COUNT];	// Array of uri to web service APIs

	CString m_strUsername;	// Username to login web service
	CString m_strPassword;	// Current password to login web service
	CString m_strHostnames;	// Hostname of web service (base url)
	CString m_strWebserviceId[WEBSERVICE_COUNT];	// String to show webservice (e.g. Customer)
	CString m_strDpdId;	// ID used for DPD
	CString m_strLocation;	// Location to move DPD csv file
	CString m_strZipcode;	// Zipcode for isolated areas
	CString m_strPrinter;	// Name of printer to print label
	bool m_bPrintPreview;	// Determines if print preview or direct print
	CString m_strNewPassword;	// New password to login web service (temporary until OK)

	int m_nPrevIndex;	// Index of previous web service in combo box
};
