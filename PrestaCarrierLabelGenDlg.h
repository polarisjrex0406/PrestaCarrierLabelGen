
// PrestaCarrierLabelGenDlg.h : header file
//

#pragma once

#include <afxwin.h>
#include <afx.h>
#include <atlenc.h>
#include <wincrypt.h>
#include <hpdf.h>
#include <afxpriv.h>
#include <afxdisp.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include "PrestaServiceComm.h"
#include "SettingsDlg.h"
#include "CPDF_VIEWER.h"
#include "PrestaInputDialog.h"

#define GET_RECENT_CALL 0
#define GET_NEXT_CALL 1
#define GET_PREV_CALL 2
#define GET_DPD_FULL_CALL 3
#define GET_POSTNLE_FULL_CALL 4
#define UPDATE_DPD_SHIPPED_CALL 5
#define UPDATE_POSTNLE_SHIPPED_CALL 6
#define HIDE_ORDER_CALL 7
#define GET_FIRST_CALL 8
#define GET_LAST_CALL 9

#define MAX_SHOP_COUNT 10

#define YPOS(y) (88 - (y))

// CPrestaCarrierLabelGenDlg dialog
class CPrestaCarrierLabelGenDlg : public CDHtmlDialog/*, public CView*/
{
// Construction
public:
	CPrestaCarrierLabelGenDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRESTACARRIERLABELGEN_DIALOG, IDH = IDR_HTML_PRESTACARRIERLABELGEN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Customize Data
private:
	PrestaServiceComm* m_pServiceComm;
	bool m_bMouseWheelEnable;
	unsigned short m_usCallType;
	HPDF_Doc m_hPdfDoc;
	// Settings
	CString m_strUsername, m_strPassword;
	CString m_strHostnames;
	CString m_strEndPoints[ENDPOINT_COUNT];
	CString m_strDpdId;
	CString m_strCsvPath;
	CStringList m_lstIsoZipcode;
	CString m_strPrinter;
	bool m_bPrintPreview;

	CString m_strCsvSeparator;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()

public:
	std::string Xbase64_endcode(const std::string& input);

	void ShowMainScreenData();

	// Thread processing for calling API
	static UINT ThreadFunction(LPVOID pParam);
	void ThreadFinished(bool bStatus);
	void CallThread(unsigned short usCallType);
	PrestaServiceComm* ServiceComm();
	void ShowTipsWhileLoading(bool bShow);

	// Process for DPD
	void PreProcessDPD();
	void WriteUtf8ToFile(CFile csvFile, const CStringW strContent) {
		// Get the TCHAR[] from the CStringW object
		const wchar_t* wcharPtr = strContent.GetString();
		// Now you can safely cast it to TCHAR[], or use it directly if your project is Unicode
		const TCHAR* tCharPtr = reinterpret_cast<const TCHAR*>(wcharPtr);
		csvFile.Write(tCharPtr, lstrlen(tCharPtr));
	}
	// Generate Label
	void GenerateDPDLabel();
	void GeneratePostNL_E_Label();
	void GenerateJpegWithText(const CString& filePath, const CString& text)
	{
		// Create a CImage object for the JPEG image
		CImage image;
		image.Create(Milli2Point(85), Milli2Point(5.8), 24); // Width, Height, Bit depth (24-bit RGB)

		// Create a device context for the image
		CDC dc;
		dc.Attach(image.GetDC());

		// Clear the image to white
		CRect rect(0, 0, Milli2Point(85), Milli2Point(5.8)); // Entire image area
		dc.FillSolidRect(&rect, RGB(255, 255, 255)); // Fill with white

		// Set up font and text color for drawing text
		CFont font;
		font.CreateFontW(Milli2Point(5.5), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
		CFont* oldFont = dc.SelectObject(&font);
		dc.SetTextColor(RGB(0, 0, 0)); // Black color

		// Draw the text on the image
		dc.DrawText(text, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		// Clean up
		dc.SelectObject(oldFont);
		image.ReleaseDC();
		dc.Detach();

		// Save the CImage object as a JPEG file
		image.Save(filePath, Gdiplus::ImageFormatJPEG);
	}


	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnClickedButtonGenerate();
	// Milli To Point in PDF
	static float Milli2Point(float fMilli);
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonPrev();

	void WriteLogs(CString strLog);
private:
	CPDF_VIEWER m_pdfViewer;
public:
	afx_msg void OnBnClickedButtonRefresh();
	bool SaveSettings();
	bool LoadSettings();
	void DefaultSettings();
	afx_msg void OnBnClickedButtonSettings();
	static CString DefaultPrinterName();
};
