#ifndef _KS_MULTI_STRING_H
#define _KS_MULTI_STRING_H

#include <string>
#include <vector>
#include <algorithm>
using namespace  std;

#include <Windows.h>

// 多字符串类，实现多字符串缓冲区的增删改


template<class ElemType>
class CKsMultiString{
public:
	CKsMultiString();
	~CKsMultiString();

public:
	// 设置多字符串的字符缓冲区, 缓冲区长度按字节大小
	bool SetBuffer(const ElemType* npMultiStrBuffer, size_t nuBufferSize);

	// 获取多字符串的缓冲区
	const ElemType* GetBuffer();	
	
	// 获取多字符串缓冲区的字节长度
	size_t GetSize();

	// 插入字符串到指定位置
	bool Insert(size_t nuIndex, const ElemType* nswStrValue);
	
	// 附加字符串到末尾
	bool Append(const ElemType* nswStrValue);	
	
	// 删除字符串
	bool Remove(const ElemType* nswStrValue);	
	
	// 清空所有字符串
	bool Clear();

	// 获取字符串个数
	size_t GetCount()
	{
		return moStringVec.size();
	}

	// 获取指定索引的字符串
	const ElemType* operator[](size_t nuIndex)
	{
		if ( nuIndex >= GetCount())
		{
			return NULL;
		}		
		return moStringVec[nuIndex].c_str();
	}

	// 查找字符串，返回索引，返回-1表示不存在
	int Find(const ElemType* nswStrValue)
	{
		size_t luIndex = 0;
		for (luIndex = 0; luIndex < moStringVec.size(); luIndex++)
		{
			if (moStringVec[luIndex].compare(nswStrValue) == 0)
			{
				return (int)luIndex;
			}
		}
		return -1;
	}

private:
	ElemType* mpDataBuffer;
	bool mbDirty; // 标志位，标识是否需要重建DataBuffer

	typedef basic_string<ElemType> KsStringType;
	vector<KsStringType> moStringVec;

};


template<class ElemType>
CKsMultiString<ElemType>::CKsMultiString()
{
	mpDataBuffer = NULL;
	mbDirty = true;
	moStringVec.empty();
}

template<class ElemType>
CKsMultiString<ElemType>::~CKsMultiString()
{
	if (mpDataBuffer != NULL)
	{
		delete mpDataBuffer;
		mpDataBuffer = NULL;
	}
	moStringVec.clear();
}

template<class ElemType>
bool CKsMultiString<ElemType>::SetBuffer(const ElemType* npMultiStrBuffer, size_t nuBufferSize)
{
	if (npMultiStrBuffer == NULL)
	{
		return false;
	}

	// 解析缓冲区
	size_t luTotalLen = 0;		
	ElemType* lpBuffer = (ElemType*)npMultiStrBuffer;
	while((luTotalLen < nuBufferSize) && *lpBuffer)
	{
		KsStringType lswElemObject = lpBuffer;
		moStringVec.push_back(lswElemObject);

		size_t luLen = lswElemObject.length() + sizeof('\0');
		lpBuffer += luLen;
		luTotalLen += luLen * sizeof(ElemType);
	}

	mbDirty = true;
	return true;
}

template<class ElemType>
const ElemType* CKsMultiString<ElemType>::GetBuffer()
{
	ElemType* lpBuffer = NULL; 
	size_t luBufferSize = 0;

	do 
	{
		// 检查标志位，如果为false，则表示不需要重建DataBuffer
		if (mbDirty == false)
		{
			break;
		}

		if (mpDataBuffer != NULL)
		{
			delete mpDataBuffer;
			mpDataBuffer = NULL;
		}
		
		// 重建DataBuffer
		luBufferSize = GetSize();
		mpDataBuffer = (ElemType*)new char[luBufferSize];
		if (mpDataBuffer == NULL)
		{
			break;
		}
		memset(mpDataBuffer, 0, luBufferSize);

		lpBuffer = mpDataBuffer;
		size_t luCurItemLen = 0;
		// 拼接buffer的内容
		for(size_t luIndex = 0; luIndex < moStringVec.size(); luIndex++)
		{
			luCurItemLen = moStringVec[luIndex].length();
			RtlCopyMemory(lpBuffer, moStringVec[luIndex].c_str(), luCurItemLen*sizeof(ElemType));
			lpBuffer += luCurItemLen + sizeof('\0');
		}
		
		mbDirty = false;
	} while (false);

	return mpDataBuffer;
}

template<class ElemType>
size_t CKsMultiString<ElemType>::GetSize()
{

	size_t luBufferSize = 0;

	for (size_t luIndex = 0; luIndex < moStringVec.size(); luIndex++)
	{
		luBufferSize += moStringVec[luIndex].length()*sizeof(ElemType);
		luBufferSize += sizeof(ElemType);
	}
	luBufferSize += sizeof(ElemType);

	return luBufferSize;
}

template<class ElemType>
bool CKsMultiString<ElemType>::Insert(size_t nuIndex, const ElemType* nswStrValue)
{

	if (nswStrValue == NULL)
	{
		return false;
	}

	if (nuIndex > moStringVec.size())
	{
		return false;
	}

	KsStringType lswTmpString = nswStrValue;
	if (lswTmpString.empty())
	{
		return false;
	}

	moStringVec.insert(moStringVec.begin() + nuIndex, nswStrValue);

	mbDirty = true;
	return true;
}

template<class ElemType>
bool CKsMultiString<ElemType>::Append(const ElemType* nswStrValue)
{
	if (nswStrValue == NULL)
	{
		return false;
	}
	
	KsStringType lswTmpString = nswStrValue;
	if (lswTmpString.empty())
	{
		return false;
	}

	moStringVec.push_back(nswStrValue);
	
	mbDirty = true;
	return true;	
}

template<class ElemType>
bool CKsMultiString<ElemType>::Remove(const ElemType* nswStrValue)
{

	if (nswStrValue == NULL)
	{
		return false;
	}

	vector<KsStringType>::iterator lpElemItr = std::remove(moStringVec.begin(), moStringVec.end(), nswStrValue);
	if (lpElemItr != moStringVec.end())
	{
		moStringVec.erase(lpElemItr, moStringVec.end());
	}

	mbDirty = true;
	return true;
}

template<class ElemType>
bool CKsMultiString<ElemType>::Clear()
{

	moStringVec.clear();

	if (mpDataBuffer != NULL)
	{
		delete mpDataBuffer;
		mpDataBuffer = NULL;
	}

	mbDirty = true;
	return true;
}




#endif