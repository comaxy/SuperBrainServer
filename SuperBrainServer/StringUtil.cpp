#include "StringUtil.h"

CString StringUtil::Utf8ToCString(const std::string& utf8Str)
{
#ifdef _UNICODE
	int nLengthResult = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), utf8Str.length(), NULL, 0);
	WCHAR *bufResult = new WCHAR[nLengthResult]();
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), utf8Str.length(), bufResult, nLengthResult);
	CString strResult(bufResult, nLengthResult);
	delete[] bufResult; bufResult = 0;
	return strResult;
#else
	int nLengthWide = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), utf8Str.length(), NULL, 0);
	WCHAR *bufWide = new WCHAR[nLengthWide]();
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), utf8Str.length(), bufWide, nLengthWide);
	int nLengthResult = WideCharToMultiByte(CP_ACP, 0, bufWide, nLengthWide, NULL, 0, NULL, NULL);
	CHAR *bufResult = new CHAR[nLengthResult]();
	WideCharToMultiByte(CP_ACP, 0, bufWide, nLengthWide, bufResult, nLengthResult, NULL, NULL);
	CString strResult(bufResult, nLengthResult);
	delete[] bufResult; bufResult = 0;
	delete[] bufWide; bufWide = 0;
	return strResult;
#endif
}

std::string StringUtil::CStringToUtf8(const CString& cstr)
{
#ifdef _UNICODE
	int nBufSizeUTF8 = WideCharToMultiByte(CP_UTF8, 0, cstr, cstr.GetLength(), NULL, 0, NULL, NULL);
	CHAR *pBufUTF8 = new CHAR[nBufSizeUTF8];
	memset(pBufUTF8, 0, nBufSizeUTF8);
	WideCharToMultiByte(CP_UTF8, 0, cstr, cstr.GetLength(), pBufUTF8, nBufSizeUTF8, NULL, NULL);
	std::string result(pBufUTF8, nBufSizeUTF8);
	delete[] pBufUTF8; pBufUTF8 = NULL;
	return result;
#else
	int nBufSizeW = MultiByteToWideChar(CP_ACP, 0, cstr, cstr.GetLength(), NULL, 0);
	WCHAR *pBufW = new WCHAR[nBufSizeW]();
	MultiByteToWideChar(CP_ACP, 0, cstr, cstr.GetLength(), pBufW, nBufSizeW);
	int nBufSizeUTF8 = WideCharToMultiByte(CP_UTF8, 0, pBufW, nBufSizeW, NULL, 0, NULL, NULL);
	CHAR *pBufUTF8 = new CHAR[nBufSizeUTF8]();
	WideCharToMultiByte(CP_UTF8, 0, pBufW, nBufSizeW, pBufUTF8, nBufSizeUTF8, NULL, NULL);
	std::string result(pBufUTF8, nBufSizeUTF8);
	delete[] pBufUTF8; pBufUTF8 = NULL;
	delete[] pBufW; pBufW = NULL;
	return result;
#endif
}

CString StringUtil::multiByteToCString(const std::string& mbstr)
{
#ifdef _UNICODE
	int nLengthResult = MultiByteToWideChar(CP_ACP, 0, mbstr.c_str(), mbstr.length(), NULL, 0);
	WCHAR *bufResult = new WCHAR[nLengthResult]();
	MultiByteToWideChar(CP_ACP, 0, mbstr.c_str(), mbstr.length(), bufResult, nLengthResult);
	CString strResult(bufResult, nLengthResult);
	delete[] bufResult; bufResult = 0;
	return strResult;
#else
	return CString(mbstr.c_str());
#endif
}

std::string StringUtil::CStringToMultiByte(const CString& cstr)
{
#ifdef _UNICODE
	int nBufSizeMb = WideCharToMultiByte(CP_ACP, 0, cstr, cstr.GetLength(), NULL, 0, NULL, NULL);
	CHAR *pBufMb = new CHAR[nBufSizeMb];
	memset(pBufMb, 0, nBufSizeMb);
	WideCharToMultiByte(CP_ACP, 0, cstr, cstr.GetLength(), pBufMb, nBufSizeMb, NULL, NULL);
	std::string result(pBufMb, nBufSizeMb);
	delete[] pBufMb; pBufMb = NULL;
	return result;
#else
	return std::string(cstr.GetBuffer(), cstr.GetLength());
#endif
}
