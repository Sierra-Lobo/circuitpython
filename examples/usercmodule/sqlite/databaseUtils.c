#include "databaseUtils.h"

status getText(usqlite_cursor_t* self, int column,  char** text, size_t* n)
{
	(*text) = sqlite3_column_text(self->stmt, column);
	*n = sqlite3_column_bytes(self->stmt, column);
	//memcpy(text, ptr, n);
	return SUCCESS;
}

status copyBlob(usqlite_cursor_t* self, int column,  uint8_t** blob, size_t* n)
{
	uint8_t* ptr = (uint8_t*)sqlite3_column_blob(self->stmt, column);
	(*n) = sqlite3_column_bytes(self->stmt, column);
	(*blob )= (uint8_t*)m_new(byte, *n);
	memcpy(*blob, ptr, *n);
	return SUCCESS;
}

status getBlob(usqlite_cursor_t* self, int column,  uint8_t** blob, size_t* n)
{
	*blob = (uint8_t*)sqlite3_column_blob(self->stmt, column);
	(*n) = sqlite3_column_bytes(self->stmt, column);
	return SUCCESS;
}

double getDouble(usqlite_cursor_t* self, int column)
{
	return sqlite3_column_double(self->stmt, column);
}


int getInt(usqlite_cursor_t* self, int column)
{
	return sqlite3_column_int(self->stmt, column);
}

void resetStatement(usqlite_cursor_t* self)
{
	sqlite3_clear_bindings(self->stmt);
	sqlite3_reset(self->stmt);
}

//update list of tables
void updateTablesList(usqlite_connection_t* conn) {


	usqlite_cursor_t self;

	createStatement(conn, &self, "SELECT name FROM pragma_table_list;");
	
	conn->tables = 0;

	char* tableName;
	size_t n;
	size_t max = 10;
    while( self.rc != SQLITE_DONE  && (max > 0)) {
	  stepExecute(&self); 
      if( self.rc == SQLITE_ROW ) {
		  usqlite_printf("SELECT NAME FROM PRAGMA_TABLE_LIST\n");
		  getText(&self, 0, &tableName,&n );
		  if (strcmp(tableName, "configs") == 0) conn->tables |= 1;
		  else if (strcmp(tableName, "commands") == 0) conn->tables |= (1 << COMMANDS);
		  else if (strcmp(tableName, "soh") == 0) conn->tables |= (1 << SOH);
		  else if (strcmp(tableName, "downlinks") == 0) conn->tables |= (1 << DOWNLINKS);
		  else if (strcmp(tableName, "uplinks") == 0) conn->tables |= (1 << UPLINKS);
		  else if (strcmp(tableName, "events") == 0) conn->tables |= (1 << EVENTS);
		  else if (strcmp(tableName, "payload") == 0) conn->tables |= (1 << PAYLOAD);
      } else if( self.rc == SQLITE_ERROR ) {
		mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("SQLITE TABLE CHECK ERROR"));
        return;
      }
	  --max;
    }
	cursorExit(&self);



}

int handleRc(usqlite_cursor_t* self)
{
    switch (self->rc)
    {
        case SQLITE_ROW:
            break;

        case SQLITE_DONE:
            self->rowcount = sqlite3_changes(self->connection->db);
            break;

        default:
            self->rowcount = -1;
            break;
    }
	return SUCCESS;
}


int createStatement(usqlite_connection_t* conn, usqlite_cursor_t* self, const char* sql)
{
	int ret = sqlite3_prepare_v2(conn->db, sql, strlen(sql), &self->stmt, NULL );	
	self->connection = conn;
	if (ret != SQLITE_OK) {
		usqlite_printf("stmt: %s error code: %d\n", sql, ret);
		mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("could not create sql stmt"));
        sqlite3_finalize(self->stmt);
        self->stmt = NULL;
		return DB_ERROR;
	}
	self->rowcount = 0;
	return SUCCESS;
}

int createStatementPersistent(usqlite_connection_t* conn, usqlite_cursor_t* self, const char* sql)
{
	int ret = sqlite3_prepare_v3(conn->db, sql, -1, SQLITE_PREPARE_PERSISTENT, &self->stmt, NULL );	
	if (ret != SQLITE_OK) {
		usqlite_printf("stmt: %s error code: %d\n", sql, ret);
		mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("could not create sql stmt"));
        sqlite3_finalize(self->stmt);
        self->stmt = NULL;
		return DB_ERROR;
	}
	return SUCCESS;
}



int executeStr(usqlite_cursor_t* self, const char* sql) {

	self->rowcount = 0;
    //cursorClose(self);
    if (!sql || !*sql) {
        mp_raise_ValueError(MP_ERROR_TEXT("empty sql??"));
		return INPUT_ERROR;
    }

	createStatement(self->connection, self, sql);	
	return executeStmtNoReturn(self);
	/*self->rc = stepExecute(self);
    switch (self->rc)
    {
        case SQLITE_ROW:
            break;

        case SQLITE_DONE:
            self->rowcount = sqlite3_changes(self->connection->db);
            break;

        default:
            self->rowcount = -1;
            break;
    }
	sqlite3_finalize(self->stmt);
	return self->rc;
*/
	
}


int fetchInt(usqlite_cursor_t* self, size_t col, uint32_t* res)
{
	if (self->rc != SQLITE_ROW) {
		usqlite_printf("fetch int not ROW %d\n", self->rc);
		stepExecute(self);
	}
	(*res) = getInt(self, 0);
	if (self->rc == SQLITE_ROW) {
		//stepExecute(self);
	}
	return SUCCESS;
}

int fetchBlob(usqlite_cursor_t* self, size_t col, void** data, size_t* len)
{
	if ((self->rc != SQLITE_ROW)) {
		usqlite_printf("fetch blob not row%d", self->rc);
		stepExecute(self);
	}
	usqlite_printf("rc before fetch Blob %d", self->rc);
	copyBlob(self, col, data, len);
	//(*data) = (uint8_t*)sqlite3_column_blob(self->stmt, col);
	usqlite_printf("data from fetchBlob:\n");
	usqlite_printf("data:%s\n", (char*)(*data));	
	(*len) = sqlite3_column_bytes(self->stmt, col);
	if (self->rc == SQLITE_ROW) {
		//getBlob(self, col, data, len);
		//stepExecute(self);
		usqlite_printf("rc == sqlite row fetch Blob %d", self->rc);
	}
	return self->rc;
}


int cursorExit(usqlite_cursor_t* self)
{
    if (!self->stmt) {
        return SUCCESS; //closing empty statement
    }

    usqlite_logprintf(___FUNC___ " closing: stmt\n");
	//sqlite3_clear_bindings(self->stmt);
	//sqlite3_reset(self->stmt);
    sqlite3_finalize(self->stmt);
    self->stmt = NULL;
    self->rowcount = -1;
    self->rc = SQLITE_OK;
	usqlite_printf("stmt closed\n");	
	return SUCCESS;
}

//TODO not sure why this hangs forever?
int executeStmtNoReturn(usqlite_cursor_t* self) {
    self->rowcount = 0;

	while (! (self->rc == SQLITE_DONE)) {
		usqlite_printf("run no return ec %d\n", self->rc);
    stepExecute(self);

    switch (self->rc)
    {
        case SQLITE_ROW:
            break;

        case SQLITE_DONE:
            self->rowcount = sqlite3_changes(self->connection->db);
            break;

		case SQLITE_BUSY:
		case SQLITE_OK:
			return SUCCESS;
        
		default:
            self->rowcount = -1;
            return SQLITE_ERROR;
    }
	}
    return self->rc;
}


#define get_max_rows "SELECT max(id) from ?1"
uint32_t getMaxRows(usqlite_connection_t* self, const char* tableName)
{
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, get_max_rows);
	sqlite3_bind_text(cursor.stmt, 1, tableName, strlen(tableName), SQLITE_TRANSIENT);
	stepExecute(&cursor);
	uint32_t maxRows = sqlite3_column_int64(cursor.stmt, 0);
	return maxRows;	

}

uint32_t handleNegativeIndex(usqlite_connection_t* self, const char* tableName,  int index)
{
	if (index >= 0) return index;

	uint32_t maxRows = getMaxRows(self, tableName);
	return maxRows + index + 1;
}



void createBitset(uint8_t** bitset, size_t n)
{
	(*bitset) = m_new(byte, ( n / 8 ) + ( n % 8 != 0 ));
}

bool getBitsetValue(uint8_t* bitset, size_t index)
{
	size_t byt = index / 8;
	size_t shift = index % 8;

	return bitset[ byt ] & ( 1 << shift );
}

void setBitsetValue(uint8_t* bitset, size_t idx, bool value)
{
	size_t byt = idx / 8;
	size_t shift = idx % 8;

	if( value ) {
	// Turn on the selected bit
	bitset[byt] |= ( 1 << shift );
	} else {
	// Leave on all bits but the selected one
	bitset[byt] &= ~( 1 << shift );
	}

}



mp_obj_t mp_obj_bytearray_move(size_t n, void *items) {
    mp_obj_array_t *o = m_new_obj(mp_obj_array_t);
    o->base.type = &mp_type_bytearray;
    o->typecode = 1;
    o->free = 0;
    o->len = n;
    o->items = items;
    return MP_OBJ_FROM_PTR(o);
}
