#include <stdlib.h>
#include "SQL.h"

CSQL::CSQL(const char *filename)
{
	m_DatabaseName = filename;
	m_DB = 0;
	m_Data.clear();
}

bool CSQL::Open()
{	
	if (sqlite3_open(m_DatabaseName, &m_DB))
	{
		dbg_msg("SQL", "Database don't open: %s", sqlite3_errmsg(m_DB));
		return false;
	}
	return true;
}

static int callback(void *pData, int argc, char **ppArgv, char **ppColName)
{
	CSQL *pThis = (CSQL *)pData;
	for(int i = 0; i < argc; i++)
	{
		CSQL::SData Data = CSQL::SData();
		str_copy(Data.ColName, ppColName[i], sizeof(Data.ColName));
		str_copy(Data.Value, ppArgv[i], sizeof(Data.Value));
		pThis->m_Data.add(Data);
	}
	return 0;
}

bool CSQL::Query(const char *query)
{
	char *err = 0;
	if (sqlite3_exec(m_DB, query, callback, this, &err))
	{
		dbg_msg("SQL", "Query error: %s", err);
		sqlite3_free(err);
		return false;
	}
	return true;
}

int CSQL::GetInt(const char *ColName)
{
	int value = -1;
	bool find=false;
	for(int i = 0; i < m_Data.size(); i++)
	{
		if(str_comp(m_Data[i].ColName, ColName) == 0)
		{
			find = true;
			value = atoi(m_Data[i].Value);
			m_Data.remove_index(i);
			break;
		}
	}
	if(find)
		return value;
	else
		return -1;
}

bool CSQL::GetBool(const char *ColName)
{
	bool value = false;
	bool find=false;
	for(int i = 0; i < m_Data.size(); i++)
	{
		if(str_comp(m_Data[i].ColName, ColName) == 0)
		{
			find = true;
			value = (atoi(m_Data[i].Value) == 1);
			m_Data.remove_index(i);
			break;
		}
	}
	if(find)
		return value;
	else
		return NULL;
}

const char *CSQL::GetString(const char *ColName)
{
	const char *value = nullptr;
	bool find=false;
	for(int i = 0; i < m_Data.size(); i++)
	{
		if(str_comp(m_Data[i].ColName, ColName) == 0)
		{
			find = true;
			value = m_Data[i].Value;
			m_Data.remove_index(i);
			break;
		}
	}
	if(find)
		return value;
	else
		return NULL;
}

void CSQL::Close()
{
	m_Data.clear();
}