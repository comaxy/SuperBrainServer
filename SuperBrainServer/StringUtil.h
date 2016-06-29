#include <atlstr.h>
#include <string>

class StringUtil
{
public:
	static CString Utf8ToCString(const std::string& utf8str);
	static std::string CStringToUtf8(const CString& cstr);
};
