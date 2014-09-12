#include "lua_fatfs.h"

static const char *fatfs_err2str(FRESULT fret)
{
	const char *serr;

	switch( fret )
	{
	case FR_OK:
		serr = "(0) Succeeded";
		break;
	case FR_DISK_ERR:
		serr = "(1) A hard error occurred in the low level disk I/O layer";
		break;
	case FR_INT_ERR:
		serr = "(2) Assertion failed";
		break;
	case FR_NOT_READY:
		serr = "(3) The physical drive cannot work";
		break;
	case FR_NO_FILE:
		serr = "(4) Could not find the file";
		break;
	case FR_NO_PATH:
		serr = "(5) Could not find the path";
		break;
	case FR_INVALID_NAME:
		serr = "(6) The path name format is invalid";
		break;
	case FR_DENIED:
		serr = "(7) Access denied due to prohibited access or directory full";
		break;
	case FR_EXIST:
		serr = "(8) Access denied due to prohibited access";
		break;
	case FR_INVALID_OBJECT:
		serr = "(9) The file/directory object is invalid";
		break;
	case FR_WRITE_PROTECTED:
		serr = "(10) The physical drive is write protected";
		break;
	case FR_INVALID_DRIVE:
		serr = "(11) The logical drive number is invalid";
		break;
	case FR_NOT_ENABLED:
		serr = "(12) The volume has no work area";
		break;
	case FR_NO_FILESYSTEM:
		serr = "(13) There is no valid FAT volume";
		break;
	case FR_MKFS_ABORTED:
		serr = "(14) The f_mkfs() aborted due to any parameter error";
		break;
	case FR_TIMEOUT:
		serr = "(15) Could not get a grant to access the volume within defined period";
		break;
	case FR_LOCKED:
		serr = "(16) The operation is rejected according to the file sharing policy";
		break;
	case FR_NOT_ENOUGH_CORE:
		serr = "(17) LFN working buffer could not be allocated";
		break;
	case FR_TOO_MANY_OPEN_FILES:
		serr = "(18) Number of open files > _FS_SHARE";
		break;
	case FR_INVALID_PARAMETER:
		serr = "(19) Given parameter is invalid";
		break;
	default:
		serr = "No error number";
		break;
	}

	return serr;
}

typedef struct LoadF
{
	int n;  /* number of pre-read characters */
	FIL f;  /* file being read */
	char buff[LUAL_BUFFERSIZE];  /* area for reading file */
} LoadF;

static const char *getF (lua_State *L, void *ud, size_t *size)
{
	LoadF *lf = (LoadF *)ud;
	(void)L;  /* not used */
	unsigned int n;

	if (lf->n > 0)
	{  /* are there pre-read characters to be read? */
		*size = lf->n;  /* return them (chars already in buffer) */
		lf->n = 0;  /* no more pre-read characters */
	}
	else
	{  /* read a block from file */
		/* 'fread' can return > 0 *and* set the EOF flag. If next call to
		 *        'getF' called 'fread', it might still wait for user input.
		 *               The next check avoids this problem. */
		if (f_eof(&lf->f)) return NULL;
		f_read(&lf->f, lf->buff, sizeof(lf->buff), &n);
		*size = n;
	}
	return lf->buff;
}

static int errfile (lua_State *L, const char *what, int fnameindex, FRESULT ret)
{
	const char *serr = fatfs_err2str(ret);
	const char *filename = lua_tostring(L, fnameindex) + 1;

	lua_pushfstring(L, "cannot %s %s: %s", what, filename, serr);

	lua_remove(L, fnameindex);
	return LUA_ERRFILE;
}

static int skipBOM (LoadF *lf)
{
	const char *p = "\xEF\xBB\xBF";  /* Utf8 BOM mark */
	int c;
	FRESULT fret;
	unsigned int n;

	lf->n = 0;
	do
	{
		fret = f_read(&lf->f, &c, 1, &n);
		if(FR_OK != fret) c = EOF;

		if (c == EOF || c != *(const unsigned char *)p++) return c;
		lf->buff[lf->n++] = c;  /* to be read by the parser */
	} while (*p != '\0');
	lf->n = 0;  /* prefix matched; discard it */

	fret = f_read(&lf->f, &c, 1, &n);
	if(FR_OK != fret) c = EOF;

	return c;  /* return next character */
}

/*
 ** reads the first character of file 'f' and skips an optional BOM mark
 ** in its beginning plus its first line if it starts with '#'. Returns
 ** true if it skipped the first line.  In any case, '*cp' has the
 ** first "valid" character of the file (after the optional BOM and
 ** a first-line comment).
 */
static int skipcomment (LoadF *lf, int *cp)
{
	int c = *cp = skipBOM(lf);
	unsigned int n;
	FRESULT fret;

	if (c == '#')
	{
		/* first line is a comment (Unix exec. file)? */
		do
		{  /* skip first line */
			fret = f_read(&lf->f, &c, 1, &n);
			if(FR_OK != fret)
				c = EOF;
		} while (c != EOF && c != '\n') ;
		fret = f_read(&lf->f, &c, 1, &n);
		return 1;  /* there was a comment */
	}
	else
		return 0;  /* no comment */
}

int fatfs_loadfilex (lua_State *L, const char *filename, const char *mode)
{
	LoadF lf;
	int status, readstatus;
	int c;
	int fnameindex = lua_gettop(L) + 1;  /* index of filename on the stack */
	FRESULT fret;

	if (filename == NULL)
	{
		return LUA_ERRFILE;
	}
	else
	{
		lua_pushfstring(L, "@%s", filename);
		fret = f_open(&lf.f, filename, FA_READ);
		if (FR_OK != fret)
			return errfile(L, "open", fnameindex, fret);
	}

	if (skipcomment(&lf, &c))  /* read initial portion */
		lf.buff[lf.n++] = '\n';  /* add line to correct line numbers */

	if (c == LUA_SIGNATURE[0] && filename)
	{	/* binary file? */
		fret = f_close(&lf.f);
		if(FR_OK != fret)
			return errfile(L, "close", fnameindex, fret);

		fret = f_open(&lf.f, filename, FA_READ);
		if (FR_OK != fret)
			return errfile(L, "open", fnameindex, fret);

		skipcomment(&lf, &c);  /* re-read initial portion */
	}
	if (c != EOF)
		lf.buff[lf.n++] = c;  /* 'c' is the first character of the stream */

	status = lua_load(L, getF, &lf, lua_tostring(L, -1), mode);
	readstatus = f_error(&lf.f);

	if (filename)
		f_close(&lf.f);  /* close file (even in case of errors) */

	if (readstatus)
	{
		lua_settop(L, fnameindex);  /* ignore results from `lua_load' */
		return errfile(L, "read", fnameindex, (FRESULT)lf.f.err);
	}
	lua_remove(L, fnameindex);

	return status;
}

static int fatfs_loadlib(lua_State *L)
{
	const char *name = luaL_checkstring(L, -1);

	if( fatfs_dofile(L, name) )
	{	// error
		lua_pushnil(L);
	}
	else
	{	// OK
		lua_pushboolean(L, 1);
	}

	return 1;
}

static const luaL_Reg fatfslib[] =
{
	{"loadlib", fatfs_loadlib},
	{NULL, NULL}
};

/*
		print(\"require:\", name); \
*/

static char *const fatfs_lib =
"fatfs_packet_loaded={}; \
function fatfs_require(name) \
	if not fatfs_packet_loaded[name] then \
		fatfs_loadlib(name); \
		fatfs_packet_loaded[name] = true; \
	end \
	return fatfs_packet_loaded[name]; \
end";

LUAMOD_API int open_fatfslib(lua_State *L)
{
	luaL_newlib(L, fatfslib);

	lua_register(L, "fatfs_loadlib", fatfs_loadlib);
	luaL_dostring(L, fatfs_lib);

	return 1;
}

