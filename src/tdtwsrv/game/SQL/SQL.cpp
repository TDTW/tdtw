#include "SQL.h"
#include <base/system.h>

CSQL::CSQL(const char *filename)
{
	m_DatabaseName = filename;
	m_DB = 0;
}

bool CSQL::Open()
{	
	if (sqlite3_open(m_DatabaseName, &m_DB))
	{
		dbg_msg("SQL", "Database don't open: %s", sqlite3_errmsg(m_DB));
		return false;
	}
}

bool CSQL::Query(const char *query)
{
	char *err = 0;
	if (sqlite3_exec(m_DB, "", 0, 0, &err))
	{
		dbg_msg("SQL", "Query error: %s", err);
		sqlite3_free(err);
		return false;
	}

}

void CSQL::Close()
{
	sqlite3_close(m_DB);
}
