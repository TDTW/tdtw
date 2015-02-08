#include <sqlite3.h>

class CSQL
{
public:
	CSQL(const char *filename);
	bool Open();
	bool Query(const char *query);
	void Close();
private:
	const char *m_DatabaseName;
	sqlite3 *m_DB;
};