#include "DbManager.h"

bool DbManager::initialize()
{
	//// ��ʼ�����ݿ�
	//CString dbPath = exeFilePath.Left(exeFilePath.ReverseFind(TEXT('\\')) + 1) + TEXT("sb.db");
	//sqlite3* db = NULL;
	//sqlite3_open(StringUtil::CStringToUtf8(dbPath).c_str(), &db);

	return true;
}

void DbManager::uninitialize()
{

}