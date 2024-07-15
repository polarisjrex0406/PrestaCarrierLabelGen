#pragma once

// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated
// by Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////

#include "afxwin.h"

class CPDF_VIEWER : public CWnd
{
protected:
	DECLARE_DYNCREATE(CPDF_VIEWER)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= {0xca8a9780, 0x280d, 0x11cf, {0xa2, 0x4d, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = nullptr)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID);
	}

	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
				UINT nID, CFile* pPersist = nullptr, BOOL bStorage = FALSE,
				BSTR bstrLicKey = nullptr)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// Attributes
public:


// Operations
public:
// IAcroAXDocShim

// Functions
//

	CString get_src()
	{
		CString result;
		InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
		return result;
	}

	void put_src(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR;
		InvokeHelper(0x1, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
	}

	BOOL LoadFile(LPCTSTR fileName)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, fileName);
		return result;
	}

	void setShowToolbar(BOOL On)
	{
		static BYTE parms[] = VTS_BOOL;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, On);
	}

	void gotoFirstPage()
	{
		InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}

	void gotoLastPage()
	{
		InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}

	void gotoNextPage()
	{
		InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}

	void gotoPreviousPage()
	{
		InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}

	void setCurrentPage(long n)
	{
		static BYTE parms[] = VTS_I4;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, n);
	}

	void goForwardStack()
	{
		InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}

	void goBackwardStack()
	{
		InvokeHelper(0xA, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}

	void setPageMode(LPCTSTR pageMode)
	{
		static BYTE parms[] = VTS_BSTR;
		InvokeHelper(0xB, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, pageMode);
	}

	void setLayoutMode(LPCTSTR layoutMode)
	{
		static BYTE parms[] = VTS_BSTR;
		InvokeHelper(0xC, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, layoutMode);
	}

	void setNamedDest(LPCTSTR namedDest)
	{
		static BYTE parms[] = VTS_BSTR;
		InvokeHelper(0xD, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, namedDest);
	}

	void Print()
	{
		InvokeHelper(0xE, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}

	void printWithDialog()
	{
		InvokeHelper(0xF, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}

	void setZoom(float percent)
	{
		static BYTE parms[] = VTS_R4;
		InvokeHelper(0x10, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, percent);
	}

	void setZoomScroll(float percent, float left, float top)
	{
		static BYTE parms[] = VTS_R4 VTS_R4 VTS_R4;
		InvokeHelper(0x11, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, percent, left, top);
	}

	void setView(LPCTSTR viewMode)
	{
		static BYTE parms[] = VTS_BSTR;
		InvokeHelper(0x12, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, viewMode);
	}

	void setViewScroll(LPCTSTR viewMode, float offset)
	{
		static BYTE parms[] = VTS_BSTR VTS_R4;
		InvokeHelper(0x13, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, viewMode, offset);
	}

	void setViewRect(float left, float top, float width, float height)
	{
		static BYTE parms[] = VTS_R4 VTS_R4 VTS_R4 VTS_R4;
		InvokeHelper(0x14, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, left, top, width, height);
	}

	void printPages(long from, long to)
	{
		static BYTE parms[] = VTS_I4 VTS_I4;
		InvokeHelper(0x15, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, from, to);
	}

	void printPagesFit(long from, long to, BOOL shrinkToFit)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_BOOL;
		InvokeHelper(0x16, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, from, to, shrinkToFit);
	}

	void printAll()
	{
		InvokeHelper(0x17, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}

	void printAllFit(BOOL shrinkToFit)
	{
		static BYTE parms[] = VTS_BOOL;
		InvokeHelper(0x18, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, shrinkToFit);
	}

	void setShowScrollbars(BOOL On)
	{
		static BYTE parms[] = VTS_BOOL;
		InvokeHelper(0x19, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, On);
	}

	VARIANT GetVersions()
	{
		VARIANT result;
		InvokeHelper(0x1A, DISPATCH_METHOD, VT_VARIANT, (void*)&result, nullptr);
		return result;
	}

	void setCurrentHightlight(long a, long b, long c, long d)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4;
		InvokeHelper(0x1B, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, a, b, c, d);
	}

	void setCurrentHighlight(long a, long b, long c, long d)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4;
		InvokeHelper(0x1C, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, a, b, c, d);
	}

	void postMessage(VARIANT strArray)
	{
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0x1D, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, &strArray);
	}

	VARIANT get_messageHandler()
	{
		VARIANT result;
		InvokeHelper(0x1E, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, nullptr);
		return result;
	}

	void put_messageHandler(VARIANT newValue)
	{
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0x1E, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, &newValue);
	}

	void execCommand(VARIANT strArray)
	{
		static BYTE parms[] = VTS_VARIANT;
		InvokeHelper(0x1F, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, &strArray);
	}



};
