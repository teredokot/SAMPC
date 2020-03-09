
#include "main.h"

typedef struct {
	int iRows;
	int iColumns;
	char** pszResults;
	char* szErrMsg;
	int iCurrentRow;
} DBResult;

static DataStructures::Map<int, sqlite3*> g_mapDBs;
static DataStructures::Map<int, DBResult*> g_mapDBResults;

int set_amxstring(AMX* amx, cell amx_addr, const char* source, int max);

// native DB:db_open(name[])
static cell n_db_open(AMX* amx, cell* params)
{
	char* filename = NULL;
	amx_StrParam(amx, params[1], filename);
	if (filename == NULL)
		return 0;

	if (filename[0] == '\0' || filename[0] == '\\' || filename[0] == '/')
		return 0;

	size_t max = strlen(filename);
	for (size_t i = 0; i < max; i++)
	{
		if (filename[i] == '.' && ((i + 1) < max && filename[i + 1] == '.'))
			return 0;

		if (filename[i] == ':')
			return 0;
	}

	char path[MAX_PATH] = { 0 };
#ifdef WIN32
	strcat_s(path, "scriptfiles\\");
	strcat_s(path, filename);
#else
	strcat(path, "scriptfiles/");
	strcat(path, filename);
#endif

	sqlite3* db = NULL;
	if (sqlite3_open(path, &db) != SQLITE_OK)
	{
		if (g_bDBLogging)
			logprintf("[Error] Can't open sqlite database %s.", path);

		return 0;
	}

	int key = 0;
	while (true)
	{
		if (!g_mapDBs.HasEx(key))
		{
			g_mapDBs.Set(key, db);
			break;
		}
		key++;
	}

	return key + 1;
}

// native db_close(DB:db)
static cell n_db_close(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (1 > params[1] || !g_mapDBs.HasEx(key))
	{
		if(g_bDBLogging)
			logprintf("[Error] db_close: invalid database handle %d", params[1]);

		return 0;
	}

	sqlite3* db = g_mapDBs.Get(key);
	int err = sqlite3_close(db);
	g_mapDBs.Delete(key);

	return err == SQLITE_OK;
}

// native DBResult:db_query(DB:db, query[])
static cell n_db_query(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (1 > params[1] || !g_mapDBs.HasEx(key))
	{
		if (g_bDBLogging)
			logprintf("[Error] db_query: invalid database handle %d", params[1]);

		return 0;
	}

	DBResult* result = (DBResult*)calloc(1, sizeof(DBResult));
	if (result == NULL)
		return 0;
	result->iCurrentRow = 0;

	char* query = NULL;
	amx_StrParam(amx, params[2], query);
	if (query == NULL)
		return 0;

	if (g_bDBLogQueries)
		logprintf("[db_log_queries]: %s", query);

	sqlite3* db = g_mapDBs.Get(key);
	if (sqlite3_get_table(db, query, &result->pszResults, &result->iRows,
		&result->iColumns, &result->szErrMsg) != SQLITE_OK)
	{
		if (g_bDBLogging)
			logprintf("[Warning] db_query: Query failed: %s", result->szErrMsg);

		sqlite3_free_table(result->pszResults);
		sqlite3_free(result->szErrMsg);
		free(result);
		return 0;
	}

	key = 0;
	while (true)
	{
		if (!g_mapDBResults.HasEx(key))
		{
			g_mapDBResults.Set(key, result);
			break;
		}
		key++;
	}

	return key + 1;
}

// native db_free_result(DBResult:dbresult)
static cell n_db_free_result(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (1 > params[1] || !g_mapDBResults.HasEx(key))
	{
		if (g_bDBLogging)
			logprintf("[Error] db_free_result(): invalid result handle %d", params[1]);

		return 0;
	}

	DBResult* dbresult = g_mapDBResults.Get(key);

	sqlite3_free_table(dbresult->pszResults);
	sqlite3_free(dbresult->szErrMsg);
	free(dbresult);

	g_mapDBResults.Delete(key);

	return 1;
}

// native db_num_rows(DBResult:dbresult)
static cell n_db_num_rows(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (1 > params[1] || !g_mapDBResults.HasEx(key))
	{
		if (g_bDBLogging)
			logprintf("[Error] db_num_rows(): invalid result handle %d", params[1]);

		return 0;
	}

	return g_mapDBResults.Get(key)->iRows;
}

// native db_next_row(DBResult:dbresult)
static cell n_db_next_row(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (1 > params[1] || !g_mapDBResults.HasEx(key))
	{
		if (g_bDBLogging)
			logprintf("[Error] db_next_row(): invalid result handle %d", params[1]);

		return 0;
	}

	DBResult* dbresult = g_mapDBResults.Get(key);
	if (dbresult->iCurrentRow == (dbresult->iRows - 1))
		return 0;

	dbresult->iCurrentRow++;

	return 1;
}

// native db_num_fields(DBResult:dbresult)
static cell n_db_num_fields(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (1 > params[1] || !g_mapDBResults.HasEx(key))
	{
		if (g_bDBLogging)
			logprintf("[Error] db_num_fields(): invalid result handle %d", params[1]);

		return 0;
	}

	return g_mapDBResults.Get(key)->iColumns;
}

// native db_field_name(DBResult:dbresult, field, result[], maxlength)
static cell n_db_field_name(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (1 > params[1] || !g_mapDBResults.HasEx(key))
	{
		if (g_bDBLogging)
			logprintf("[Error] db_field_name(): invalid result handle %d", params[1]);

		return 0;
	}

	DBResult* dbresult = g_mapDBResults.Get(key);
	int field = params[2];
	if (field < 0 || field >= dbresult->iColumns)
	{
		set_amxstring(amx, params[3], "", params[4]);
		return 0;
	}

	set_amxstring(amx, params[3], dbresult->pszResults[field], params[4]);

	return 1;
}

// native db_get_field(DBResult:dbresult, field, result[], maxlength)
static cell n_db_get_field(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (1 > params[1] || !g_mapDBResults.HasEx(key))
	{
		if (g_bDBLogging)
			logprintf("[Error] db_get_field(): invalid result handle %d", params[1]);

		return 0;
	}

	DBResult* dbresult = g_mapDBResults.Get(key);
	int field = params[2];
	if (field < 0 || field >= dbresult->iColumns)
	{
		set_amxstring(amx, params[3], "", params[4]);
		return 0;
	}

	set_amxstring(amx, params[3], dbresult->pszResults[dbresult->iColumns * (dbresult->iCurrentRow + 1) + field], params[4]);

	return 1;
}

// native db_get_field_assoc(DBResult:dbresult, const field[], result[], maxlength)
static cell n_db_get_field_assoc(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (1 > params[1] || !g_mapDBResults.HasEx(key))
	{
		if (g_bDBLogging)
			logprintf("[Error] db_get_field_assoc(): invalid result handle %d", params[1]);

		return 0;
	}
	
	char* fieldname = NULL;
	amx_StrParam(amx, params[2], fieldname);
	if (fieldname == NULL)
		return 0;

	DBResult* dbresult = g_mapDBResults.Get(key);
	int field = -1;
	for (int i = 0; i < dbresult->iColumns; i++)
	{
		if (strcmp(fieldname, dbresult->pszResults[i]) == 0)
		{
			field = i;
			break;
		}
	}
	if (field < 0 || field >= dbresult->iColumns)
	{
		set_amxstring(amx, params[3], "", params[4]);
		return 0;
	}

	set_amxstring(amx, params[3], dbresult->pszResults[dbresult->iColumns * (dbresult->iCurrentRow + 1) + field], params[4]);

	return 1;
}

// native db_get_field_int(DBResult:result, field = 0)
static cell n_db_get_field_int(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (1 > params[1] || !g_mapDBResults.HasEx(key))
	{
		if (g_bDBLogging)
			logprintf("[Error] db_get_field_int(): invalid result handle %d", params[1]);

		return 0;
	}

	DBResult* dbresult = g_mapDBResults.Get(key);
	int field = params[2];
	if (field < 0 || field >= dbresult->iColumns)
	{
		return 0;
	}

	field = dbresult->iColumns * (dbresult->iCurrentRow + 1) + field;
	if (!dbresult->pszResults[field])
		return 0;

	return (cell)atoi(dbresult->pszResults[field]);
}

// native db_get_field_assoc_int(DBResult:result, const field[])
static cell n_db_get_field_assoc_int(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (1 > params[1] || !g_mapDBResults.HasEx(key))
	{
		if (g_bDBLogging)
			logprintf("[Error] db_get_field_assoc_int(): invalid result handle %d", params[1]);

		return 0;
	}

	char* fieldname = NULL;
	amx_StrParam(amx, params[2], fieldname);
	if (fieldname == NULL)
		return 0;

	DBResult* dbresult = g_mapDBResults.Get(key);
	int field = -1;
	for (int i = 0; i < dbresult->iColumns; i++)
	{
		if (strcmp(fieldname, dbresult->pszResults[i]) == 0)
		{
			field = i;
			break;
		}
	}
	if (field < 0 || field >= dbresult->iColumns)
	{
		return 0;
	}

	field = dbresult->iColumns * (dbresult->iCurrentRow + 1) + field;
	if (!dbresult->pszResults[field])
		return 0;

	return (cell)atoi(dbresult->pszResults[field]);
}

// native Float:db_get_field_float(DBResult:result, field = 0)
static cell n_db_get_field_float(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	float ret = 0.0f;
	if (1 > params[1] || !g_mapDBResults.HasEx(key))
	{
		if (g_bDBLogging)
			logprintf("[Error] db_get_field_float(): invalid result handle %d", params[1]);

		return amx_ftoc(ret);
	}

	DBResult* dbresult = g_mapDBResults.Get(key);
	int field = params[2];
	if (field < 0 || field >= dbresult->iColumns)
	{
		return amx_ftoc(ret);
	}

	char* result = dbresult->pszResults[dbresult->iColumns * (dbresult->iCurrentRow + 1) + field];
	if (!result)
		return amx_ftoc(ret);

	ret = strtof(result, NULL);

	return amx_ftoc(ret);
}

// native Float:db_get_field_assoc_float(DBResult:result, const field[])
static cell n_db_get_field_assoc_float(AMX* amx, cell* params)
{
	float ret = 0.0f;
	int key = params[1] - 1;
	if (1 > params[1] || !g_mapDBResults.HasEx(key))
	{
		if (g_bDBLogging)
			logprintf("[Error] db_get_field_assoc_float(): invalid result handle %d", params[1]);

		return amx_ftoc(ret);
	}

	char* fieldname = NULL;
	amx_StrParam(amx, params[2], fieldname);
	if (fieldname == NULL)
		return amx_ftoc(ret);

	DBResult* dbresult = g_mapDBResults.Get(key);
	int field = -1;
	for (int i = 0; i < dbresult->iColumns; i++)
	{
		if (strcmp(fieldname, dbresult->pszResults[i]) == 0)
		{
			field = i;
			break;
		}
	}
	if (field < 0 || field >= dbresult->iColumns)
	{
		return amx_ftoc(ret);
	}

	char* result = dbresult->pszResults[dbresult->iColumns * (dbresult->iCurrentRow + 1) + field];
	if (!result)
		return amx_ftoc(ret);

	ret = strtof(result, NULL);

	return amx_ftoc(ret);
}

// native db_get_mem_handle(DB:db)
static cell n_db_get_mem_handle(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (0 < params[1] || g_mapDBs.HasEx(key))
	{
		return (cell)g_mapDBs.Get(key);
	}
	return 0;
}

// native db_get_result_mem_handle(DBResult:result)
static cell n_db_get_result_mem_handle(AMX* amx, cell* params)
{
	int key = params[1] - 1;
	if (0 < params[1] || g_mapDBResults.HasEx(key))
	{
		return (cell)g_mapDBResults.Get(key);
	}
	return 0;
}

// native db_debug_openfiles()
static cell n_db_debug_openfiles(AMX* amx, cell* params)
{
	return g_mapDBs.Size();
}

// native db_debug_openresults()
static cell n_db_debug_openresults(AMX* amx, cell* params)
{
	return g_mapDBResults.Size();
}

AMX_NATIVE_INFO sampDb_Natives[] = {
	{"db_open", n_db_open},
	{"db_close", n_db_close},
	{"db_query", n_db_query},
	{"db_free_result", n_db_free_result},
	{"db_num_rows", n_db_num_rows},
	{"db_next_row", n_db_next_row},
	{"db_num_fields", n_db_num_fields},
	{"db_field_name", n_db_field_name},
	{"db_get_field", n_db_get_field},
	{"db_get_field_assoc", n_db_get_field_assoc},
	{"db_get_field_int", n_db_get_field_int},
	{"db_get_field_assoc_int", n_db_get_field_assoc_int},
	{"db_get_field_float", n_db_get_field_float},
	{"db_get_field_assoc_float", n_db_get_field_assoc_float},
	{"db_get_mem_handle", n_db_get_mem_handle},
	{"db_get_result_mem_handle", n_db_get_result_mem_handle},
	{"db_debug_openfiles", n_db_debug_openfiles},
	{"db_debug_openresults", n_db_debug_openresults},
	{NULL,NULL}
};

int amx_sampDbInit(AMX* amx)
{
	return amx_Register(amx, sampDb_Natives, -1);
}

/*int amx_sampDbCleanup(AMX* amx)
{
	return AMX_ERR_NONE;
}*/
