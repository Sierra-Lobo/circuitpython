#include "databaseUtils.h"
status getText(usqlite_cursor_t* self, int column,  char** text, size_t* n)
{
	const unsigned char* ptr = sqlite3_column_text(self->stmt, column);
	*n = sqlite3_column_bytes(self->stmt, column);
	//memcpy(text, ptr, n);
	return SUCCESS;
}

status copyBlob(usqlite_cursor_t* self, int column,  uint8_t* blob, size_t* n)
{
	uint8_t* ptr = (uint8_t*)sqlite3_column_blob(self->stmt, column);
	(*n) = sqlite3_column_bytes(self->stmt, column);
	memcpy(blob, ptr, *n);
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

	createStatementPersistent(conn, &self, "SELECT name FROM pragma_table_list;");
	
	conn->tables = 0;

	char* tableName;
	size_t n;
    while( self.rc != SQLITE_DONE ) {
	  stepExecute(&self); 
      if( self.rc == SQLITE_ROW ) {
		  getText(&self, 0, &tableName,&n );
		  if (strcmp(tableName, "config") == 0) conn->tables |= 1;
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
    }



}




int createStatement(usqlite_connection_t* conn, usqlite_cursor_t* self, const char* sql)
{
	int ret = sqlite3_prepare_v2(conn->db, sql, -1, &self->stmt, NULL );	
	self->connection = conn;
	if (ret != SQLITE_OK) {
		mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("could not create sql stmt"));
        sqlite3_finalize(self->stmt);
        self->stmt = NULL;
		return DB_ERROR;
	}
	return SUCCESS;
}

int createStatementPersistent(usqlite_connection_t* conn, usqlite_cursor_t* self, const char* sql)
{
	int ret = sqlite3_prepare_v3(conn->db, sql, -1, SQLITE_PREPARE_PERSISTENT, &self->stmt, NULL );	
	if (ret != SQLITE_OK) {
		mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("could not create sql stmt"));
        sqlite3_finalize(self->stmt);
        self->stmt = NULL;
		return DB_ERROR;
	}
	return SUCCESS;
}
