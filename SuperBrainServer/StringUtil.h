#pragma once

#include <atlstr.h>
#include <string>

class StringUtil
{
public:
	static CString Utf8ToCString(const std::string& utf8str);
	static std::string CStringToUtf8(const CString& cstr);

	static CString multiByteToCString(const std::string& mbstr);
	static std::string CStringToMultiByte(const CString& cstr);

	static CString GetGUID();
};
