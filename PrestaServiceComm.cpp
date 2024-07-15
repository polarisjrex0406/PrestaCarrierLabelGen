#include "pch.h"
#include "PrestaServiceComm.h"


PrestaServiceComm::PrestaServiceComm(CString strAuthString) {
	m_lstAvailableState.AddTail(L"2");
	m_lstAvailableState.AddTail(L"3");
	m_lstAvailableState.AddTail(L"9");
	m_lstAvailableState.AddTail(L"16");
	m_lstAvailableState.AddTail(L"17");
	m_lstAvailableState.AddTail(L"29");

	m_lstHostnames.AddTail(L"www.traveltags.nl");
	m_lstHostnames.AddTail(L"www.eagletac.nl");

	m_strAuthString = strAuthString;
}

CStringW PrestaServiceComm::MakeApiCall(CString strHostname, CString strEndPoint, CString strMethod
	/* = L"GET"*/, CString strParams/* = L""*/, CString strData/* = L""*/) {
	CStringW responseText;
	CInternetSession session(_T("Order Carrier Session"));
	CHttpConnection* pConnection = session.GetHttpConnection(strHostname);
	CHttpFile* pFile = pConnection->OpenRequest(strMethod, strEndPoint, 
		nullptr, 1, nullptr, nullptr, INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE);
	CString requestHeaders;

	CString strAuthContent = L"Authorization: Basic ";
	strAuthContent += m_strAuthString;
	pFile->AddRequestHeaders(strAuthContent);
	int bufLen = 0;
	if (strMethod.Compare(L"GET") == 0) {
		requestHeaders = L"Content-Type: application/json\r";
		pFile->AddRequestHeaders(requestHeaders);
		pFile->SendRequest(nullptr, 0, (void*)(LPCTSTR)strParams, strParams.GetLength());

		TCHAR szBuf[1024];
		while (bufLen = pFile->Read(szBuf, 1023)) {
			BYTE* byteArray = new BYTE[bufLen * 2 + 1];
			for (int i = 0; i < bufLen; i++) {
				byteArray[i * 2] = LOBYTE(szBuf[i]);
				byteArray[i * 2 + 1] = HIBYTE(szBuf[i]);
			}

			CFile writeFile;
			if (writeFile.Open(L"accent.txt", CFile::modeWrite | CFile::modeCreate)) {
				//writeFile.SeekToEnd();
				writeFile.Write(szBuf, bufLen);
				writeFile.Close();
			}
			char filename[] = "accent.txt";
			CStringW str;
			std::ifstream file(filename, std::ios::binary);
			if (file.is_open()) {
				// Determine the file size
				file.seekg(0, std::ios::end);
				size_t file_size = file.tellg();
				file.seekg(0, std::ios::beg);
				// Allocate buffer to hold file content
				char* buffer = new char[file_size + 1];
				// Read file content into buffer
				file.read(buffer, file_size);
				buffer[file_size] = '\0'; // Null-terminate the string
				// Close the file
				file.close();
				// Convert UTF-8 encoded buffer to Unicode (UTF-16) CString
				str = CStringW(CA2W(buffer, CP_UTF8));
			}

			CFile::Remove(L"accent.txt");

			//CString str((const char*)byteArray, bufLen);
			responseText += str;
		}
	}
	else if (strMethod.Compare(L"PUT") == 0) {
		pFile->AddRequestHeaders(_T("Content-Type: application/xml"));		
		if (pFile->SendRequestEx(strData.GetLength() * sizeof(TCHAR))) {
			pFile->WriteString(strData);
			pFile->EndRequest();
		}
	}
	
	if (nullptr != pFile) {
		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);
	}

	m_strErrorMessage += L"strHostname: " + strHostname + L"\n";
	m_strErrorMessage += L"strEndPoint: " + strEndPoint + L"\n";
	m_strErrorMessage += L"strMethod: " + strMethod + L"\n";
	m_strErrorMessage += L"strParams: " + strParams + L"\n";
	m_strErrorMessage += L"strData: " + strData + L"\n";
	m_strErrorMessage += L"responseText: " + responseText + L"\n";

	pFile->Close();
	delete pFile;
	pConnection->Close();
	delete pConnection;
	session.Close();

	return responseText;
}

Json::Value PrestaServiceComm::ParseText2Json(CString strResponse) {
	Json::Value jsonVal;
	Json::Reader jsonReader;
	// Convert CString to a standard string
	std::string str;
	str = CT2A(strResponse);
	// Parse the JSON string into the Json::Value
	
	if (!jsonReader.parse(str, jsonVal)) {
		// JSON parsing failed
		std::cerr << "Failed to parse JSON: " << jsonReader.getFormattedErrorMessages() << std::endl;
	}
	return jsonVal;
}

CStringW PrestaServiceComm::ParseJsonTextVal2CString(Json::Value jsnVal) {
	CStringW textVal;
	Json::String jsnString;
	jsnString = jsnVal.asString();
	std::string stdString = jsnVal.asString();
	textVal = CStringW(stdString.c_str());
	return textVal;
}

// JSON value
Json::Value PrestaServiceComm::CurrentOrderInformation() {
	return m_jsnRoot;
}

Json::Value PrestaServiceComm::MergeJsonArray(Json::Value jsnMerged, Json::Value jsnRaw) {
	if (!jsnMerged.empty()) {
		for (int i = 0; i < jsnRaw["orders"].size(); i++) {
			jsnMerged["orders"].append(jsnRaw["orders"][i]);
		}
	}
	else {
		jsnMerged["orders"] = jsnRaw["orders"];
	}
	for (int i = 0; i < jsnMerged["orders"].size() - 1; i++) {
		for (int j = 0; j < jsnMerged["orders"].size() - i - 1; j++) {
			if (jsnMerged["orders"][j]["id"].asInt() > jsnMerged["orders"][j + 1]["id"].asInt()) {
				Json::Value jsonTemp;
				jsonTemp = jsnMerged["orders"][j];
				jsnMerged["orders"][j] = jsnMerged["orders"][j + 1];
				jsnMerged["orders"][j + 1] = jsonTemp;
			}
		}
	}
	return jsnMerged;
}

// Last Error Message
CString PrestaServiceComm::LastErrorMessage() {
	return m_strErrorMessage;
}

void PrestaServiceComm::ClearErrorMessage() {
	m_strErrorMessage = L"";
}

CString PrestaServiceComm::MethodGET(CString strHostname, CString strEndPoint, CString strFilter, 
	CString strDisplay, CString strSortBy/* = L""*/, CString strLimit/* = L"?limit=1"*/, 
	CString strOutputFormat/* = L"&output_format=JSON"*/) {
	m_strErrorMessage += L"Called MethodGET()\n";
	CString responseText, params;
	CString method = L"GET";
	// Limit by
	params += strLimit;
	// Filter
	params += strFilter;
	// Display by fields
	params += strDisplay;
	// Sort by date_add
	params += strSortBy;
	// JSON-format output
	params += strOutputFormat;
	responseText = MakeApiCall(strHostname, strEndPoint + params, method);
	return responseText;
}

bool PrestaServiceComm::GetOpenOrder(CString strDisplay, CString strFilter) {
	CString filter, responseText;
	Json::Value jsonVal;
	m_strErrorMessage += L"Called GetOpenOrder()\n";
	responseText = MethodGET(m_aryPrestaOrders.CurrentHostname(), m_strEndPoints[ORDERS_ENDPOINT],
		strFilter, strDisplay);
	jsonVal = ParseText2Json(responseText);
	if (jsonVal.empty()) {
		m_strErrorMessage += L"Failed GetOpenOrder()\n";
		return false;
	}
	else {
		m_strErrorMessage += L"Succeed GetOpenOrder()\n";
		m_jsnTemp["orders"] = jsonVal["orders"];
		return true;
	}
}

bool PrestaServiceComm::GetCustomer(CString strDisplay/* = L"&&display=full"*/, CString strFilter/* = L""*/) {
	CString responseText;
	Json::Value jsonVal;
	m_strErrorMessage += L"Called GetCustomer()\n";
	responseText = MethodGET(m_aryPrestaOrders.CurrentHostname(), m_strEndPoints[CUSTOMERS_ENDPOINT], strFilter, strDisplay);
	jsonVal = ParseText2Json(responseText);
	if (jsonVal.empty()) {
		m_strErrorMessage += L"Failed GetCustomer()\n";
		return false;
	}
	else {
		m_strErrorMessage += L"Succeed GetCustomer()\n";
		m_jsnTemp["customers"] = jsonVal["customers"];
		return true;
	}
}

bool PrestaServiceComm::GetCarrier(CString strDisplay/* = L"&&display=full"*/, CString strFilter/* = L""*/) {
	CString responseText;
	Json::Value jsonVal;
	m_strErrorMessage += L"Called GetCarrier()\n";
	responseText = MethodGET(m_aryPrestaOrders.CurrentHostname(), m_strEndPoints[CARRIERS_ENDPOINT], strFilter, strDisplay);
	jsonVal = ParseText2Json(responseText);
	if (jsonVal.empty()) {
		m_strErrorMessage += L"Failed GetCarrier()\n";
		return false;
	}
	else {
		m_strErrorMessage += L"Succeed GetCarrier()\n";
		m_jsnTemp["carriers"] = jsonVal["carriers"];
		return true;
	}
}

bool PrestaServiceComm::GetShop(CString strDisplay/* = L"&&display=full"*/, CString strFilter/* = L""*/) {
	CString responseText;
	Json::Value jsonVal;
	m_strErrorMessage += L"Called GetShop()\n";
	responseText = MethodGET(m_aryPrestaOrders.CurrentHostname(), m_strEndPoints[SHOPS_ENDPOINT], strFilter, strDisplay);
	jsonVal = ParseText2Json(responseText);
	if (jsonVal.empty()) {
		m_strErrorMessage += L"Failed GetShop()\n";
		return false;
	}
	else {
		m_strErrorMessage += L"Succeed GetShop()\n";
		m_jsnTemp["shops"] = jsonVal["shops"];
		return true;
	}
}

// Get Address
bool PrestaServiceComm::GetAddress(CString strDisplay/* = L"&display=full"*/, CString strFilter/* = L""*/) {
	CString responseText;
	Json::Value jsonVal;
	m_strErrorMessage += L"Called GetAddress()\n";
	responseText = MethodGET(m_aryPrestaOrders.CurrentHostname(), m_strEndPoints[ADDRESSES_ENDPOINT], strFilter, strDisplay);
	jsonVal = ParseText2Json(responseText);
	if (jsonVal.empty()) {
		m_strErrorMessage += L"Failed GetAddress()\n";
		return false;
	}
	else {
		m_strErrorMessage += L"Succeed GetAddress()\n";
		m_jsnTemp["addresses"] = jsonVal["addresses"];
		return true;
	}
}
// Get Invoice
bool PrestaServiceComm::GetInvoice(CString strDisplay/* = L"&display=full" */ , CString strFilter/* = L""*/) {
	CString responseText;
	Json::Value jsonVal;
	m_strErrorMessage += L"Called GetInvoice()\n";
	responseText = MethodGET(m_aryPrestaOrders.CurrentHostname(), m_strEndPoints[INVOICES_ENDPOINT], strFilter, strDisplay);
	jsonVal = ParseText2Json(responseText);
	if (jsonVal.empty()) {
		m_strErrorMessage += L"Failed GetInvoice()\n";
		return false;
	}
	else {
		m_strErrorMessage += L"Succeed GetInvoice()\n";
		m_jsnTemp["order_invoices"] = jsonVal["order_invoices"];
		return true;
	}
}
// Get Country
bool PrestaServiceComm::GetCountry(CString strDisplay/* = L"&display=full"*/, CString strFilter/* = L""*/) {
	m_strErrorMessage += L"Called GetCountry()\n";
	CString responseText;
	Json::Value jsonVal;
	responseText = MethodGET(m_aryPrestaOrders.CurrentHostname(), m_strEndPoints[COUNTRIES_ENDPOINT], strFilter, strDisplay);
	jsonVal = ParseText2Json(responseText);
	if (jsonVal.empty()) {
		m_strErrorMessage += L"Failed GetCountry()\n";
		return false;
	}
	else {
		m_strErrorMessage += L"Succeed GetCountry()\n";
		m_jsnTemp["countries"] = jsonVal["countries"];
		return true;
	}
}

CString PrestaServiceComm::GetXml(CString strEndPoint, CString strFilter/* = L""*/) {
	m_strErrorMessage += L"Called GetXml()\n";
	CString responseText, display, sortBy, outputFormat, limit;
	display = L"&display=full";
	sortBy = L"";
	outputFormat = L"";
	limit = L"";
	responseText = MethodGET(m_aryPrestaOrders.CurrentHostname(), strEndPoint, strFilter, display, sortBy, limit, outputFormat);
	return responseText;
}

bool PrestaServiceComm::GetShowData() {
	m_strErrorMessage += L"Called GetShowData()\n";
	CString responseText, display, filter;
	bool result = false;
	m_jsnTemp = m_jsnRoot;
	// Retrieve data from orders
	filter = L"&filter[id]=[" + m_aryPrestaOrders.CurrentOrderId() + L"]";
	// Display id, reference, id_carrier, id_customer, id_shop from orders
	display = L"&display=[id,reference,id_carrier,id_customer,id_shop,id_address_delivery]";
	result = GetOpenOrder(display, filter);
	if (!result) {
		m_jsnTemp.clear();
		return result;
	}
	// Retrieve data from addresses
	display = L"&display=[lastname]";
	filter = L"&filter[id]=[" + ParseJsonTextVal2CString(m_jsnTemp["orders"][0]["id_address_delivery"]) + L"]";
	result = GetAddress(display, filter);
	if (!result) {
		m_jsnTemp.clear();
		return result;
	}
	// Retrieve data from carriers
	display = L"&display=[name]";
	filter = L"&filter[id]=[" + ParseJsonTextVal2CString(m_jsnTemp["orders"][0]["id_carrier"]) + L"]";
	result = GetCarrier(display, filter);
	if (!result) {
		m_jsnTemp.clear();
		return result;
	}
	// Retrieve data from shops
	display = L"&display=[name]";
	filter = L"&filter[id]=[" + ParseJsonTextVal2CString(m_jsnTemp["orders"][0]["id_shop"]) + L"]";
	result = GetShop(display, filter);
	if (!result) {
		m_jsnTemp.clear();
		return result;
	}
	m_jsnRoot = m_jsnTemp;
	return result;
}

bool PrestaServiceComm::GetRefresh(int nType/* = RECENT_REFRESH*/, CString strSortBy/* = L""*/) {
	m_strErrorMessage += L"Called GetRefresh()\n";
	CString filter, display, limit, responseText;
	Json::Value jsonMerged, jsonTemp;
	display = L"&display=[id,current_state]";
	int limitCount = 30;
	if (m_lstHostnames.GetCount() > 0) {
		limitCount = m_aryPrestaOrders.GetMaxSize() / m_lstHostnames.GetCount();
	}
	limit.Format(L"?limit=%d", limitCount);
	// Filter by current_state
	if (!m_lstAvailableState.IsEmpty()) {
		POSITION pos;
		for (int i = 0; i < m_lstAvailableState.GetCount(); i++) {
			filter += (0 == i) ? L"&filter[current_state]=[" : L"|";
			pos = m_lstAvailableState.FindIndex(i);
			filter += m_lstAvailableState.GetAt(pos);
		}
		if (m_lstAvailableState.GetCount() > 0) {
			filter += L"]";
		}
	}
	for (int i = 0; i < m_lstHostnames.GetCount(); i++) {
		POSITION pos = m_lstHostnames.FindIndex(i);
		CString filterById = L"";
		if (nType != RECENT_REFRESH && nType != FIRST_REFRESH && nType != LAST_REFRESH) {
			if (nType == PREV_REFRESH && m_aryPrestaOrders.FirstId(m_lstHostnames.GetAt(pos)) != L"-1") {
				filterById += L"&filter[id]=<[" + m_aryPrestaOrders.FirstId(m_lstHostnames.GetAt(pos)) + L"]";
			}
			else if (nType == NEXT_REFRESH && m_aryPrestaOrders.LastId(m_lstHostnames.GetAt(pos)) != L"-1") {
				filterById += L"&filter[id]=>[" + m_aryPrestaOrders.LastId(m_lstHostnames.GetAt(pos)) + L"]";
			}
		}
		responseText = MethodGET(m_lstHostnames.GetAt(pos), m_strEndPoints[ORDERS_ENDPOINT],
			filter + filterById, display, strSortBy, limit);
		Json::Value jsonRaw = ParseText2Json(responseText);
		// Convert ordered objects to array
		if (jsonRaw.empty() || jsonRaw["orders"].isArray()) {
			jsonTemp = jsonRaw;
		}
		else if (jsonRaw["orders"].isObject()) {
			// Iterate through the members of the object
			std::vector<std::string> memberNames = jsonRaw["orders"].getMemberNames();
			jsonTemp.clear();
			for (const auto& memberName : memberNames)
			{
				Json::Value memberValue = jsonRaw["orders"][memberName];
				// Process the member value
				jsonTemp["orders"].append(memberValue);
			}
		}
		else {
			jsonTemp.clear();
		}

		if (!jsonTemp.empty() && !jsonTemp["orders"].empty()) {
			for (int j = 0; j < jsonTemp["orders"].size(); j++) {
				std::string hostname = CT2A(m_lstHostnames.GetAt(pos));
				jsonTemp["orders"][j]["hostname"] = hostname;
			}
			jsonMerged = MergeJsonArray(jsonMerged, jsonTemp);
		}
	}
	if (jsonMerged.empty() || jsonMerged["orders"].empty()) {
		return false;
	}
	if (nType == PREV_REFRESH) {
		m_aryPrestaOrders.AddHead(jsonMerged["orders"]);
	}
	else {
		m_aryPrestaOrders.AddTail(jsonMerged["orders"]);
		if (nType == FIRST_REFRESH) {
			m_aryPrestaOrders.setIndex(0);
		}
	}
	return GetShowData();
}

bool PrestaServiceComm::GetRecent() {
	m_strErrorMessage += L"Called GetRecent()\n";
	CString sortBy = L"&sort=[id_DESC]";
	m_aryPrestaOrders.ClearAllOrders();
	return GetRefresh(RECENT_REFRESH, sortBy);
}

bool PrestaServiceComm::GetPrev() {
	m_strErrorMessage += L"Called GetPrev()\n";
	CString filter, sortBy;
	sortBy = L"&sort=[id_DESC]";
	if (m_aryPrestaOrders.IsFirst()) {
		return GetRefresh(PREV_REFRESH, sortBy);
	}
	else {
		m_aryPrestaOrders.PrevOrder();
		return GetShowData();
	}
}

bool PrestaServiceComm::GetNext() {
	m_strErrorMessage += L"Called GetNext()\n";
	CString filter, sortBy;
	sortBy = L"&sort=[id_ASC]";
	if (m_aryPrestaOrders.IsLast()) {
		return GetRefresh(NEXT_REFRESH, sortBy);
	}
	else {
		m_aryPrestaOrders.NextOrder();
		return GetShowData();
	}
}

bool PrestaServiceComm::GetFirst() {
	m_strErrorMessage += L"Called GetFirst()\n";
	CString sortBy = L"&sort=[id_DESC]";
	m_aryPrestaOrders.ClearAllOrders();
	return GetRefresh(FIRST_REFRESH, sortBy);
}

bool PrestaServiceComm::GetLast() {
	m_strErrorMessage += L"Called GetLast()\n";
	CString sortBy = L"&sort=[id_ASC]";
	m_aryPrestaOrders.ClearAllOrders();
	return GetRefresh(LAST_REFRESH, sortBy);
}

// Get Full data
bool PrestaServiceComm::GetFullData() {
	m_strErrorMessage += L"Called GetFullData()\n";
	CString responseText, display, filter;
	bool result;
	Json::Value jsonFull, jsonResource;
	m_jsnTemp = m_jsnRoot;
	display = L"&display=full";
	// Retrieve data from open orders
	filter = L"&filter[id]=[" + ParseJsonTextVal2CString(m_jsnTemp["orders"][0]["id"]) + L"]";
	result = GetOpenOrder(display, filter);
	if (!result) {
		m_jsnTemp.clear();
		return result;
	}
	// Retrieve data from order_invoice, address, country, customer
	// order_invoice
	filter = L"&filter[id_order]=[" + ParseJsonTextVal2CString(m_jsnTemp["orders"][0]["id"]) + L"]";
	result = GetInvoice(display, filter);
	if (!result) {
		m_jsnTemp.clear();
		return result;
	}
	// address
	filter = L"&filter[id]=[" + ParseJsonTextVal2CString(m_jsnTemp["orders"][0]["id_address_delivery"]) + L"]";
	result = GetAddress(display, filter);
	if (!result) {
		m_jsnTemp.clear();
		return result;
	}
	// country
	filter = L"&filter[id]=[" + ParseJsonTextVal2CString(m_jsnTemp["addresses"][0]["id_country"]) + L"]";
	result = GetCountry(display, filter);
	if (!result) {
		m_jsnTemp.clear();
		return result;
	}
	// customer
	filter = L"&filter[id]=[" + ParseJsonTextVal2CString(m_jsnTemp["orders"][0]["id_customer"]) + L"]";
	result = GetCustomer(display, filter);
	if (!result) {
		m_jsnTemp.clear();
		return result;
	}
	// shops
	filter = L"&filter[id]=[" + ParseJsonTextVal2CString(m_jsnTemp["orders"][0]["id_shop"]) + L"]";
	result = GetShop(display, filter);
	if (!result) {
		m_jsnTemp.clear();
		return result;
	}
	m_jsnRoot = m_jsnTemp;
	return result;
}

// Method PUT
CString PrestaServiceComm::MethodPUT(CString strHostname, CString strEndPoint, CString strFilter, CString strUpdateXml, CString strOutputFormat/* = L"output_format=JSON"*/) {
	m_strErrorMessage += L"Called MethodPUT()\n";
	CString responseText, params;
	CString method = L"PUT";
	responseText = MakeApiCall(strHostname, strEndPoint, method, L"", strUpdateXml);
	return responseText;
}

// Update Order
bool PrestaServiceComm::UpdateOrder(CString strFilter, CString strUpdateXml) {
	m_strErrorMessage += L"Called UpdateOrder()\n";
	CString endPoint = m_strEndPoints[ORDERS_ENDPOINT] + "/" + strFilter;
	m_strErrorMessage = MethodPUT(m_aryPrestaOrders.CurrentHostname(), endPoint, L"", strUpdateXml);
	return true;
}

// Update Order as 'shipped'
bool PrestaServiceComm::UpdateAsShipped(int nCurrentState/* = 4*/) {
	m_strErrorMessage += L"Called UpdateAsShipped()\n";
	CString orderXml, updateXml, endPoint, filter;
	bool res;
	// Get full data of 'orders' in XML format
	endPoint = m_strEndPoints[ORDERS_ENDPOINT];
	filter = L"&filter[id]=[" + ParseJsonTextVal2CString(m_jsnRoot["orders"][0]["id"]) + L"]";
	orderXml = GetXml(endPoint, filter);
	// Parse orderXml as XML document
	CoInitialize(NULL);
	try
	{
		// Create an instance of the XML DOMDocument
		MSXML2::IXMLDOMDocumentPtr xmlDoc;
		xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
		if (xmlDoc == NULL)
		{
			m_strErrorMessage = _T("Failed to create XML DOMDocument.");
			return false;
		}
		// Load the XML from the CString
		VARIANT_BOOL success = xmlDoc->loadXML(static_cast<const wchar_t*>(orderXml));
		if (success == VARIANT_FALSE)
		{
			m_strErrorMessage = _T("Failed to load XML from CString.");
			return false;
		}
		// Get the <current_state> node
		MSXML2::IXMLDOMNodePtr currentStateNode = xmlDoc->selectSingleNode(_T("//current_state"));

		// Get the parent node of orderNode.
		if (currentStateNode != NULL) {
			currentStateNode->Puttext(L"4");
		}
		else {
			m_strErrorMessage = _T("Failed to find <current_state> node.");
			return false;
		}

		// Serialize the updated XML back to the CString
		LPWSTR lpBuf = orderXml.GetBufferSetLength(xmlDoc->xml.length());
		BSTR* pbstr = new BSTR;
		// Check if lpwstr is not NULL
		if (lpBuf != nullptr) {
			// Allocate a new BSTR and copy the LPWSTR content
			*pbstr = SysAllocString(lpBuf);
		}
		else {
			// If lpwstr is NULL, assign an empty BSTR
			*pbstr = SysAllocString(L"");
		}
		xmlDoc->get_xml(pbstr);
		orderXml.ReleaseBuffer();
		_bstr_t xmlBstr = xmlDoc->Getxml();
		updateXml = static_cast<const wchar_t*>(xmlBstr);
	}
	catch (_com_error& e) {
		// Handle COM errors
		m_strErrorMessage = _T("COM error occurred: ") + CString(e.ErrorMessage());
		return false;
	}
	catch (...) {
		// Handle other exceptions
		m_strErrorMessage = _T("An unknown error occurred.");
		return false;
	}
	CoUninitialize();
	filter = ParseJsonTextVal2CString(m_jsnRoot["orders"][0]["id"]);
	updateXml.Replace(L"  ", L"");
	updateXml.Remove(_T('\t'));
	updateXml.Replace(L"\n", L"");
	updateXml.Replace(_T("\r\n"), _T(" "));  // Replace Windows-style newline
	updateXml.Replace(_T("\n"), _T(" "));    // Replace Unix-style newline
	updateXml.Replace(_T("\r"), _T(" "));    // Replace old Mac-style newline
	updateXml.Replace(L"<orders>", L"");
	updateXml.Replace(L"</orders>", L"");
 	res = UpdateOrder(filter, updateXml);
	if (!res) return res;
	return HideOrder();
}

bool PrestaServiceComm::HideOrder() {
	m_strErrorMessage += L"Called HideOrder()\n";
	bool res;
	m_aryPrestaOrders.DeleteOrder();
	if (m_aryPrestaOrders.IsEmpty()) {
		res = GetRecent();
	}
	else {
		res = GetShowData();
	}
	return res;
}

void PrestaServiceComm::SetEndPoints(CString strEndPoints[]) {
	for (int i = 0; i < ENDPOINT_COUNT; i++) {
		m_strEndPoints[i] = strEndPoints[i];
	}
}

void PrestaServiceComm::SetHostnames(CString strHostnames) {
	m_lstHostnames.RemoveAll();
	int indexSemiColon = -1;
	CString buffer;
	for (int i = 0; i < strHostnames.GetLength(); i++) {
		if (strHostnames[i] == _T(';')) {
			buffer = strHostnames.Mid(indexSemiColon + 1, i - indexSemiColon - 1);
			m_lstHostnames.AddTail(buffer);
			indexSemiColon = i;
		}
	}
}

void PrestaServiceComm::SetAuth(CString strAuth) {
	m_strAuthString = strAuth;
}