#include "pch.h"
#include "CPrestaOrderArray.h"

CPrestaOrderArray::CPrestaOrderArray() {
	m_nSize = 0;
	m_nIndex = -1;
}

CString CPrestaOrderArray::CurrentOrderId() {
	return m_aryOrder[m_nIndex].orderId;
}

CString CPrestaOrderArray::CurrentHostname() {
	return m_aryOrder[m_nIndex].hostname;
}

CString CPrestaOrderArray::LastId(CString strHostname) {
	CString lastOrderId = L"-1";
	for (int i = 0; i < m_nSize; i++) {
		if (m_aryOrder[i].hostname == strHostname) {
			if (_tstoi(lastOrderId) < _tstoi(m_aryOrder[i].orderId)) {
				lastOrderId = m_aryOrder[i].orderId;
			}
		}
	}
	return lastOrderId;
}

CString CPrestaOrderArray::FirstId(CString strHostname) {
	CString firstOrderId = L"-1";
	for (int i = 0; i < m_nSize; i++) {
		if (m_aryOrder[i].hostname == strHostname) {
			if (firstOrderId == L"-1") {
				firstOrderId = m_aryOrder[i].orderId;
				continue;
			}
			if (_tstoi(firstOrderId) > _tstoi(m_aryOrder[i].orderId)) {
				firstOrderId = m_aryOrder[i].orderId;
			}
		}
	}
	return firstOrderId;
}

void CPrestaOrderArray::NextOrder() {
	if (!IsLast()) {
		m_nIndex++;
	}
}

void CPrestaOrderArray::PrevOrder() {
	if (!IsFirst()) {
		m_nIndex--;
	}
}

void CPrestaOrderArray::DeleteOrder() {
	if (m_nSize > 0) {
		for (int i = m_nIndex; i < m_nSize - 1; i++) {
			m_aryOrder[i] = m_aryOrder[i + 1];
		}
		m_nSize--;
		//m_nIndex = m_nIndex > 0 ? m_nIndex - 1: m_nIndex;
		m_nIndex = m_nIndex < m_nSize ? m_nIndex : m_nIndex - 1;
	}
}

void CPrestaOrderArray::ClearAllOrders() {
	m_nSize = 0;
	m_nIndex = -1;
}

bool CPrestaOrderArray::IsLast() {
	return m_nIndex == (m_nSize - 1);
}

bool CPrestaOrderArray::IsFirst() {
	return m_nIndex == 0;
}

bool CPrestaOrderArray::IsEmpty() {
	return m_nSize == 0;
}

int CPrestaOrderArray::OrderCount() {
	return m_nSize;
}

void CPrestaOrderArray::AddTail(Json::Value jsnOrders) {
	int deleteCount, remainCount, addCount;
	if (jsnOrders.size() == 0) {
		return;
	}
	addCount = MAX_SIZE > jsnOrders.size() ? jsnOrders.size() : MAX_SIZE;
	deleteCount = (m_nSize + addCount) - MAX_SIZE;
	if (deleteCount <= 0) {
		deleteCount = 0;
	}
	remainCount = m_nSize - deleteCount;
	if (deleteCount > 0) {
		for (int i = 0; i < remainCount; i++) {
			m_aryOrder[i] = m_aryOrder[i + deleteCount];
		}
	}
	for (int i = 0; i < addCount; i++) {
		COrder orderAdd;
		orderAdd.orderId = Json2CString(jsnOrders[i]["id"]);
		orderAdd.hostname = Json2CString(jsnOrders[i]["hostname"]);
		m_aryOrder[i + remainCount] = orderAdd;
	}
	m_nSize = remainCount + addCount;
	m_nIndex = m_nSize - 1;
	/*m_nIndex = remainCount;*/
}

void CPrestaOrderArray::AddHead(Json::Value jsnOrders) {
	int deleteCount, remainCount, addCount;
	if (jsnOrders.size() == 0) {
		return;
	}
	addCount = MAX_SIZE > jsnOrders.size() ? jsnOrders.size() : MAX_SIZE;
	deleteCount = (m_nSize + addCount) - MAX_SIZE;
	if (deleteCount <= 0) {
		deleteCount = 0;
	}
	remainCount = m_nSize - deleteCount;

	if (deleteCount > 0) {
		for (int i = remainCount - 1; i >= 0; i--) {
			m_aryOrder[i + deleteCount] = m_aryOrder[i];
		}
	}
	for (int i = 0; i < addCount; i++) {
		COrder orderAdd;
		orderAdd.orderId = Json2CString(jsnOrders[i]["id"]);
		orderAdd.hostname = Json2CString(jsnOrders[i]["hostname"]);
		m_aryOrder[i] = orderAdd;
	}
	m_nSize = remainCount + addCount;
	m_nIndex = addCount - 1;
}