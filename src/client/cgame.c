#include "client.h"
#include "client/cgame.h"

static void *cgame_library;
cgame_export_t *cge;
cgame_export_extensions_t cge_e;
cgame_state_t cgcl;


static void PF_dprintf(const char *fmt, ...)
{
	char        msg[MAXPRINTMSG];
	va_list     argptr;

	va_start(argptr, fmt);
	Q_vsnprintf(msg, sizeof(msg), fmt, argptr);
	va_end(argptr);

	Com_Printf("%s", msg);
}

static q_noreturn void PF_error(const char *fmt, ...)
{
	char        msg[MAXERRORMSG];
	va_list     argptr;

	va_start(argptr, fmt);
	Q_vsnprintf(msg, sizeof(msg), fmt, argptr);
	va_end(argptr);

	Com_Error(ERR_DROP, "Game Error: %s", msg);
}

static void *PF_CG_GetExtension(const char *name)
{
	if (!name)
        return NULL;
	Com_Printf("Engine: PF_CG_GetExtension for %s\n", name);
	if (!Q_stricmp(name, "R_DRAWSTRETCHPIC"))
		return CG_R_DrawStretchPic;
	if (!Q_stricmp(name, "R_DRAWSTRING"))
		return CG_R_DrawString;
	if (!Q_stricmp(name, "CG_CL_GAMESTATE"))
		return &cgcl;
	if (!Q_stricmp(name, "R_SETCLIPAREA"))
		return CG_R_SetClipArea;
	if (!Q_stricmp(name, "R_RESETCLIPAREA"))
		return CG_R_ResetClipArea;
	Com_Printf("Engine: Extension not found.\n");
	return NULL;
}

static float PF_ReadFloat(void)
{
	float f;
	int tp;
	tp = MSG_ReadLong();
    memcpy(&f, &tp, sizeof(f));

	return f;
}

static int PF_CG_modelindex(const char *modname)
{
	char *name;
	for (int i = 2; i < cl.csr.max_models; i++) {
		name = cl.configstrings[cl.csr.models + i];
		if (!name[0] && i != MODELINDEX_PLAYER) {
			break;
		}
		if (Q_stricmp(name, modname))
			continue;
		return i;
	}
	return 0;
}

static const cgame_import_t cgame_import = {
	.dprintf = PF_dprintf,
	.error = PF_error,
	.GetExtension = PF_CG_GetExtension,
	
	.modelindex = PF_CG_modelindex,

    .ReadChar = MSG_ReadChar,
    .ReadByte = MSG_ReadByte,
    .ReadShort = MSG_ReadShort,
	.ReadWord = MSG_ReadWord,
    .ReadLong = MSG_ReadLong,
    .ReadFloat = PF_ReadFloat,
    .ReadString = MSG_ReadString,
    .ReadPosition = MSG_ReadPos,
    .ReadDir = MSG_ReadDir,

	.trace = CL_Trace,
	.pointcontents = CL_PointContents,
};


/*
===============
CG_ShutdownGameProgs
Called when either the entire server is being killed, or
it is changing to a different game directory.
===============
*/
void CG_ShutdownGameProgs(void)
{
	memset(&cge_e, 0, sizeof(cge_e)); // clear extension pointers
	memset(&cgcl, 0, sizeof(cgcl)); // clear cgame state
	if (cge) {
		cge->Shutdown();
		cge = NULL;
	}
	if (cgame_library) {
		Sys_FreeLibrary(cgame_library);
		cgame_library = NULL;
	}
	Cvar_Set("cg_features", "0");

	//Z_LeakTest(TAG_FREE);
}

static void *CG_LoadGameLibraryFrom(const char *path)
{
	void *entry;

	entry = Sys_LoadLibrary(path, "GetCGameAPI", &cgame_library);
	if (!entry)
		Com_EPrintf("Failed to load cgame library: %s\n", Com_GetLastError());
	else
		Com_Printf("Loaded cgame library from %s\n", path);

	return entry;
}

static void *CG_LoadGameLibrary(const char *libdir, const char *gamedir)
{
	char path[MAX_OSPATH];

	if (Q_concat(path, sizeof(path), libdir,
		PATH_SEP_STRING, gamedir, PATH_SEP_STRING,
		"cgame" CPUSTRING LIBSUFFIX) >= sizeof(path)) {
		Com_EPrintf("CGame library path length exceeded\n");
		return NULL;
	}

	if (os_access(path, X_OK)) {
		Com_Printf("Can't access %s: %s\n", path, strerror(errno));
		return NULL;
	}

	return CG_LoadGameLibraryFrom(path);
}

/*
===============
CG_InitGameProgs
Init the game subsystem for a new map
===============
*/
void CG_InitGameProgs(void)
{
	cgame_import_t   import;
	cgame_entry_t    entry = NULL;

	// unload anything we have now
	CG_ShutdownGameProgs();

	// for debugging or `proxy' mods
	if (sys_forcegamelib->string[0])
		entry = CG_LoadGameLibraryFrom(sys_forcegamelib->string);

	// try game first
	if (!entry && fs_game->string[0]) {
		if (sys_homedir->string[0])
			entry = CG_LoadGameLibrary(sys_homedir->string, fs_game->string);
		if (!entry)
			entry = CG_LoadGameLibrary(sys_libdir->string, fs_game->string);
	}

	// then try baseq2
	if (!entry) {
		if (sys_homedir->string[0])
			entry = CG_LoadGameLibrary(sys_homedir->string, BASEGAME);
		if (!entry)
			entry = CG_LoadGameLibrary(sys_libdir->string, BASEGAME);
	}

	// all paths failed
	if (!entry)
		Com_Error(ERR_DROP, "Failed to load cgame library");

	// load a new game dll
	import = cgame_import;

	cge = entry(&import);
	if (!cge) {
		Com_Error(ERR_DROP, "CGame library returned NULL exports");
	}

	if (cge->apiversion != CGAME_API_VERSION) {
		Com_Error(ERR_DROP, "CGame library is version %d, expected %d",
			cge->apiversion, CGAME_API_VERSION);
	}

	// initialize
	cge->Init();

	memset(&cge_e, 0, sizeof(cge_e));
	memset(&cgcl, 0, sizeof(cgcl));
	cgcl.num_entityStates = sizeof(cl.entityStates) / sizeof(cl.entityStates[0]);
	cgcl.entityStates = cl.cg_entityStates;
	if (cge->GetExtension)
	{
		cge_e.UI_Render = cge->GetExtension("UI_RENDER");
		cge_e.CG_ReadDeltaEntity = cge->GetExtension("READDELTAENTITY");
		cge_e.CG_ReadDeltaPlayerState = cge->GetExtension("READDELTAPLAYERSTATE");
		cge_e.CG_RunPrediction = cge->GetExtension("RUNPREDICTION");
		cge_e.CG_FinalizeFrame = cge->GetExtension("FINALIZEFRAME");
	}

	/*
	// sanitize edict_size
	if (cge->edict_size < sizeof(edict_t) || cge->edict_size >(unsigned)INT_MAX / MAX_EDICTS) {
		Com_Error(ERR_DROP, "Game library returned bad size of edict_t");
	}
	*/
}
