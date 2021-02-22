#pragma once

#include "MemoryPool.h"
#include "Compare.h"

template <typename T>
class CLinkedList : public CMemPoolNode<CLinkedList<T>>
{
protected:
	T m_data;
	CLinkedList<T> *m_pNext;

public:
	CLinkedList(const T &data, CLinkedList<T> *pNext = NULL);
	~CLinkedList();

	T &GetData() { return m_data; }
	const T &GetData() const { return m_data; }
	void SetData(const T &data) { m_data = data; }

	CLinkedList<T> *GetNext() const { return m_pNext; }
	void SetNext(CLinkedList<T> *pNode) { m_pNext = pNode; }

	template <typename Compare>
	void InsertOrdered(const T &data)
	{
		Compare comp;
		if(!m_pNext)
			m_pNext = new CLinkedList<T>(data);
		else
		{
			CLinkedList<T> *pNext = this;
			while(NULL != pNext->GetNext() && comp(pNext->GetNext()->GetData(), data))
				pNext = pNext->GetNext();
			pNext->SetNext(new CLinkedList<T>(data, pNext->GetNext()));
		}
	}

	template <typename Compare>
	void InsertOrdered(CLinkedList<T> *pEntry)
	{
		Compare comp;
		if(!m_pNext)
		{
			m_pNext = pEntry;
			pEntry->SetNext(NULL);
		}
		else
		{
			CLinkedList<T> *pNext = this;
			while(NULL != pNext->GetNext() && comp(pNext->GetNext()->GetData(), pEntry->GetData()))
				pNext = pNext->GetNext();
			pEntry->SetNext(pNext->GetNext());
			pNext->SetNext(pEntry);
		}
	}

	template <typename Compare>
	void ReOrder(CLinkedList<T> *pNode)
	{
		Compare comp;
		CLinkedList<T> *pCurNode = this;
		while(pCurNode->GetNext() && pCurNode->GetNext() != pNode && comp(pCurNode->GetNext()->GetData(), pNode->GetData()))
		{
			pCurNode = pCurNode->GetNext();
		}

		if(pCurNode->GetNext() == pNode)
		{
			pCurNode->SetNext(pNode->GetNext());
			while(pCurNode->GetNext() && comp(pCurNode->GetNext()->GetData(), pNode->GetData()))
			{
				pCurNode = pCurNode->GetNext();
			}

			pNode->SetNext(pCurNode->GetNext());
			pCurNode->SetNext(pNode);
		}
		else if(comp(pCurNode->GetNext()->GetData(), pNode->GetData()))
		{
			CLinkedList<T> *pNext = pNode->GetNext();
			pNode->SetNext(pCurNode->GetNext());
			pCurNode->SetNext(pNode);
			pCurNode = pNode;

			while(pCurNode->GetNext() != pNode)
			{
				pCurNode = pCurNode->GetNext();
			}

			pCurNode->SetNext(pNext);
		}
		else
		{
			throw "Not in list!!";
		}
	}
};

template <typename T>
CLinkedList<T>::CLinkedList(const T &data, CLinkedList<T> *pNext /* = NULL */)
: m_data(data), m_pNext(pNext)
{
}

template <typename T>
CLinkedList<T>::~CLinkedList()
{
}
