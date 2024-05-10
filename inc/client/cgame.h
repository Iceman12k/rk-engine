#define CGAME_API_VERSION   1

typedef struct {
	server_frame_t 	frame;
	server_frame_t 	oldframe;

	pmoveParams_t 	pmp;
	usercmd_t		cmd;
	usercmd_t		cmds[CMD_BACKUP];

	short			predicted_origins[CMD_BACKUP][3];
	vec3_t			localmove;

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

	int		(*modelindex)(const char *name);

	int 	(*ReadChar)(void);
	int 	(*ReadByte)(void);
	int 	(*ReadShort)(void);
	int 	(*ReadLong)(void);
	float 	(*ReadFloat)(void);
	void 	(*ReadString)(char *dest, size_t size);
	void 	(*ReadPosition)(vec3_t pos);			// some fractional bits
	void 	(*ReadDir)(vec3_t pos);					// single byte encoded, very coarse

	trace_t (* q_gameabi trace)(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int contentmask);
    int (*pointcontents)(const vec3_t point);
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
	void	(*CG_ReadDeltaEntity)(entity_state_t *to, entity_state_extension_t *ext, int number, uint64_t bits, msgEsFlags_t flags);
	void	(*CG_RunPrediction)(pmove_t *pm, int *current, int *ack, int *frame);
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