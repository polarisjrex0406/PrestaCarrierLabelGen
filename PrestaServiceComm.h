#pragma once
#include <afxinet.h>
#pragma warning(disable : 4192)
#import <msxml6.dll>
#pragma warning(default : 4192)
#include "CPrestaOrderArray.h"

#include <iostream>
#include <fstream>


inline CString VariantToCString(const _variant_t& varSrc)
{
	_bstr_t bstr(varSrc);
	return static_cast<LPCTSTR>(bstr);
}

#define ENDPOINT_COUNT 7
#define ADDRESSES_ENDPOINT 0
#define CARRIERS_ENDPOINT 1
#define COUNTRIES_ENDPOINT 2
#define CUSTOMERS_ENDPOINT 3
#define ORDERS_ENDPOINT 4
#define SHOPS_ENDPOINT 5
#define INVOICES_ENDPOINT 6

#define PARSE_XML_SUCCESS 0
#define PARSE_XML_ERROR 1

#define XPATH_COUNT 6
#define ORDER_ID_XPATH 0
#define REFERENCE_XPATH 1
#define CUSTOMER_NAME_XPATH 2
#define ORDER_CARRIER_XPATH 3
#define SHOP_NAME_XPATH 4
#define DATE_ADD_XPATH 5

#define RECENT_REFRESH 0
#define PREV_REFRESH -1
#define NEXT_REFRESH 1
#define FIRST_REFRESH -2
#define LAST_REFRESH 2

class PrestaServiceComm
{
public:
	PrestaServiceComm(CString strAuthString);
	CStringW MakeApiCall(CString strHostname, CString strEndPoint, CString strMethod = L"GET",
		CString strParams = L"", CString strData = L"");

	// Method GET
	CString MethodGET(CString strHostname, CString strEndPoint, CString strFilter,
		CString strDisplay, CString strSortBy = L"", CString strLimit = L"?limit=1",
		CString strOutputFormat = L"&output_format=JSON");
	// Get Open Order
	bool GetOpenOrder(CString strDisplay, CString strFilter);
	// Get Customer
	bool GetCustomer(CString strDisplay = L"&display=full", CString strFilter = L"");
	// Get Carrier
	bool GetCarrier(CString strDisplay = L"&display=full", CString strFilter = L"");
	// Get Shop
	bool GetShop(CString strDisplay = L"&display=full", CString strFilter = L"");
	// Get Address
	bool GetAddress(CString strDisplay = L"&display=full", CString strFilter = L"");
	// Get Invoice
	bool GetInvoice(CString strDisplay = L"&display=full", CString strFilter = L"");
	// Get Country
	bool GetCountry(CString strDisplay = L"&display=full", CString strFilter = L"");
	// Get as XML
	CString GetXml(CString strEndPoint, CString strFilter);
	// Get Data for Show
	bool GetShowData();
	bool GetRefresh(int nType = RECENT_REFRESH, CString strSortBy = L"");
	bool GetRecent();
	bool GetPrev();
	bool GetNext();
	bool GetFirst();
	bool GetLast();
	// Get Data for Generate
	bool GetFullData();

	// Method PUT
	CString MethodPUT(CString strHostname, CString strEndPoint, CString strFilter, CString strUpdateXml, CString strOutputFormat = L"output_format=JSON");
	// Update Order
	bool UpdateOrder(CString strFilter, CString strUpdateXml);
	// Update Order as 'shipped'
	bool UpdateAsShipped(int nCurrentState = 4);
	bool HideOrder();

	// Parse Text to JSON
	Json::Value ParseText2Json(CString strResponse);
	// Parse Json Text Value to CString
	static CStringW ParseJsonTextVal2CString(Json::Value jsnVal);
	// JSON value
	Json::Value CurrentOrderInformation();
	// Merge JSON Array
	Json::Value MergeJsonArray(Json::Value jsnMerged, Json::Value jsnRaw);
	bool CompareJsonObjects(const Json::Value& obj1, const Json::Value& obj2) {
		// Compare the "id" property of the JSON objects
		return obj1["id"].asInt() < obj2["id"].asInt();
	}
	// Last Error Message
	CString LastErrorMessage();
	void ClearErrorMessage();
	// Set End Points
	void SetEndPoints(CString strEndPoints[]);
	void SetHostnames(CString strHostnames);
	void SetAuth(CString strAuth);

private:
	CString m_strBaseUrl;
	CString m_strAuthString;

	CStringList m_lstAvailableState;

	CStringList m_lstInvisibleOrder;

	CStringList m_lstHostnames;

	Json::Value m_jsnRoot, m_jsnTemp;
	CString m_strErrorMessage;

	CPrestaOrderArray m_aryPrestaOrders;
	
	// End Points
	CString m_strEndPoints[ENDPOINT_COUNT];

	CStringA ConvertUnicodeToUTF8(const CStringW& uni)
	{
		if (uni.IsEmpty()) return ""; // nothing to do
		CStringA utf8;
		int cc = 0;
		// get length (cc) of the new multibyte string excluding the \0 terminator first
		if ((cc = WideCharToMultiByte(CP_UTF8, 0, uni, -1, NULL, 0, 0, 0) - 1) > 0)
		{
			// convert
			char* buf = utf8.GetBuffer(cc);
			if (buf) WideCharToMultiByte(CP_UTF8, 0, uni, -1, buf, cc, 0, 0);
			utf8.ReleaseBuffer();
		}
		return utf8;
	}

	CString Utf8ToCString(const TCHAR* utf8Text) {
		int len = lstrlen(utf8Text) + 1;
		char* buffer = new char[len];
		WideCharToMultiByte(CP_ACP, 0, utf8Text, -1, buffer, len, nullptr, nullptr);
		CString ansiText(buffer);
		delete[] buffer;
		return ansiText;
	}
};