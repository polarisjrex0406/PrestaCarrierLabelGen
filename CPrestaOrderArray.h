#pragma once

#include <json/json.h>
#include <json/writer.h>
#include <iostream>

#define MAX_SIZE 1000

struct COrder {
	CString orderId;
	CString hostname;
};

class CPrestaOrderArray
{
public:
	CPrestaOrderArray();

	int GetMaxSize() { return MAX_SIZE; }
	void setIndex(int nIndex) { m_nIndex = nIndex; }
	CString CurrentOrderId();
	CString CurrentHostname();
	CString LastId(CString strHostname);
	CString FirstId(CString strHostname);
	void NextOrder();
	void PrevOrder();
	void DeleteOrder();
	void ClearAllOrders();
	bool IsLast();
	bool IsFirst();
	bool IsEmpty();
	int OrderCount();
	void AddTail(Json::Value jsnOrders);
	void AddHead(Json::Value jsnOrders);
	CString Json2CString(Json::Value jsnVal) {
		CString textVal;
		std::string stdString = jsnVal.asString();
		textVal = CString(stdString.c_str());
		return textVal;
	}
private:
	int m_nSize;
	int m_nIndex;
	COrder m_aryOrder[MAX_SIZE];
};

