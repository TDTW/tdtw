#include <sqlite3.h>
#include <base/tl/array.h>

class CSQL
{

public:
	CSQL(const char *filename);
	bool Open();
	bool Query(const char *query);
	void Close();
	int GetInt(const char *ColName);
	bool GetBool(const char *ColName);
	const char *GetString(const char *ColName);

	struct SData
	{
		char *ColName;
		char *Value;
	};

	array<SData> m_Data;
private:
	const char *m_DatabaseName;
	sqlite3 *m_DB;
};