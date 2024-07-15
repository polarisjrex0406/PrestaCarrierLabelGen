
// PrestaCarrierLabelGenDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "PrestaCarrierLabelGen.h"
#include "PrestaCarrierLabelGenDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPrestaCarrierLabelGenDlg dialog

BEGIN_DHTML_EVENT_MAP(CPrestaCarrierLabelGenDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()


CPrestaCarrierLabelGenDlg::CPrestaCarrierLabelGenDlg(CWnd* pParent /*=nullptr*/)
	: CDHtmlDialog(IDD_PRESTACARRIERLABELGEN_DIALOG, IDR_HTML_PRESTACARRIERLABELGEN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPrestaCarrierLabelGenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PDF_VIEWER, m_pdfViewer);
}

BEGIN_MESSAGE_MAP(CPrestaCarrierLabelGenDlg, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_BUTTON_GENERATE, &CPrestaCarrierLabelGenDlg::OnClickedButtonGenerate)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CPrestaCarrierLabelGenDlg::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_PREV, &CPrestaCarrierLabelGenDlg::OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CPrestaCarrierLabelGenDlg::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_SETTINGS, &CPrestaCarrierLabelGenDlg::OnBnClickedButtonSettings)
END_MESSAGE_MAP()


// CPrestaCarrierLabelGenDlg message handlers

BOOL CPrestaCarrierLabelGenDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	LoadSettings();
	m_strCsvSeparator = L";";

	// Font and Align Settings for Dialog Controls
	CFont font;
	font.CreatePointFont(160, L"Arial");
	GetDlgItem(IDC_EDIT_CARRIER)->SetFont(&font);
	GetDlgItem(IDC_EDIT_CUSTOMERNAME)->SetFont(&font);
	GetDlgItem(IDC_EDIT_ORDERID)->SetFont(&font);
	GetDlgItem(IDC_EDIT_REFERENCE)->SetFont(&font);
	GetDlgItem(IDC_EDIT_SHOPNAME)->SetFont(&font);
	GetDlgItem(IDC_BUTTON_GENERATE)->SetFont(&font);
	GetDlgItem(IDC_BUTTON_NEXT)->SetFont(&font);
	GetDlgItem(IDC_BUTTON_PREV)->SetFont(&font);
	GetDlgItem(IDC_BUTTON_REFRESH)->SetFont(&font);
	GetDlgItem(IDC_BUTTON_SETTINGS)->SetFont(&font);
	GetDlgItem(IDC_STATIC_TIPS_LOADING)->SetFont(&font);
	GetDlgItem(IDC_COMBO_DPD_TYPE)->SetFont(&font);

	CComboBox* pCmbBox = (CComboBox*)GetDlgItem(IDC_COMBO_DPD_TYPE);
	pCmbBox->AddString(_T("Home"));
	pCmbBox->AddString(_T("Mailbox"));
	pCmbBox->AddString(_T("Classic"));
	pCmbBox->SetCurSel(0);

	CString authInput = m_strUsername + L":" + m_strPassword;
	std::string plainText = CT2A(authInput.GetString());
	std::string b64Enc = Xbase64_endcode(plainText);
	CString authString = CA2T(b64Enc.c_str());

	m_pServiceComm = new PrestaServiceComm(authString);
	if (nullptr == m_pServiceComm) {
		AfxMessageBox(L"Error in Presta Web Service Communication");
		return FALSE;
	}
	m_pServiceComm->SetEndPoints(m_strEndPoints);
	m_pServiceComm->SetHostnames(m_strHostnames);

	m_bMouseWheelEnable = false;
	GetDlgItem(IDC_STATIC_TIPS_LOADING)->ShowWindow(SW_HIDE);
	CallThread(GET_RECENT_CALL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPrestaCarrierLabelGenDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDHtmlDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPrestaCarrierLabelGenDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDHtmlDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPrestaCarrierLabelGenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HRESULT CPrestaCarrierLabelGenDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT CPrestaCarrierLabelGenDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}

void CPrestaCarrierLabelGenDlg::ShowMainScreenData()
{
	// TODO: Add your implementation code here.
	Json::Value jsonRoot = m_pServiceComm->CurrentOrderInformation();
	if (jsonRoot.empty()) {
		return;
	}

	if (!jsonRoot["carriers"].empty() && jsonRoot["carriers"].isValidIndex(0) && !jsonRoot["carriers"][0].empty() && !jsonRoot["carriers"][0]["name"].empty()) {
		SetDlgItemText(IDC_EDIT_CARRIER, PrestaServiceComm::ParseJsonTextVal2CString(jsonRoot["carriers"][0]["name"]));
	}
	if (!jsonRoot["addresses"].empty() && jsonRoot["addresses"].isValidIndex(0) && !jsonRoot["addresses"][0].empty() && !jsonRoot["addresses"][0]["lastname"].empty()) {
		SetDlgItemText(IDC_EDIT_CUSTOMERNAME, PrestaServiceComm::ParseJsonTextVal2CString(jsonRoot["addresses"][0]["lastname"]));
	}
	if (!jsonRoot["orders"].empty() && jsonRoot["orders"].isValidIndex(0) && !jsonRoot["orders"][0].empty()) {
		if (!jsonRoot["orders"][0]["id"].empty()) {
			SetDlgItemText(IDC_EDIT_ORDERID, PrestaServiceComm::ParseJsonTextVal2CString(jsonRoot["orders"][0]["id"]));
		}
		if (!jsonRoot["orders"][0]["reference"].empty()) {
			SetDlgItemText(IDC_EDIT_REFERENCE, PrestaServiceComm::ParseJsonTextVal2CString(jsonRoot["orders"][0]["reference"]));
		}
	}
	if (!jsonRoot["shops"].empty() && jsonRoot["shops"].isValidIndex(0) && !jsonRoot["shops"][0].empty() && !jsonRoot["shops"][0]["name"].empty()) {
		SetDlgItemText(IDC_EDIT_SHOPNAME, PrestaServiceComm::ParseJsonTextVal2CString(jsonRoot["shops"][0]["name"]));
	}
}


BOOL CPrestaCarrierLabelGenDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	// Call your function here based on the mouse scroll direction
	if (m_bMouseWheelEnable) {
		if (zDelta < 0)
		{
			// Call function when scrolling up
			CallThread(GET_NEXT_CALL);
		}
		else if (zDelta > 0)
		{
			// Call function when scrolling down
			CallThread(GET_PREV_CALL);
		}
	}
	return CDHtmlDialog::OnMouseWheel(nFlags, zDelta, pt);
}

UINT CPrestaCarrierLabelGenDlg::ThreadFunction(LPVOID pParam) {
	CPrestaCarrierLabelGenDlg* pDlg = static_cast<CPrestaCarrierLabelGenDlg*>(pParam);
	bool status = false;
    if (pDlg != nullptr)
    {
		switch (pDlg->m_usCallType) {
		case GET_RECENT_CALL:
			status = pDlg->m_pServiceComm->GetRecent();
			break;
		case GET_NEXT_CALL:
			status = pDlg->m_pServiceComm->GetNext();
			break;
		case GET_PREV_CALL:
			status = pDlg->m_pServiceComm->GetPrev();
			break;
		case GET_DPD_FULL_CALL:
		case GET_POSTNLE_FULL_CALL:
			status = pDlg->m_pServiceComm->GetFullData();
			break;
		case UPDATE_DPD_SHIPPED_CALL:
		case UPDATE_POSTNLE_SHIPPED_CALL:
			status = pDlg->m_pServiceComm->UpdateAsShipped();
			break;
		case HIDE_ORDER_CALL:
			status = pDlg->m_pServiceComm->HideOrder();
			break;
		case GET_FIRST_CALL:
			status = pDlg->m_pServiceComm->GetFirst();
			break;
		case GET_LAST_CALL:
			status = pDlg->m_pServiceComm->GetLast();
			break;
		default:
			break;
		}
		pDlg->ThreadFinished(status);
    }
    return 0;
}

void CPrestaCarrierLabelGenDlg::ThreadFinished(bool bStatus) {
	ShowTipsWhileLoading(false);
	WriteLogs(m_pServiceComm->LastErrorMessage());
	if (bStatus) {
		switch (m_usCallType) {
		case GET_RECENT_CALL:
		case GET_NEXT_CALL:
		case GET_PREV_CALL:
		case UPDATE_DPD_SHIPPED_CALL:
		case UPDATE_POSTNLE_SHIPPED_CALL:
		case HIDE_ORDER_CALL:
		case GET_FIRST_CALL:
		case GET_LAST_CALL:
			ShowMainScreenData();
			break;
		case GET_DPD_FULL_CALL:
			PreProcessDPD();
			break;
		case GET_POSTNLE_FULL_CALL:
			GeneratePostNL_E_Label();
			break;
		default:
			break;
		}
	}
	else {
//		AfxMessageBox(m_pServiceComm->LastErrorMessage());
	}
}

void CPrestaCarrierLabelGenDlg::CallThread(unsigned short usCallType) {
	m_usCallType = usCallType;

	CString logText[10];
	logText[GET_RECENT_CALL] = L"GET_RECENT_CALL";
	logText[GET_NEXT_CALL] = L"GET_NEXT_CALL";
	logText[GET_PREV_CALL] = L"GET_PREV_CALL";
	logText[GET_DPD_FULL_CALL] = L"GET_DPD_FULL_CALL";
	logText[GET_POSTNLE_FULL_CALL] = L"GET_POSTNLE_FULL_CALL";
	logText[UPDATE_DPD_SHIPPED_CALL] = L"UPDATE_DPD_SHIPPED_CALL";
	logText[UPDATE_POSTNLE_SHIPPED_CALL] = L"UPDATE_POSTNLE_SHIPPED_CALL";
	logText[HIDE_ORDER_CALL] = L"HIDE_ORDER_CALL";
	logText[GET_FIRST_CALL] = L"GET_FIRST_CALL";
	logText[GET_LAST_CALL] = L"GET_LAST_CALL";
	WriteLogs(logText[m_usCallType]);
	m_pServiceComm->ClearErrorMessage();

	ShowTipsWhileLoading(true);
	CWinThread* pThread = AfxBeginThread(CPrestaCarrierLabelGenDlg::ThreadFunction, this);
}

PrestaServiceComm* CPrestaCarrierLabelGenDlg::ServiceComm() {
	return m_pServiceComm;
}

void CPrestaCarrierLabelGenDlg::ShowTipsWhileLoading(bool bShow) {
	int cmdShow = bShow ? SW_SHOW : SW_HIDE;
	GetDlgItem(IDC_EDIT_CARRIER)->EnableWindow(!bShow);
	GetDlgItem(IDC_EDIT_CUSTOMERNAME)->EnableWindow(!bShow);
	GetDlgItem(IDC_EDIT_ORDERID)->EnableWindow(!bShow);
	GetDlgItem(IDC_EDIT_REFERENCE)->EnableWindow(!bShow);
	GetDlgItem(IDC_EDIT_SHOPNAME)->EnableWindow(!bShow);
	GetDlgItem(IDC_BUTTON_GENERATE)->EnableWindow(!bShow);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(!bShow);
	GetDlgItem(IDC_BUTTON_PREV)->EnableWindow(!bShow);
	GetDlgItem(IDC_BUTTON_REFRESH)->EnableWindow(!bShow);
	GetDlgItem(IDC_BUTTON_SETTINGS)->EnableWindow(!bShow);
	GetDlgItem(IDC_STATIC_TIPS_LOADING)->ShowWindow(cmdShow);
	m_pdfViewer.EnableWindow(!bShow);
	m_bMouseWheelEnable = !bShow;
	Json::Value jsonVal = m_pServiceComm->CurrentOrderInformation();
	CString carrierType = m_pServiceComm->ParseJsonTextVal2CString(jsonVal["carriers"][0]["name"]);
	GetDlgItem(IDC_COMBO_DPD_TYPE)->EnableWindow(!bShow && (carrierType == L"DPD" || carrierType == L"Verzending" || carrierType == L"Pakketdienst" || carrierType == L"Service Point Delivery"));
}

void CPrestaCarrierLabelGenDlg::OnClickedButtonGenerate()
{
	// TODO: Add your control notification handler code here
	Json::Value jsonVal = m_pServiceComm->CurrentOrderInformation();
	CString carrierType = m_pServiceComm->ParseJsonTextVal2CString(jsonVal["carriers"][0]["name"]);
	if (carrierType == L"PostNL") {
		AfxMessageBox(L"This order is a PostNL order.");
		CallThread(HIDE_ORDER_CALL);
	}
	else if (-1 != carrierType.Find(L"Showroom")) {
		AfxMessageBox(L"This order is a Showroom order.");
		CallThread(HIDE_ORDER_CALL);
	}
	else if (carrierType == L"PostNL E") {
		CallThread(GET_POSTNLE_FULL_CALL);
	}
	else if (carrierType == L"DPD" || carrierType == L"Verzending" || carrierType == L"Pakketdienst" || carrierType == L"Service Point Delivery") {
		CallThread(GET_DPD_FULL_CALL);
	}
	else {
		AfxMessageBox(L"This order is " + carrierType + ".");
		CallThread(HIDE_ORDER_CALL);
	}
}

void CPrestaCarrierLabelGenDlg::PreProcessDPD() {
	WriteLogs(L"Called PreProcessDPD()\n");
	// Check zipcode
	Json::Value jsonArray = m_pServiceComm->CurrentOrderInformation();
	for (int i = 0; i < m_lstIsoZipcode.GetSize(); i++) {
		POSITION pos = m_lstIsoZipcode.FindIndex(i);
		CString isoZipcode = m_lstIsoZipcode.GetAt(pos);
		if (m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["postcode"]).Left(isoZipcode.GetLength()) == isoZipcode) {
			AfxMessageBox(L"This order is for isolated areas.");
			return;
		}
	}
	// Get House number from address
	CString streetName, houseNum, address[2];
	bool containHouseNum = false;
	address[0] = m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["address1"]);
	address[1] = m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["address2"]);
	// address1 has address until street, address2 has house number
	if (_ttoi(address[1]) > 0 && !address[0].IsEmpty()) {
		streetName = address[0];
		houseNum = address[1];
		containHouseNum = true;
	}
	// address1 or address2 has full address
	for (int i = 0; !containHouseNum && i < 2; i++) {
		int j, firstNumIndex = -1, lastValidIndex = -1, spaceIndex = -1;
		CString addrTemp = _T("");
		// Remove all spaces from right
		// e.g.: "abcd 103   " => "abcd 103"
		for (j = address[i].GetLength() - 1; j > 1; j--) {
			if (address[i][j] != _T(' ')) {
				addrTemp = address[i].Left(j + 1);
				break;
			}
		}
		if (addrTemp == _T("")) {
			break;
		}
		// Find space between street and house number
		spaceIndex = addrTemp.ReverseFind(_T(' '));
		if (spaceIndex > 0 && spaceIndex < addrTemp.GetLength() - 1) {
			firstNumIndex = spaceIndex + 1;
			lastValidIndex = addrTemp.GetLength() - 1;
			if (lastValidIndex > -1 && firstNumIndex > -1) {
				houseNum = addrTemp.Mid(firstNumIndex, lastValidIndex - firstNumIndex + 1);
				if (!houseNum.IsEmpty() && _ttoi(houseNum) > 0) {
					containHouseNum = true;
					streetName = addrTemp.Left(firstNumIndex - 1);
					break;
				}
				else {
					houseNum = _T("");
				}
			}
		}
	}
	// Check if NO house number
	if (!containHouseNum) {
		WriteLogs(L"Error: Cannot find the house number.");
		AfxMessageBox(L"Error: Cannot find the house number.");
		return;
	}
	// Get Country Name from Array or String
	CString countryName;
	if (jsonArray["countries"][0]["name"].isArray()) {
		countryName = m_pServiceComm->ParseJsonTextVal2CString(jsonArray["countries"][0]["name"][0]["value"]);
	}
	else {
		countryName = m_pServiceComm->ParseJsonTextVal2CString(jsonArray["countries"][0]["name"]);
	}
	// Get Country Code & Communication Type
	CString countryCode, taal, isoCode;
	isoCode = m_pServiceComm->ParseJsonTextVal2CString(jsonArray["countries"][0]["iso_code"]);
	// Set Country Code Except BE, DE, NL
	countryCode = isoCode;
	// Set Communication Type:
	// (NL, BE = NL, FR	= FR, DE, AT = DE, all other is EN
	if (isoCode == L"BE" || isoCode == L"NL") {
		taal = L"NL";
	}
	else if (isoCode == L"DE" || isoCode == L"AT") {
		taal = L"DE";
	}
	else if (isoCode == L"FR") {
		taal = L"FR";
	}
	else {
		isoCode = "EN";
	}
	// Check if ServicePoint
	CString Pickup_id = L"";
	if (m_pServiceComm->ParseJsonTextVal2CString(jsonArray["carriers"][0]["name"]) == L"Service Point Delivery") {
		CPrestaInputDialog inputDlg;
		if (IDOK == inputDlg.DoModal()) {
			Pickup_id = inputDlg.GetContent();
		}
	}
	// Generate CSV file
	CString fullPath = m_strCsvPath;
	fullPath += L"dpd-import";
	fullPath += m_strDpdId;
	fullPath += L".";
	// Date & Time
	// Get the current local system time
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	// Create a CTime object from the SYSTEMTIME structure
	CTime currentTime(sysTime);
	fullPath += currentTime.Format(_T("%Y%m%d%H%M%S"));
	fullPath += L".csv";

	CStdioFile csvFile;
	if (csvFile.Open(fullPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
		WriteLogs(L"Start Writing CSV.");
		// Add Headers
		csvFile.WriteString(L"Product;Service;Gewicht;Naam;Adres;Huisnummer;Postcode;Plaats;Land;Company;Telefoon;Referentie;Email2;CommunicatieType;Email3;904;Taal");
		// Check if Mailbox and add Parcel Volume
		CComboBox* pCmbBox = (CComboBox*)GetDlgItem(IDC_COMBO_DPD_TYPE);
		CString typeDpdShipment;
		pCmbBox->GetLBText(pCmbBox->GetCurSel(), typeDpdShipment);
		if (typeDpdShipment == L"Mailbox") {
			csvFile.WriteString(L";ParcelVolume");
		}
		// Add if Service Point
		if (Pickup_id != L"") {
			csvFile.WriteString(L";Pickup_id");
		}
        // Add a newline at the end of the line
		csvFile.WriteString(L"\n");

		// Add Contents
		// Product
		// PSD if Service Point
		if (Pickup_id != L"") {
			csvFile.WriteString(L"PSD");
		}
		else {
			if (typeDpdShipment == L"Classic") {
				csvFile.WriteString(L"B2B");
			}
			else if (typeDpdShipment == L"Home") {
				csvFile.WriteString(L"B2C");
			}
			else if (typeDpdShipment == L"Mailbox") {
				csvFile.WriteString(L"PL");
			}
		}

		csvFile.WriteString(m_strCsvSeparator);
		// Service
		// Check if today is Friday(==6) and country is Belgium or Netherlands
		// Not for service points
		if (Pickup_id == L"") {
			if (countryName == L"Netherlands" || countryName == L"Belgium") {
				if ((currentTime.GetDayOfWeek() == 6) && (typeDpdShipment == L"Home")) {
					csvFile.WriteString(L"6");
				}
			}
		}
		csvFile.WriteString(m_strCsvSeparator);
		// Gewicht
		if (typeDpdShipment == L"Mailbox") {
			csvFile.WriteString(L"100");
		}
		else {
			csvFile.WriteString(L"300");
		}
		csvFile.WriteString(m_strCsvSeparator);
		// Naam
		csvFile.WriteString(m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["firstname"]));
		csvFile.WriteString(L" ");
		csvFile.WriteString(m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["lastname"]));
		csvFile.WriteString(m_strCsvSeparator);
		// Adres
		csvFile.WriteString(streetName);
		csvFile.WriteString(m_strCsvSeparator);
		// Huisnummer
		csvFile.WriteString(houseNum);
		csvFile.WriteString(m_strCsvSeparator);
		// Postcode
		csvFile.WriteString(m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["postcode"]));
		csvFile.WriteString(m_strCsvSeparator);
		// Plaats
		csvFile.WriteString(m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["city"]));
		csvFile.WriteString(m_strCsvSeparator);
		// Land
		csvFile.WriteString(countryCode);
		csvFile.WriteString(m_strCsvSeparator);
		// Company
		csvFile.WriteString(m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["company"]));
		csvFile.WriteString(m_strCsvSeparator);
		// Telefoon
		if (!m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["phone"]).IsEmpty()) {
			csvFile.WriteString(m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["phone"]));
		}
		else {
			csvFile.WriteString(m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["phone_mobile"]));
		}
		csvFile.WriteString(m_strCsvSeparator);
		// Referetie (company-num-ref)
		csvFile.WriteString(m_pServiceComm->ParseJsonTextVal2CString(jsonArray["shops"][0]["name"]));
		csvFile.WriteString(L"-");
		csvFile.WriteString(m_pServiceComm->ParseJsonTextVal2CString(jsonArray["orders"][0]["id"]));
		csvFile.WriteString(L"-");
		csvFile.WriteString(m_pServiceComm->ParseJsonTextVal2CString(jsonArray["orders"][0]["reference"]));
		csvFile.WriteString(m_strCsvSeparator);
		// Email2
		csvFile.WriteString(m_pServiceComm->ParseJsonTextVal2CString(jsonArray["customers"][0]["email"]));
		csvFile.WriteString(m_strCsvSeparator);
		// CommunicatieType
		csvFile.WriteString(L"E");
		csvFile.WriteString(m_strCsvSeparator);
		// Email3
		csvFile.WriteString(m_pServiceComm->ParseJsonTextVal2CString(jsonArray["customers"][0]["email"]));
		csvFile.WriteString(m_strCsvSeparator);
		// 904
		csvFile.WriteString(L"904");
		csvFile.WriteString(m_strCsvSeparator);
		// Taal
		csvFile.WriteString(taal);
		// Parcel Volume
		if (typeDpdShipment == L"Mailbox") {
			csvFile.WriteString(m_strCsvSeparator);
			csvFile.WriteString(L"26/3/38");
		}
		// Pickup id
		if (Pickup_id != L"") {
			csvFile.WriteString(m_strCsvSeparator);
			csvFile.WriteString(Pickup_id);
		}
		// End line
		csvFile.WriteString(L"\n");
        // Close the file
		csvFile.Close();
		// Update DPD order as shipped
		CallThread(UPDATE_DPD_SHIPPED_CALL);
	}
	else {
		// Handle the case where the file could not be opened
		WriteLogs(L"Error: Opening CSV file for writing.");
		AfxMessageBox(_T("Error: Opening CSV file for writing."));
	}
	// Set default to DPD service "Home" 
	CComboBox* pCmbBox = (CComboBox*)GetDlgItem(IDC_COMBO_DPD_TYPE);
	pCmbBox->SetCurSel(0);
}

void CPrestaCarrierLabelGenDlg::GenerateDPDLabel() {
}

void CPrestaCarrierLabelGenDlg::GeneratePostNL_E_Label() {
	WriteLogs(L"Called GeneratePostNL_E_Label()\n");
	// Generate PostNL-E Label
	TCHAR tmpPath[1024];
	GetTempPath(1024, tmpPath);
	CString filePath(tmpPath);
	filePath += L"PostNL-E Label.pdf";
	WriteLogs(filePath);
	Json::Value jsonArray = m_pServiceComm->CurrentOrderInformation();
	CString bufferText;
	HPDF_Doc m_hPdfDoc = HPDF_New(NULL, NULL);

	if (!m_hPdfDoc) {
		WriteLogs(_T("Error initializing libHaru."));
		AfxMessageBox(_T("Error initializing libHaru."));
		return;
	}
	// Add a Page
	HPDF_Page page = HPDF_AddPage(m_hPdfDoc);
	if (!page) {
		WriteLogs(_T("Error: adding a new page."));
		AfxMessageBox(_T("Error: adding a new page."));
		HPDF_Free(m_hPdfDoc);
		return;
	}

	// Set Page Size
	// Convert millimeters to points
	float widthInMilli = 135.0;
	float heightInMilli = 88.0;
	float widthInPoints = Milli2Point(widthInMilli);
	float heightInPoints = Milli2Point(heightInMilli);
	HPDF_Page_SetWidth(page, widthInPoints);
	HPDF_Page_SetHeight(page, heightInPoints);

	// Create a transformation matrix
	// Set the coordinate system transformation
	HPDF_Page_GSave(page);
	HPDF_Page_Concat(page, (HPDF_REAL)(72.0 / 25.4), (HPDF_REAL)0, 
		(HPDF_REAL)0, (HPDF_REAL)(72.0 / 25.4), (HPDF_REAL)0, (HPDF_REAL)0);

	// Font Settings
	//HPDF_Font font = HPDF_GetFont(m_hPdfDoc, "Helvetica", NULL);
	//HPDF_Page_SetFontAndSize(page, font, (HPDF_REAL)3.7);
	// Font Settings for Accent letters
	const char* font_name = HPDF_LoadType1FontFromFile(m_hPdfDoc, "type1\\a010013l.afm", "type1\\a010013l.pfb");
	HPDF_Font font2 = HPDF_GetFont(m_hPdfDoc, font_name, "WinAnsiEncoding");
	HPDF_Page_SetFontAndSize(page, font2, (HPDF_REAL)3.7);

	// Big Logo - Geodox
	HPDF_Image imgGeodox;
	imgGeodox = HPDF_LoadJpegImageFromFile(m_hPdfDoc, "logo.jpeg");
	if (NULL != imgGeodox) {
		HPDF_Page_DrawImage(page, imgGeodox, (HPDF_REAL)30.6, (HPDF_REAL)YPOS(14.9), (HPDF_REAL)28, (HPDF_REAL)12);
	}

	// Small Logo
	HPDF_Image imgSmall;
	imgSmall = HPDF_LoadJpegImageFromFile(m_hPdfDoc, "logo_2.jpg");
	if (NULL != imgSmall) {
		HPDF_Page_DrawImage(page, imgSmall, (HPDF_REAL)102.7, (HPDF_REAL)YPOS(19.9), (HPDF_REAL)15.2, (HPDF_REAL)15.2);
	}

	HPDF_Page_SetLineWidth(page, (HPDF_REAL)(0.3 / 72 * 25.4));
	HPDF_Page_SetRGBStroke(page, 0, 0, 0);
	// Draw 1st rectangle
	/*
	HPDF_Page_Rectangle(page, 1.8, YPOS(25.8), 50.8, 22.9);
	HPDF_Page_Stroke(page);
	// Draw rectangle around imgGeodox
	HPDF_Page_Rectangle(page, 24.6, YPOS(14.9), 28, 12);
	HPDF_Page_Stroke(page);
	*/
	// Draw Afzender:
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, (HPDF_REAL)1.8, (HPDF_REAL)YPOS(6.1));
	HPDF_Page_ShowText(page, "Afzender:");
	HPDF_Page_EndText(page);
	// de Huufkes 3
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, (HPDF_REAL)1.8, (HPDF_REAL)YPOS(9.4));
	HPDF_Page_ShowText(page, "de Huufkes 3");
	HPDF_Page_EndText(page);
	// 5674 TL Nuenen
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, (HPDF_REAL)1.8, (HPDF_REAL)YPOS(13.1));
	HPDF_Page_ShowText(page, "5674 TL Nuenen");
	HPDF_Page_EndText(page);
	// Nederland
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, (HPDF_REAL)1.8, (HPDF_REAL)YPOS(16.7));
	HPDF_Page_ShowText(page, "Nederland");
	HPDF_Page_EndText(page);

	// Draw 2nd rectangle
	HPDF_Page_SetLineWidth(page, (HPDF_REAL)(0.7 / 72 * 25.4));
	HPDF_Page_Rectangle(page, (HPDF_REAL)22.9, (HPDF_REAL)YPOS(70), (HPDF_REAL)88.2, (HPDF_REAL)31.4);
	HPDF_Page_Stroke(page);
	// Company Firstname Lastname
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, (HPDF_REAL)26.1, (HPDF_REAL)YPOS(48.6 - 1.4));
	bufferText = L"";
	if (!jsonArray["addresses"].empty() && jsonArray["addresses"].isValidIndex(0) && !jsonArray["addresses"][0].empty()) {
		if (!jsonArray["addresses"][0]["company"].empty()) {
			bufferText += m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["company"]);
		}
		if (!jsonArray["addresses"][0]["firstname"].empty()) {
			bufferText += " ";
			bufferText += m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["firstname"]);
		}
		if (!jsonArray["addresses"][0]["lastname"].empty()) {
			bufferText += " ";
			bufferText += m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["lastname"]);
		}
	}

	WriteLogs(_T("Writing : ") + bufferText);
	HPDF_Page_ShowText(page, CStringA(bufferText));
	HPDF_Page_EndText(page);
	// address1 address2
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, (HPDF_REAL)26.1, (HPDF_REAL)YPOS(54.8 - 1.4));
	bufferText = L"";
	if (!jsonArray["addresses"].empty() && jsonArray["addresses"].isValidIndex(0) && !jsonArray["addresses"][0].empty()) {
		if (!jsonArray["addresses"][0]["address1"].empty()) {
			bufferText += m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["address1"]);
		}
		if (!jsonArray["addresses"][0]["address2"].empty()) {
			bufferText += " ";
			bufferText += m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["address2"]);
		}
	}
	WriteLogs(_T("Writing : ") + bufferText);
	HPDF_Page_ShowText(page, CStringA(bufferText));
	HPDF_Page_EndText(page);
	// postcode city
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, (HPDF_REAL)26.1, (HPDF_REAL)YPOS(60.6 - 1.4));
	bufferText = L"";
	if (!jsonArray["addresses"].empty() && jsonArray["addresses"].isValidIndex(0) && !jsonArray["addresses"][0].empty()) {
		if (!jsonArray["addresses"][0]["postcode"].empty()) {
			bufferText += m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["postcode"]);
		}
		if (!jsonArray["addresses"][0]["city"].empty()) {
			bufferText += " ";
			bufferText += m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["city"]);
		}
	}
	WriteLogs(_T("Writing : ") + bufferText);
	HPDF_Page_ShowText(page, CStringA(bufferText));
	HPDF_Page_EndText(page);
	// country state
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, (HPDF_REAL)26.1, (HPDF_REAL)YPOS(66.8 - 1.4));
	CString countryName;
	if (!jsonArray["countries"].empty() && jsonArray["countries"].isValidIndex(0) && !jsonArray["countries"][0].empty()) {
		if (!jsonArray["countries"][0]["name"].empty() && jsonArray["countries"][0]["name"].isValidIndex(0)) {
			if (!jsonArray["countries"][0]["name"][0].empty()) {
				if (!jsonArray["countries"][0]["name"][0]["value"].empty()) {
					bufferText = m_pServiceComm->ParseJsonTextVal2CString(jsonArray["countries"][0]["name"][0]["value"]).MakeUpper();
					if (!jsonArray["addresses"].empty() && jsonArray["addresses"].isValidIndex(0) && !jsonArray["addresses"][0]["state"].empty()) {
						if (!jsonArray["addresses"][0]["state"].empty()) {
							if (0 != m_pServiceComm->ParseJsonTextVal2CString(jsonArray["addresses"][0]["state"]).Compare(L"0")) {
								bufferText += " ";
								bufferText += "<state>";
							}
						}
					}
				}
			}
		}
	}
	WriteLogs(_T("Writing : ") + bufferText);
	HPDF_Page_ShowText(page, CStringA(bufferText));
	HPDF_Page_EndText(page);

	// Draw 3rd rectangle
	/*
	HPDF_Page_SetLineWidth(page, 0.3 / 72 * 25.4);
	HPDF_Page_SetRGBStroke(page, 0, 0, 0);
	HPDF_Page_Rectangle(page, 72.1, YPOS(88), 63.1, 6.0);
	HPDF_Page_Stroke(page);
	*/
	// id_order - reference
	HPDF_Page_BeginText(page);
//	HPDF_Page_MoveTextPos(page, 72.1, YPOS(86.2));
	// Draw the text within the rectangle
	bufferText = L"";
	if (!jsonArray["shops"].empty() && jsonArray["shops"].isValidIndex(0) && !jsonArray["shops"][0].empty()) {
		if (!jsonArray["shops"][0]["name"].empty()) {
			bufferText += m_pServiceComm->ParseJsonTextVal2CString(jsonArray["shops"][0]["name"]);
		}
		if (!jsonArray["orders"].empty() && jsonArray["orders"].isValidIndex(0) && !jsonArray["orders"][0].empty()) {
			if (!jsonArray["shops"][0]["name"].empty() && !jsonArray["orders"][0]["id"].empty()) {
				bufferText += "-";
			}
		}
	}
	if (!jsonArray["orders"].empty() && jsonArray["orders"].isValidIndex(0) && !jsonArray["orders"][0].empty()) {
		if (!jsonArray["orders"][0]["id"].empty()) {
			bufferText += m_pServiceComm->ParseJsonTextVal2CString(jsonArray["orders"][0]["id"]);
		}
		if (!jsonArray["orders"][0]["id"].empty() && !jsonArray["orders"][0]["reference"].empty()) {
			bufferText += "-";
		}
		if (!jsonArray["orders"][0]["reference"].empty()) {
			bufferText += m_pServiceComm->ParseJsonTextVal2CString(jsonArray["orders"][0]["reference"]);
		}
	}
	WriteLogs(_T("Writing : ") + bufferText);
	HPDF_Page_TextRect(page, (HPDF_REAL)72.1, (HPDF_REAL)(YPOS(86.2) + 4.6), (HPDF_REAL)134.8, (HPDF_REAL)YPOS(86.2), CStringA(bufferText), HPDF_TALIGN_RIGHT, NULL);
	HPDF_Page_EndText(page);

	HPDF_Page_GRestore(page);

	// Delete old PDF file before generating
	try
	{
		CFile::Remove(filePath);
	}
	catch (CFileException* pEx)
	{
		WriteLogs(_T("Old PDF file cannot be removed.\n"));
	}
	// Save PDF File
	if (HPDF_SaveToFile(m_hPdfDoc, CT2A(filePath)) != HPDF_OK) {
		AfxMessageBox(_T("Error saving the PDF file."));
		HPDF_Free(m_hPdfDoc);
		return;
	}

	HPDF_Free(m_hPdfDoc);

	// Print HPDF
	if (!m_pdfViewer.LoadFile(filePath)) {
		AfxMessageBox(_T("Error generating label."));
		return;
	}
	if (m_bPrintPreview) {
//		m_pdfViewer.ShowWindow(SW_SHOW);
		m_pdfViewer.printWithDialog();
	}
	else {
		// Set the device context for the viewer
		if (!SetDefaultPrinter(m_strPrinter)) {
			AfxMessageBox(L"Failed set printer as " + m_strPrinter);
		}
		// Print all pages
		m_pdfViewer.printAll();
	}
	// Update as 'shipped'
	CallThread(UPDATE_POSTNLE_SHIPPED_CALL);
}

float CPrestaCarrierLabelGenDlg::Milli2Point(float fMilli)
{
	// TODO: Add your implementation code here.
	// Conversion factor from millimeters to points
	const float millimetersToPoints = 72.0f / 25.4f;
	return fMilli * millimetersToPoints;
}

void CPrestaCarrierLabelGenDlg::OnBnClickedButtonNext()
{
	// TODO: Add your control notification handler code here
	CallThread(GET_LAST_CALL);
}


void CPrestaCarrierLabelGenDlg::OnBnClickedButtonPrev()
{
	// TODO: Add your control notification handler code here
	CallThread(GET_FIRST_CALL);
}


void CPrestaCarrierLabelGenDlg::OnBnClickedButtonRefresh()
{
	// TODO: Add your control notification handler code here
	CallThread(GET_RECENT_CALL);
}


bool CPrestaCarrierLabelGenDlg::SaveSettings()
{
	// TODO: Add your implementation code here.
	CString pathSettings = L"settings.dat";
	CStdioFile fileSettings;
	if (fileSettings.Open(pathSettings, CFile::modeCreate | CFile::modeWrite))
	{
		fileSettings.WriteString(_T("Username: "));
		fileSettings.WriteString(m_strUsername);
		fileSettings.WriteString(_T("\n"));

		fileSettings.WriteString(_T("Password: "));
		fileSettings.WriteString(m_strPassword);
		fileSettings.WriteString(_T("\n"));

		fileSettings.WriteString(_T("Hostname: "));
		fileSettings.WriteString(m_strHostnames);
		fileSettings.WriteString(_T("\n"));

		fileSettings.WriteString(_T("Web Service ID: "));
		for (int i = 0; i < ENDPOINT_COUNT; i++) {
			fileSettings.WriteString(m_strEndPoints[i]);
			fileSettings.WriteString(_T(";"));
		}
		fileSettings.WriteString(_T("\n"));

		fileSettings.WriteString(_T("DPD ID: "));
		fileSettings.WriteString(m_strDpdId);
		fileSettings.WriteString(_T("\n"));

		fileSettings.WriteString(_T("CSV Location: "));
		fileSettings.WriteString(m_strCsvPath);
		fileSettings.WriteString(_T("\n"));

		fileSettings.WriteString(_T("Isolated Zipcodes: "));
		for (int i = 0; i < m_lstIsoZipcode.GetCount(); i++) {
			POSITION pos = m_lstIsoZipcode.FindIndex(i);
			fileSettings.WriteString(m_lstIsoZipcode.GetAt(pos));
			fileSettings.WriteString(_T(";"));
		}
		fileSettings.WriteString(_T("\n"));

		fileSettings.WriteString(_T("Printer Name: "));
		fileSettings.WriteString(m_strPrinter);
		fileSettings.WriteString(_T("\n"));

		fileSettings.WriteString(_T("Print Preview: "));
		fileSettings.WriteString(m_bPrintPreview ? _T("T") : _T("F"));
		fileSettings.WriteString(_T("\n"));

		fileSettings.Close();

		return true;
	}
	else
	{
		// Handle the error if the file cannot be opened
		AfxMessageBox(_T("Error opening settings file for saving."), MB_ICONERROR);
		return false;
	}
}


bool CPrestaCarrierLabelGenDlg::LoadSettings()
{
	// TODO: Add your implementation code here.
	CString pathSettings = L"settings.dat";
	CStdioFile fileSettings;
	if (fileSettings.Open(pathSettings, CFile::modeRead))
	{
		CString line, data;
		while (fileSettings.ReadString(line))
		{
			// Extract data based on the format
			if (line.Find(_T("Username: ")) == 0)
			{
				m_strUsername = line.Mid((int)strlen("Username: "));
			}
			else if (line.Find(_T("Password: ")) == 0)
			{
				m_strPassword = line.Mid((int)strlen("Password: "));
			}
			else if (line.Find(_T("Hostname: ")) == 0)
			{
				m_strHostnames = line.Mid((int)strlen("Hostname: "));
			}
			else if (line.Find(_T("Web Service ID: ")) == 0)
			{
				data = line.Mid((int)strlen("Web Service ID: "));
				CString buffer;
				int j = 0, indexSemiColon = - 1;
				for (int i = 0; i < data.GetLength(); i++) {
					if (data[i] == _T(';')) {
						buffer = data.Mid(indexSemiColon + 1, i - indexSemiColon - 1);
						m_strEndPoints[j] = buffer;
						j ++;
						indexSemiColon = i;
					}
				}
			}
			else if (line.Find(_T("DPD ID: ")) == 0)
			{
				m_strDpdId = line.Mid((int)strlen("DPD ID: "));
			}
			else if (line.Find(_T("CSV Location: ")) == 0)
			{
				m_strCsvPath = line.Mid((int)strlen("CSV Location: "));
			}
			else if (line.Find(_T("Isolated Zipcodes: ")) == 0)
			{
				data = line.Mid((int)strlen("Isolated Zipcodes: "));
				int indexSemiColon = - 1;
				CString buffer;
				m_lstIsoZipcode.RemoveAll();
				for (int i = 0; i < data.GetLength(); i ++) {
					if (data[i] == _T(';')) {
						buffer = data.Mid(indexSemiColon + 1, i - indexSemiColon - 1);
						m_lstIsoZipcode.AddTail(buffer);
						indexSemiColon = i;
					}
				}
			}
			else if (line.Find(_T("Printer Name: ")) == 0) {
				m_strPrinter = line.Mid((int)strlen("Printer Name: "));
			}
			else if (line.Find(_T("Print Preview: ")) == 0) {
				data = line.Mid((int)strlen("Print Preview: "));
				m_bPrintPreview = (data == _T("T"));
			}
		}
		fileSettings.Close();
	}
	else
	{
		// Handle the error if the file cannot be opened
		AfxMessageBox(_T("No settings file. Load default settings."), MB_ICONERROR);
		DefaultSettings();
		SaveSettings();
	}
	return true;
}

void CPrestaCarrierLabelGenDlg::DefaultSettings() {
	m_strUsername = L"YZV9WFRRE5PN5BZ5IDLHCQCFKC3GX5EI";
	m_strPassword = L"";
	m_strHostnames = L"www.traveltags.nl;";
	m_strEndPoints[ADDRESSES_ENDPOINT] = L"/api/addresses";
	m_strEndPoints[CARRIERS_ENDPOINT] = L"/api/carriers";
	m_strEndPoints[COUNTRIES_ENDPOINT] = L"/api/countries";
	m_strEndPoints[CUSTOMERS_ENDPOINT] = L"/api/customers";
	m_strEndPoints[ORDERS_ENDPOINT] = L"/api/orders";
	m_strEndPoints[SHOPS_ENDPOINT] = L"/api/shops";
	m_strEndPoints[INVOICES_ENDPOINT] = L"/api/order_invoices";
	m_strDpdId = L"";
	m_strCsvPath = L"";
	m_strPrinter = DefaultPrinterName();
	m_bPrintPreview = false;
}


void CPrestaCarrierLabelGenDlg::OnBnClickedButtonSettings()
{
	// TODO: Add your control notification handler code here
	SettingsDlg settingsDialog;
	CString isoZipcode;
	for (int i = 0; i < m_lstIsoZipcode.GetCount(); i++) {
		POSITION pos = m_lstIsoZipcode.FindIndex(i);
		isoZipcode += m_lstIsoZipcode.GetAt(pos);
		isoZipcode += ";";
	}
	settingsDialog.SetValues(m_strUsername, m_strPassword, m_strHostnames, m_strEndPoints, m_strDpdId, m_strCsvPath, isoZipcode, m_strPrinter, m_bPrintPreview);
	if (IDOK == settingsDialog.DoModal()) {
		m_strUsername = settingsDialog.GetUsername();
		m_strPassword = settingsDialog.GetPassword();
		m_strHostnames = settingsDialog.GetHostname();
		for (int i = 0; i < ENDPOINT_COUNT; i++) {
			m_strEndPoints[i] = settingsDialog.GetWebserviceId(i);
		}
		m_strDpdId = settingsDialog.GetDpdId();
		m_strCsvPath = settingsDialog.GetLocation();

		isoZipcode = settingsDialog.GetZipcode();
		int indexSemiColon = -1;
		CString buffer;
		m_lstIsoZipcode.RemoveAll();
		for (int j = 0; j < isoZipcode.GetLength(); j++) {
			if (isoZipcode[j] == _T(';')) {
				buffer = isoZipcode.Mid(indexSemiColon + 1, j - indexSemiColon - 1);
				m_lstIsoZipcode.AddTail(buffer);
				indexSemiColon = j;
			}
		}
		m_strPrinter = settingsDialog.GetPrinterName();
		m_bPrintPreview = settingsDialog.GetPrintPreview();
		if (m_pServiceComm != nullptr) {
			m_pServiceComm->SetEndPoints(m_strEndPoints);
			m_pServiceComm->SetHostnames(m_strHostnames);
			CString authInput = m_strUsername + L":" + m_strPassword;
			std::string plainText = CT2A(authInput.GetString());
			std::string b64Enc = Xbase64_endcode(plainText);
			CString authString = CA2T(b64Enc.c_str());
			m_pServiceComm->SetAuth(authString);
		}

		SaveSettings();
	}
}

std::string CPrestaCarrierLabelGenDlg::Xbase64_endcode(const std::string& input) {
	BIO* bio, * b64;
	BUF_MEM* bufferPtr;

	// Create a BIO object and a base64 filter BIO
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());

	// Chain the BIOs together
	bio = BIO_push(b64, bio);

	// Write the input data to the base64 BIO
	BIO_write(bio, input.c_str(), static_cast<int>(input.length()));
	BIO_flush(bio);

	// Retrieve the result
	BIO_get_mem_ptr(bio, &bufferPtr);

	// Make a copy of the result since it will be freed when the BIO is destroyed
	std::string result(bufferPtr->data, bufferPtr->length);

	// Clean up
	BIO_free_all(bio);

	return result;
}

CString CPrestaCarrierLabelGenDlg::DefaultPrinterName() {
	// Get the default printer
	CString defPrinterName = _T("");
	TCHAR defaultPrinter[MAX_PATH];
	DWORD size = sizeof(defaultPrinter) / sizeof(defaultPrinter[0]);
	if (GetDefaultPrinter(defaultPrinter, &size))
	{
		defPrinterName.Format(_T("%s"), defaultPrinter);
	}
	return defPrinterName;
}

void CPrestaCarrierLabelGenDlg::WriteLogs(CString strLog) {
	CStdioFile logFile;
	CString logTime;
	// Get the current local system time
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	// Create a CTime object from the SYSTEMTIME structure
	CTime currentTime(sysTime);
	logTime = L">>>";
	logTime += currentTime.Format(_T("%Y-%m-%d %H:%M:%S"));
	if (logFile.Open(L"carrierlabel.log", CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText)) {
		logFile.SeekToEnd();
		logFile.WriteString(logTime);
		logFile.WriteString(L":\n");
		logFile.WriteString(strLog);
		logFile.WriteString(L"\n");
		logFile.Close();
	}
}