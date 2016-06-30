#pragma once

#include "sqlite/sqlite3.h"
#include <atlstr.h>
#include <memory>
#include <vector>

class SqliteQuery;

class Sqlite
{
public:
	Sqlite();
	virtual ~Sqlite();
	virtual bool initialize(const std::string& path);
	virtual void uninitialize();
	virtual std::shared_ptr<SqliteQuery> makeQuery();
private:
	void close();
private:
	sqlite3* m_connection;
};

class Variant
{
public:
	enum Type
	{
		TYPE_UNKNOWN,
		TYPE_INT,
		TYPE_DOUBLE,
		TYPE_TEXT,
		TYPE_BLOB,
	};

public:
	Variant() { m_type = TYPE_UNKNOWN; }
	Variant(int intValue) { init();  m_type = TYPE_INT; m_intValue = intValue; }
	Variant(double doubleValue) { init();  m_type = TYPE_DOUBLE; m_doubleValue = doubleValue; }
	Variant(const CString& textValue) { init(); m_type = TYPE_TEXT; m_textValue = textValue; }
	Variant(const char* buffer, unsigned int size)
	{
		init();
		m_type = TYPE_BLOB;
		m_blobValue = new char[size]();
		memcpy(m_blobValue, buffer, size);
		m_blobSize = size;
	}

	Variant(const Variant& variant)
	{
		copy(variant);
	}
	const Variant& operator = (const Variant& variant)
	{
		copy(variant);
		return *this;
	}

	Type type() const { return m_type; }

	int toInt() const { return m_intValue; }
	double toDouble() const { return m_doubleValue; }
	CString toString() const { return m_textValue; }
	std::pair<char*, unsigned int> toBlob() const { return std::make_pair(m_blobValue, m_blobSize); }

private:
	void init()
	{
		m_type = TYPE_UNKNOWN;
		m_intValue = 0;
		m_doubleValue = 0.0;
		m_textValue = TEXT("");
		m_blobValue = nullptr;
	}
	void copy(const Variant& variant)
	{
		switch (variant.m_type)
		{
		case TYPE_INT:
			m_type = TYPE_INT;
			m_intValue = variant.m_intValue;
			break;
		case TYPE_DOUBLE:
			m_type = TYPE_DOUBLE;
			m_doubleValue = variant.m_doubleValue;
			break;
		case TYPE_TEXT:
			m_type = TYPE_TEXT;
			m_textValue = variant.m_textValue;
			break;
		case TYPE_BLOB:
			m_type = TYPE_BLOB;
			m_blobValue = new char[variant.m_blobSize]();
			memcpy(m_blobValue, variant.m_blobValue, variant.m_blobSize);
			m_blobSize = variant.m_blobSize;
			break;
		case TYPE_UNKNOWN:
		default:
			break;
		}
	}

private:

	Type m_type;
	int m_intValue;
	double m_doubleValue;
	CString m_textValue;
	char* m_blobValue;
	unsigned int m_blobSize;
};

class SqliteQuery
{
	struct RowItem
	{
		CString key;
		Variant value;
	};

	typedef std::vector<RowItem> Row;
	typedef std::vector<Row> Result;

public:
	SqliteQuery(Sqlite* database, sqlite3* connection);
	virtual ~SqliteQuery() {}
	virtual bool exec(const CString& sql);
	virtual bool prepare(const CString& sql);
	virtual void bindValue(const CString& key, const Variant& value);
	virtual void bindValue(int i, const Variant& value);
	virtual bool exec();
	virtual bool next();
	virtual Variant value(const CString& key);
	virtual Variant value(unsigned int i);

private:
	Variant getValueFromColumn(sqlite3_stmt* stmt, int column);

private:
	sqlite3* m_connection;
	sqlite3_stmt* m_stmt;
	Result m_result;
	Result::const_iterator m_iter;
	bool m_fetchFirstResult;
};

class DbManager
{
public:
	DbManager() : m_sqlite(new Sqlite()) {}
	bool initialize();
	void uninitialize();

	std::shared_ptr<Sqlite> sqlite() const { return m_sqlite; }

private:
	std::shared_ptr<Sqlite> m_sqlite;
};
