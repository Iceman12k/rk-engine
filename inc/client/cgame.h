#define CGAME_API_VERSION   1

typedef struct {
	server_frame_t 	frame;
	server_frame_t 	oldframe;

	int				servertime;
	int				serverdelta;
	int				time;
	float			lerpfrac;
} cgame_state_t;

//
// functions provided by the main engine
//
typedef struct {
	// special messages
	void	(*q_printf(1, 2) dprintf)(const char *fmt, ...);
	void	(*q_noreturn q_printf(1, 2) error)(const char *fmt, ...);
	void	*(*GetExtension)(const char *name);
} cgame_import_t;

//
// functions exported by the game subsystem
//
typedef struct {
	int         apiversion;

	// the init function will only be called when a game starts,
	// not each time a level is loaded.  Persistant data for clients
	// and the server can be allocated in init
	void	(*Init)(void);
	void	(*Shutdown)(void);
	void	*(*GetExtension)(const char *name);
} cgame_export_t;

typedef struct {
	void	(*UI_Render)(vec2_t screensize);
} cgame_export_extensions_t;

extern cgame_export_t *cge;
extern cgame_export_extensions_t cge_e;
extern cgame_state_t cgcl;
typedef cgame_export_t *(*cgame_entry_t)(cgame_import_t *);

// cgame.c
void CG_InitGameProgs(void);
void CG_ShutdownGameProgs(void);

// screen.c
void CG_R_DrawStretchPic(int x, int y, int w, int h, const char *name);
void CG_R_DrawString(int x, int y, int flags, size_t maxChars, const char *string);
void CG_R_SetClipArea(int x, int y, int w, int h);
void CG_R_ResetClipArea(void);