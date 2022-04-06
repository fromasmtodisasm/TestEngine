#pragma once

class CObjectManager
{
  public:
	IStatObj* LoadStatObj(const char* szFileName, const char* szGeomName = 0);

  private:
	typedef std::map<string, _smart_ptr<CStatObj>, stl::less_stricmp<string>> ObjectsMap;
	ObjectsMap													   m_nameToObjectMap;

	typedef std::set<_smart_ptr<CStatObj>> LoadedObjects;
	LoadedObjects				m_lstLoadedObjects;
};