// dllmain.h : Declaration of module class.

class COutloukModule : public ATL::CAtlDllModuleT< COutloukModule >
{
public :
	DECLARE_LIBID(LIBID_OutloukLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_OUTLOUK, "{2B2873E8-0FCC-4EB4-8F42-2931B47D025B}")
};

extern class COutloukModule _AtlModule;
