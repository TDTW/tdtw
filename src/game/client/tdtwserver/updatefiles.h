#ifndef GAME_CLIENT_TDTWSERVER_UPDATEFILES_H
#define GAME_CLIENT_TDTWSERVER_UPDATEFILES_H

#include <base/tl/array.h>
#include "tdtwserver.h"

class CUpdateFiles
{
public:
	struct CInfoFiles
	{
		char Name[128];
	};
	CUpdateFiles(class CTDTWServer *Server);
	
	void AddFile(const char *pFile);
	void StartUpdate();
	void EndUpdate();
	bool m_Updating;
	array<CInfoFiles> m_UpdateFiles;

	class CTDTWServer *Server() { return m_Server; }
private:
	class CTDTWServer *m_Server;
};

#endif //GAME_CLIENT_TDTWSERVER_UPDATEFILES_H