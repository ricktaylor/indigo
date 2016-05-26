

#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static pthread_key_t s_key;
static pthread_once_t s_key_once = PTHREAD_ONCE_INIT;

struct Database
{
	sqlite3*      m_db;
	sqlite3_stmt* m_statements[128];
};

struct unlock_notify
{
	int             fired;
	pthread_cond_t  cond;
	pthread_mutex_t mutex;
};

static void unlock_notify_cb(void **apArg, int nArg)
{
	int i;
	for (i = 0; i < nArg; i++)
	{
		struct unlock_notify* p = (struct unlock_notify*)apArg[i];

		int err = pthread_mutex_lock(&p->mutex);
		if (err != 0)
		{
			log_error("Failed to lock mutex: %s\n", strerror(err));
			abort();
		}

		p->fired = 1;
		err = pthread_cond_signal(&p->cond);
		if (err != 0)
		{
			log_error("Failed to signal condition variable: %s\n", strerror(err));
			abort();
		}

		pthread_mutex_unlock(&p->mutex);
	}
}

static int wait_for_unlock(sqlite3* db)
{
	struct unlock_notify un = {0};

	pthread_mutex_init(&un.mutex,NULL);
	pthread_cond_init(&un.cond,NULL);

	int err = sqlite3_unlock_notify(db,&unlock_notify_cb,&un);
	if (err != SQLITE_OK)
	{
		return err;
	}

	err = pthread_mutex_lock(&un.mutex);
	if (err != 0)
	{
		log_error("Failed to lock mutex: %s\n", strerror(err));
		abort();
	}

	while (!un.fired)
	{
		err = pthread_cond_wait(&un.cond,&un.mutex);
		if (err != 0)
		{
			log_error("Failed to lock mutex: %s\n", strerror(err));
			abort();
		}
	}

	pthread_mutex_unlock(&un.mutex);

	pthread_mutex_destroy(&un.mutex);
	pthread_cond_destroy(&un.cond);

	return err;
}

int sqlite3_blocking_step(sqlite3_stmt* pStmt)
{
	int err = sqlite3_step(pStmt);
	while ((err & SQLITE_LOCKED) == SQLITE_LOCKED)
	{
		err = wait_for_unlock(sqlite3_db_handle(pStmt));
		if (err != SQLITE_OK)
		{
			break;
		}

		sqlite3_reset(pStmt);
		err = sqlite3_step(pStmt);
	}

	return err;
}

static int prepStatement(sqlite3* db, const char* szSQL, sqlite3_stmt** stmt)
{
	size_t len = szSQL ? strlen(szSQL)+1 : 0;
	int err = sqlite3_prepare_v2(db,szSQL,len,stmt,NULL);
	while ((err & SQLITE_LOCKED) == SQLITE_LOCKED)
	{
		err = wait_for_unlock(db);
		if (err != SQLITE_OK)
		{
			break;
		}

		err = sqlite3_prepare_v2(db,szSQL,len,stmt,NULL);
	}
	return err;
}

static int initDatabase(sqlite3* db)
{
	if (sqlite3_extended_result_codes(db,1) ||
			sqlite3_busy_timeout(db,5000))
	{
		log_error("sqlite3_create_function failed: %s\n",sqlite3_errmsg(db));
		return -1;
	}

	return 0;
}

static void databaseClose(void* p)
{
	struct Database* db = (struct Database*)p;
	if (db)
	{
		if (db->m_db)
		{
			size_t i = 0;
			for (;i < (sizeof(db->m_statements)/sizeof(db->m_statements[0])); ++i)
			{
				if (db->m_statements[i])
				{
					sqlite3_finalize(db->m_statements[i]);
				}
			}

			sqlite3_close(db->m_db);
		}

		free(db);
	}
}

static void errorLogCallback(void *pArg, int iErrCode, const char *zMsg)
{
	int major = iErrCode & 0xFF;
	if (major != SQLITE_LOCKED)
	{
		if (major == SQLITE_NOTICE)
		{
			log_verbose(1,"sqlite3 notice: (%d) %s\n", iErrCode, zMsg);
		}
		else if (major == SQLITE_WARNING)
		{
			log_warning("sqlite3 warning: (%d) %s\n", iErrCode, zMsg);
		}
		else
		{
			log_error("sqlite3 error: (%d) %s\n", iErrCode, zMsg);
		}
	}
}

static void makeKey()
{
    int err = pthread_key_create(&s_key, &databaseClose);
    if (err)
    {
    	log_error("pthread_key_create failed: %s\n",strerror(err));
    }
    else
    {
    	sqlite3_config(SQLITE_CONFIG_LOG, &errorLogCallback, NULL);
    }
}

static struct Database* getThreadDatabase()
{
	if (!getDbFilename)
	{
		log_error("Implement getDbFilename()\n");
		assert(0);
		return NULL;
	}

	struct Database* db = NULL;
	int err = pthread_once(&s_key_once, &makeKey);
	if (err)
	{
		log_error("pthread_once failed: %s\n",strerror(err));
	}
	else
	{
		db = (struct Database*)pthread_getspecific(s_key);
		if (!db)
		{
			db = calloc(1,sizeof(struct Database));
			if (!db)
			{
				log_error("Failed to allocate memory: %s\n",strerror(errno));
			}
			else
			{
				err = sqlite3_open_v2(getDbFilename(),&db->m_db,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE,NULL);
				if (err)
				{
					if (!db->m_db)
					{
						log_error("sqlite3_open_v2 failed: out of memory\n");
					}
					else
					{
						log_error("sqlite3_open_v2 failed: %s\n",sqlite3_errmsg(db->m_db));
					}

					databaseClose(db);
					db = NULL;
				}
				else if (initDatabase(db->m_db) == 0)
				{
					err = pthread_setspecific(s_key, db);
					if (err)
					{
						log_error("pthread_setspecific failed: %s\n",strerror(err));
						databaseClose(db);
						db = NULL;
					}
				}
			}
		}
	}

	return db;
}

int beginTransaction()
{
	return sqlExec("BEGIN TRANSACTION;");
}

int commitTransaction()
{
	int ret = sqlExec("COMMIT TRANSACTION;");
	if (ret == -1)
	{
		rollbackTransaction();
	}
	return ret;
}

void rollbackTransaction()
{
	sqlExec("ROLLBACK TRANSACTION;");
}

sqlite3_stmt* prepareStatement(const char* szSQL)
{
	sqlite3_stmt* stmt = NULL;

	struct Database* db = getThreadDatabase();
	if (db)
	{
		static const size_t len = (sizeof(db->m_statements)/sizeof(db->m_statements[0]));

		int found = 0;
		uint32_t offset = FNV1a(szSQL);
		uint32_t end = offset + len;

		// Search the table first for a gap
		for (;offset != end; ++offset)
		{
			uint32_t h = offset % len;
			if (!db->m_statements[h] || strcmp(sqlite3_sql(db->m_statements[h]),szSQL) == 0)
			{
				found = 1;
				offset = h;
				break;
			}
		}

#if defined(HAVE_SQLITE3_STMT_BUSY)
		if (!found)
		{
			log_verbose(1,"SQL busy: %s\n",szSQL);

			// Now search for a not in use entry
			end = offset + len;
			for (;offset < end; ++offset)
			{
				uint32_t h = offset % len;

				if (!sqlite3_stmt_busy(db->m_statements[h]))
				{
					sqlite3_finalize(db->m_statements[h]);
					db->m_statements[h] = NULL;
					found = 1;
					offset = h;
					break;
				}
			}
		}
#endif

		if (!found)
		{
			log_error("Out of hash table space!\n");
			return NULL;
		}

		if (!db->m_statements[offset])
		{
			if (prepStatement(db->m_db,szSQL,&db->m_statements[offset]) != SQLITE_OK)
			{
				log_error("prepareStatement failed: %s, SQL: %s\n",sqlite3_errmsg(db->m_db),szSQL);
				return NULL;
			}
		}

		stmt = db->m_statements[offset];
		if (stmt)
		{
			// Clear the bindings
			sqlite3_clear_bindings(stmt);
		}
	}

	return stmt;
}

static int bindArgs(sqlite3_stmt* stmt, va_list args)
{
	int err = SQLITE_OK;
	struct arg_reorder
	{
		char type;
		int index;
	} arg_buf[10];
	memset(arg_buf,0,sizeof(arg_buf));

	// First re-order the bind args to match the varargs
	int params = sqlite3_bind_parameter_count(stmt);
	if (params > sizeof(arg_buf))
	{
		log_error("Too many parameters in expression %s\n",sqlite3_sql(stmt));
		return SQLITE_ERROR;
	}

	int max = 0;
	int i = 1;
	for (; i <= params && err == SQLITE_OK; ++i)
	{
		const char* szParam = sqlite3_bind_parameter_name(stmt,i);
		int pos = szParam[2] - '0';
		if (arg_buf[pos].type != 0)
		{
			log_error("Duplicate parameter index %c (%s) in expression %s\n",szParam[2],szParam,sqlite3_sql(stmt));
			err = SQLITE_ERROR;
		}
		else
		{
			arg_buf[pos].type = szParam[1];
			arg_buf[pos].index = i;

			if (pos >= max)
				max = pos+1;
		}
	}

	// Now scan the arg_buf, binding the argument
	for (i = 0; i < max && err == SQLITE_OK; ++i)
	{
		switch (arg_buf[i].type)
		{
		case 0:
			break;

		case 'B':
			{
				const void* b = va_arg(args,const void*);
				size_t len = va_arg(args,size_t);
				if (len == 0 || !b)
					err = sqlite3_bind_null(stmt,arg_buf[i].index);
				else
					err = sqlite3_bind_blob(stmt,arg_buf[i].index,b,len,SQLITE_TRANSIENT);
			}
			break;

		case 'D':
			err = sqlite3_bind_double(stmt,arg_buf[i].index,va_arg(args,double));
			break;

		case 'I':
			err = sqlite3_bind_int(stmt,arg_buf[i].index,va_arg(args,int));
			break;

		case 'L':
			err = sqlite3_bind_int64(stmt,arg_buf[i].index,va_arg(args,sqlite3_int64));
			break;

		case 'T':
			{
				const char* s = va_arg(args,const char*);
				if (!s)
					err = sqlite3_bind_null(stmt,arg_buf[i].index);
				else
					err = sqlite3_bind_text(stmt,arg_buf[i].index,s,-1,SQLITE_TRANSIENT);
			}
			break;

		case 'V':
			err = sqlite3_bind_value(stmt,arg_buf[i].index,va_arg(args,const sqlite3_value*));
			break;

		default:
			log_error("Unknown parameter type :%c%d in expression %s\n",arg_buf[i].type,arg_buf[i].index,sqlite3_sql(stmt));
			err = SQLITE_ERROR;
			break;
		}
	}

	return err;
}

// Returns 0 on success, -1 on error, 1 on changes
int sqlExec(const char* szSQL, ...)
{
	va_list args;
	va_start(args,szSQL);

	int retval = -1;
	sqlite3_stmt* stmt = prepareStatement(szSQL);
	if (stmt)
	{
		int err = bindArgs(stmt,args);
		if (err == SQLITE_OK)
		{
			do
			{
				err = sqlite3_blocking_step(stmt);
			}
			while (err == SQLITE_ROW);

			if (err == SQLITE_DONE)
			{
				retval = (sqlite3_changes(sqlite3_db_handle(stmt)) ? 1 : 0);
			}
		}

		sqlite3_reset(stmt);
	}

	va_end(args);

	return retval;
}

// Returns NULL on error, or a prepared and bound statement
sqlite3_stmt* sqlSelect(const char* szSQL, ...)
{
	va_list args;
	va_start(args,szSQL);

	sqlite3_stmt* stmt = prepareStatement(szSQL);
	if (stmt)
	{
		int err = bindArgs(stmt,args);
		if (err != SQLITE_OK)
		{
			stmt = NULL;
		}
	}

	va_end(args);

	return stmt;
}

sqlite3_int64 getLastRowId()
{
	struct Database* db = getThreadDatabase();
	if (!db)
		return 0;

	return sqlite3_last_insert_rowid(db->m_db);
}

sqlite3* getDatabase()
{
	struct Database* db = getThreadDatabase();
	return db ? db->m_db : NULL;
}

int sqlBulkExec(const char* szSQL)
{
	struct Database* db = getThreadDatabase();
	if (!db)
		return -1;

	char* szErr = NULL;
	int err = sqlite3_exec(db->m_db,szSQL,NULL,NULL,&szErr);
	while ((err & SQLITE_LOCKED) == SQLITE_LOCKED)
	{
		err = wait_for_unlock(db->m_db);
		if (err != SQLITE_OK)
		{
			break;
		}

		err = sqlite3_exec(db->m_db,szSQL,NULL,NULL,&szErr);
	}

	if (err != SQLITE_OK)
	{
		log_error("sqlite3_exec failed: (%d) %s, while executing %s\n",err >> 8,szErr,szSQL);
	}

	sqlite3_free(szErr);

	return (err == SQLITE_OK ? 0 : -1);
}
