#include "DbManager.h"
#include "StringUtil.h"
#include "sqlite/sqlite3.h"
#include <atlstr.h>

Sqlite::Sqlite()
	: m_connection(nullptr)
{
}

Sqlite::~Sqlite()
{
	close();
}

void Sqlite::close()
{
	if (m_connection)
	{
		sqlite3_close(m_connection);
		m_connection = nullptr;
	}
}

bool Sqlite::initialize(const std::string& path)
{
	int ret = sqlite3_open(path.c_str(), &m_connection);
	if (ret != SQLITE_OK)
	{
		return false;
	}

	return true;
}

void Sqlite::uninitialize()
{
	close();
}

std::shared_ptr<SqliteQuery> Sqlite::makeQuery()
{
	return std::make_shared<SqliteQuery>(this, m_connection);
}

SqliteQuery::SqliteQuery(Sqlite* database, sqlite3* connection)
	: m_connection(connection)
	, m_stmt(nullptr)
	, m_result()
	, m_iter()
	, m_fetchFirstResult(false)
{
}

bool SqliteQuery::exec(const CString& sql)
{
	if (prepare(sql))
	{
		return exec();
	}
	else
	{
		return false;
	}
}

Variant SqliteQuery::getValueFromColumn(sqlite3_stmt* stmt, int column)
{
	int sqlite3_type = sqlite3_column_type(stmt, column);
	switch (sqlite3_type)
	{
	case SQLITE_INTEGER:
		return Variant(sqlite3_column_int(stmt, column));
	case SQLITE_FLOAT:
		return Variant(sqlite3_column_double(stmt, column));
	case SQLITE_TEXT:
		return Variant(
			StringUtil::Utf8ToCString(std::string(
			reinterpret_cast<const char*>(sqlite3_column_text(stmt, column)),
			sqlite3_column_bytes(stmt, column))));
	case SQLITE_BLOB:
		return Variant(
			reinterpret_cast<const char*>(sqlite3_column_blob(stmt, column)),
			sqlite3_column_bytes(stmt, column));
	default:
		break;
	}

	return Variant();
}

bool SqliteQuery::prepare(const CString& sql)
{
	m_result.clear();
	m_stmt = nullptr;

	int ret = sqlite3_prepare_v2(m_connection,
		StringUtil::CStringToUtf8(sql).c_str(), -1,
		&m_stmt, nullptr);
	if (ret != SQLITE_OK)
	{
		sqlite3_finalize(m_stmt);
		return false;
	}

	return true;
}

void SqliteQuery::bindValue(const CString& key, const Variant& value)
{
	int index = sqlite3_bind_parameter_index(m_stmt, StringUtil::CStringToUtf8(key).c_str());
	bindValue(index, value);
}

void SqliteQuery::bindValue(int i, const Variant& value)
{
	switch (value.type())
	{
	case Variant::TYPE_INT:
		sqlite3_bind_int(m_stmt, i, value.toInt());
		break;
	case Variant::TYPE_DOUBLE:
		sqlite3_bind_double(m_stmt, i, value.toDouble());
		break;
	case Variant::TYPE_TEXT:
		{
			CString text = value.toString();
			std::string utf8Text = StringUtil::CStringToUtf8(text);
			sqlite3_bind_text(m_stmt, i, utf8Text.c_str(), utf8Text.size(), SQLITE_TRANSIENT);
		}
		break;
	case Variant::TYPE_BLOB:
		{
			std::pair<char*, unsigned int> buffer = value.toBlob();
			sqlite3_bind_blob(m_stmt, i, buffer.first, buffer.second, SQLITE_TRANSIENT);
		}
		break;
	default:
		break;
	}
}

bool SqliteQuery::exec()
{
	int ret = sqlite3_step(m_stmt);
	while (ret == SQLITE_ROW)
	{
		Row row;
		int columnCount = sqlite3_column_count(m_stmt);
		for (int column = 0; column < columnCount; ++column)
		{
			RowItem rowItem;
			rowItem.key = StringUtil::Utf8ToCString(sqlite3_column_name(m_stmt, column));
			rowItem.value = getValueFromColumn(m_stmt, column);
			row.push_back(rowItem);
		}
		m_result.push_back(row);
		ret = sqlite3_step(m_stmt);
	}

	sqlite3_finalize(m_stmt);
	m_stmt = nullptr;

	if (!m_result.empty())
	{
		m_fetchFirstResult = true;
	}

	return true;
}

bool SqliteQuery::next()
{
	if (m_result.empty())
	{
		return false;
	}

	if (m_fetchFirstResult)
	{
		m_iter = m_result.begin();
		m_fetchFirstResult = false;
	}
	else
	{
		++m_iter;
	}

	if (m_iter == m_result.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

Variant SqliteQuery::value(const CString& key)
{
	for (auto iterItem = m_iter->begin(); iterItem != m_iter->end(); ++iterItem)
	{
		if (iterItem->key == key)
		{
			return iterItem->value;
		}
	}
	return Variant();
}

Variant SqliteQuery::value(unsigned int i)
{
	if (i >= m_iter->size() || i < 0)
	{
		return Variant();
	}

	return (*m_iter)[i].value;
}

bool DbManager::initialize()
{
	CString exeFilePath;
	GetModuleFileName(NULL, exeFilePath.GetBufferSetLength(MAX_PATH), MAX_PATH);
	CString dbPath = exeFilePath.Left(exeFilePath.ReverseFind(TEXT('\\')) + 1) + TEXT("sb.db");
	if (!m_sqlite->initialize(StringUtil::CStringToUtf8(dbPath)))
	{
		return false;
	}

	std::shared_ptr<SqliteQuery> query = m_sqlite->makeQuery();
	bool ok = false;
	ok = query->exec(CString(
		"CREATE TABLE IF NOT EXISTS "
		"player ("
		"name                  TEXT,"
		"password              TEXT,"
		"PRIMARY KEY (name)"
		")"));
	if (!ok)
	{
		return false;
	}
}

void DbManager::uninitialize()
{
	m_sqlite->uninitialize();
}
