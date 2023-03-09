
#define CGAME_API_VERSION   1

typedef struct edict_s edict_t;

//
// functions provided by the main engine
//
typedef struct {
	// special messages
	void(*q_printf(1, 2) dprintf)(const char *fmt, ...);
	void(*q_noreturn q_printf(1, 2) error)(const char *fmt, ...);
} cgame_import_t;

//
// functions exported by the game subsystem
//
typedef struct {
	int         apiversion;

	// the init function will only be called when a game starts,
	// not each time a level is loaded.  Persistant data for clients
	// and the server can be allocated in init
	void(*Init)(void);
	void(*Shutdown)(void);


} cgame_export_t;

extern cgame_export_t *cge;
typedef cgame_export_t *(*cgame_entry_t)(cgame_import_t *);

void CG_InitGameProgs(void);







