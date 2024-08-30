#include "DataBaseInterface.h"
#include "databaseUtils.h"
#include "math.h"


status checkError(int ec) {
	return (ec == SQLITE_OK) || (ec == SQLITE_DONE); 
}

status initializeDatabase(usqlite_connection_t* self)
{
	if (self->active) return SUCCESS;
		
	int result= SUCCESS;
	
	//need way of checking if tables have already been made
	updateTablesList(self);
	if (!(self->tables & (1 << CONFIG))) {	
		usqlite_cursor_t cursor;
		cursor.connection = self;
		usqlite_printf("create config\n");
		result = executeStr(&cursor, create_config);

		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
            mp_raise_ValueError(MP_ERROR_TEXT("result is not success"));
			return result;
		}
		self->tables |= (1 << CONFIG);
		cursorExit(&cursor);
	}

	if (!(self->tables & (1 << COMMANDS))) {
		usqlite_cursor_t cursor;
		cursor.connection = self;
		usqlite_printf("create commands\n");
		result = executeStr(&cursor, create_commands);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
			mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Cannot create commands table"));
			return result;
		}
		self->tables |= (1 << COMMANDS);
		cursorExit(&cursor);
	}

	if (!(self->tables & (1 << SOH))) {
		usqlite_cursor_t cursor;
		cursor.connection = self;
		usqlite_printf("create soh\n");
		result = executeStr(&cursor, create_soh);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
			mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Cannot create soh table"));
			return result;
		}
		self->tables |= (1 << SOH);
		cursorExit(&cursor);
	}

	if (!(self->tables & (1 << DOWNLINKS))) {
		usqlite_cursor_t cursor;
		cursor.connection = self;
		usqlite_printf("create downlinks\n");
		result = executeStr(&cursor, create_downlinks);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
			mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Cannot create downlinks table"));
			return result;
		}
		self->tables |= (1 << DOWNLINKS);
		cursorExit(&cursor);
	}

	if (!(self->tables & (1 << EVENTS))) {
		usqlite_cursor_t cursor;
		cursor.connection = self;
		usqlite_printf("create events\n");
		result = executeStr(&cursor, create_events);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
			mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Cannot create events table"));
			return result;
			}
		self->tables |= (1 << EVENTS);
		cursorExit(&cursor);
		}
	if (!(self->tables & (1 << UPLINKS))) {
		usqlite_cursor_t cursor;
		cursor.connection = self;
		usqlite_printf("create uplinks\n");
		result = executeStr(&cursor, create_uplinks);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
			mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Cannot create uplinks table"));
			return result;
		}
		self->tables |= (1 << UPLINKS);
		cursorExit(&cursor);
	}
	
	if (!(self->tables & (1 << PAYLOAD))) {
		usqlite_cursor_t cursor;
		cursor.connection = self;
		usqlite_printf("create payload\n");
		result = executeStr(&cursor, create_payload);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
			mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Cannot create uplinks table"));
			return result;
		}
		self->tables |= (1 << PAYLOAD);
		cursorExit(&cursor);
	}

	self->active = 1;
	return result;


}

int insertSoh(usqlite_connection_t* self, uint32_t timestamp, uint8_t* data, size_t len) {
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_soh);
	int ec = sqlite3_bind_int64(cursor.stmt, 1, timestamp);
	ec = sqlite3_bind_blob(cursor.stmt, 2, data, len, SQLITE_TRANSIENT);
	ec = stepExecute(&cursor);
	handleRc(&cursor);
	cursorExit(&cursor);
	return checkError(ec); 
	
}









int fetchSoh(usqlite_connection_t* self, uint32_t index, uint8_t** data, size_t* len) {

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_soh_index);
	sqlite3_bind_int64(cursor.stmt, 1, index);
	stepExecute(&cursor);
	uint32_t timestamp;
	fetchInt(cursor.stmt, 0, &timestamp);
	fetchBlob(&cursor, 1, data, len);	
	cursorExit(&cursor);
	return timestamp; 
}

int fetchSohTimestamp(usqlite_connection_t* self, uint32_t timeStart, uint32_t timeEnd, uint8_t** data, size_t* len) {
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_soh_time);
	sqlite3_bind_int64(cursor.stmt, 1, timeStart);
	sqlite3_bind_int64(cursor.stmt, 2, timeEnd);
	stepExecute(&cursor);
	uint32_t timestamp;
	fetchInt(cursor.stmt, 0, &timestamp);
	fetchBlob(&cursor, 1, data, len);	
	cursorExit(&cursor);
	return timestamp; 

}

int logEvent(usqlite_connection_t* self, uint8_t level, const char* msg, size_t n, uint32_t timestamp, double uptime ){
	
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_event);
	int ec = sqlite3_bind_int(cursor.stmt, 1, level);
	ec = sqlite3_bind_int64(cursor.stmt, 2, timestamp);
	ec = sqlite3_bind_int64(cursor.stmt, 3, uptime);
	ec = sqlite3_bind_text(cursor.stmt, 4, msg, n, SQLITE_TRANSIENT); 
	ec = stepExecute(&cursor);
	cursorExit(&cursor);
	return checkError(ec); 
}


int insertCommand(usqlite_connection_t* self, uint8_t cmdID, double dueTime, uint8_t* args, size_t len ){

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_command);
	int ec = sqlite3_bind_int64(cursor.stmt, 1, dueTime);
	ec = sqlite3_bind_int(cursor.stmt, 2, cmdID);
	ec = sqlite3_bind_blob(cursor.stmt, 3, args, len, SQLITE_TRANSIENT);
	ec = stepExecute(&cursor);
	cursorExit(&cursor);
	return checkError(ec); 

}


int getNextCommand(usqlite_connection_t* self, uint8_t* cmdID, uint8_t** args, size_t* len, uint32_t* index){

	
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_command);
	int ec = stepExecute(&cursor);
	fetchInt(&cursor, 0, index);
	uint32_t ctime;
	fetchInt(&cursor, 1, &ctime);
	fetchInt(&cursor, 2, cmdID);
	fetchBlob(&cursor, 3, args, len);
	cursorExit(&cursor);
	return checkError(ec); 

}


int deleteCommand(usqlite_connection_t* self, uint32_t index){

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, delete_command);
	sqlite3_bind_int64(cursor.stmt, 1, index);
	int ec = stepExecute(&cursor);
	cursorExit(&cursor);
	return checkError(ec); 

}



int insertPayloadData(usqlite_connection_t* self, uint8_t* data, size_t len,  double pos[3], uint32_t timestamp ) {
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_payload);
	sqlite3_bind_int64(cursor.stmt, 1, timestamp);
	sqlite3_bind_blob(cursor.stmt, 2, pos, 3 * sizeof(double), SQLITE_TRANSIENT);
	sqlite3_bind_blob(cursor.stmt, 3, data, len, SQLITE_TRANSIENT);
	stepExecute(&cursor);
	uint32_t dbid;
	fetchInt(&cursor, 0, &dbid); 
	cursorExit(&cursor);
	return dbid;

}


int createEmptyPayloadEntry(usqlite_connection_t* self, size_t dataLen, double pos[3], uint32_t timestamp)
{
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_payload_empty);
	sqlite3_bind_int64(cursor.stmt, 1, timestamp);
	sqlite3_bind_blob(cursor.stmt, 2, pos, 3 * sizeof(double), SQLITE_TRANSIENT);
	sqlite3_bind_int64(cursor.stmt, 3, dataLen);
	stepExecute(&cursor);
	uint32_t dbid;
	fetchInt(&cursor, 0, &dbid);
	cursorExit(&cursor);
	return dbid;
}


int fetchPayloadDataID(usqlite_connection_t* self, uint32_t index, uint8_t** data, size_t* len, uint8_t** pos, uint32_t* timestamp) {


	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_payload_index);
	sqlite3_bind_int64(cursor.stmt, 1, index);
	int ec = stepExecute(&cursor);
	fetchInt(cursor.stmt, 0, timestamp);
	fetchBlob(&cursor, 1, data, len);
	size_t poslen;	
	fetchBlob(&cursor, 2, pos, &poslen);	
	cursorExit(&cursor);
	return checkError(ec); 
}

int fetchPayloadDataTime(usqlite_connection_t* self, uint32_t* index, uint8_t** data,size_t* len,  uint8_t** pos, uint32_t* timeStart, uint32_t* timeStop) {

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_payload_time);
	sqlite3_bind_int64(cursor.stmt, 1, *timeStart);
	sqlite3_bind_int64(cursor.stmt, 2, *timeStop);
	int ec = stepExecute(&cursor);
	fetchInt(cursor.stmt, 0, timeStart);
	fetchBlob(&cursor, 1, data, len);
	size_t poslen;	
	fetchBlob(&cursor, 2, pos, &poslen);	
	cursorExit(&cursor);
	return checkError(ec); 
	
}


int fetchPayloadDataPos(usqlite_connection_t* self, uint32_t* index, uint8_t* data,size_t* len,  double pos[3], double* timestamp) {
	
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_payload_pos);
	sqlite3_bind_int64(cursor.stmt, 1, *timestamp);
	int ec = stepExecute(&cursor);
	fetchInt(cursor.stmt, 0, timestamp);
	fetchBlob(&cursor, 1, &data, len);
	size_t poslen;	
	fetchBlob(&cursor, 2, &pos, &poslen);	
	cursorExit(&cursor);
	return checkError(ec); //?


}



int deletePayloadDataID(usqlite_connection_t* self, uint32_t index) {

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, delete_payload);
	sqlite3_bind_int64(cursor.stmt, 1, index);
	int ec = stepExecute(&cursor);
	cursorExit(&cursor);
	return checkError(ec); 

}






int createUplink(usqlite_connection_t* self, uint32_t txID, uint32_t txSize, uint32_t numPackets) {

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_uplink);
	sqlite3_bind_int64(cursor.stmt, 1, txID);
	sqlite3_bind_int64(cursor.stmt, 2, txSize);
	//figure out what to do for bitset, this might be ok, or maybe make obj for bitset
	uint8_t* missing;
	createBitset(&missing, numPackets);
	sqlite3_bind_blob(cursor.stmt, 3, missing, numPackets, SQLITE_TRANSIENT);
	sqlite3_bind_int64(cursor.stmt, 4, numPackets);
	int ec = stepExecute(&cursor);
#if MICROPY_MALLOC_USES_ALLOCATED_SIZE
	m_free(missing, ( numPackets / 8 ) + ( numPackets % 8 != 0 ));
#else
	m_free(missing);
#endif
	cursorExit(&cursor);
	return checkError(ec); 

}



/*
int insertUplinkPacket(usqlite_connection_t* self,uint32_t txID, uint32_t packetID,  uint8_t* data, size_t len) {

 sqlite_blob_write ???
}
*/

int deleteEntry(usqlite_connection_t* self, uint32_t ID, const char* tableName) {
	usqlite_cursor_t cursor;
	if (strcmp(tableName, "configs") ==0) createStatement(self, &cursor, delete_config);
	if (strcmp(tableName, "commands") ==0) createStatement(self, &cursor, delete_command);
	if (strcmp(tableName, "soh") ==0) createStatement(self, &cursor, delete_soh_id);
	if (strcmp(tableName, "downlinks") ==0) createStatement(self, &cursor, delete_downlink_id);
	if (strcmp(tableName, "uplinks") ==0) createStatement(self, &cursor, delete_uplink_id);
	if (strcmp(tableName, "events") ==0) createStatement(self, &cursor, delete_event);
	if (strcmp(tableName, "payload") ==0) createStatement(self, &cursor, delete_payload);
	sqlite3_bind_int64(cursor.stmt, 1, ID);
	int ec = stepExecute(&cursor);
	cursorExit(&cursor);
	return checkError(ec); 
}


int createDownlink(usqlite_connection_t* self, uint8_t* data, size_t len, size_t numPackets, int priority) {
	
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_downlink);
	sqlite3_bind_blob(cursor.stmt, 1, data, len, SQLITE_TRANSIENT); 
	sqlite3_bind_int(cursor.stmt, 2, priority);
	
	sqlite3_bind_blob(cursor.stmt, 3, data, len, SQLITE_TRANSIENT); 
	//sqlite3_bind_int(cursor.stmt, 3, ( numPackets / 8 ) + ( numPackets % 8 > 0 ));
	sqlite3_bind_int(cursor.stmt, 4, numPackets);
	stepExecute(&cursor);
	stepExecute(&cursor);
	size_t dbid;
	fetchInt(&cursor, 0, &dbid);
	cursorExit(&cursor);
	return dbid;
}

int createDownlinkEmpty(usqlite_connection_t* self, size_t len, size_t numPackets, int priority) {
	
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_downlink_empty);
	sqlite3_bind_int(cursor.stmt, 1, len); 
	sqlite3_bind_int(cursor.stmt, 2, priority);
	sqlite3_bind_int(cursor.stmt, 3, ( numPackets / 8 ) + ( numPackets % 8 > 0 ));
	sqlite3_bind_int(cursor.stmt, 4, numPackets);
	stepExecute(&cursor);
	stepExecute(&cursor);
	size_t dbid;
	fetchInt(&cursor, 0, &dbid);
	cursorExit(&cursor);
	return dbid;
}


int getNextDownlink(usqlite_connection_t* self, uint8_t** data, size_t* len, uint32_t* index) {

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_downlink);
	int ec = stepExecute(&cursor);
	fetchInt(&cursor, 0, index);
	fetchBlob(&cursor, 1, data, len);
	cursorExit(&cursor);
	return checkError(ec); 

}

int getNextDownlinkID(usqlite_connection_t* self, uint32_t* index) {

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_downlink_id);
	int ec  = stepExecute(&cursor);
	fetchInt(&cursor, 0, index);
	cursorExit(&cursor);
	return checkError(ec); 
}

int getMissingPacketIds(usqlite_connection_t* self, uint32_t txID, uint32_t** packetIDs, size_t* len)
{
	usqlite_cursor_t cursor;
	uint8_t* missing;
	createStatement(self, &cursor, query_missing_uplink);
	sqlite3_bind_int64(cursor.stmt, 1, txID);
	int ec = stepExecute(&cursor);
	size_t numBytes;
	fetchInt(&cursor, 0, len);
	fetchBlob(&cursor, 1, &missing, &numBytes); 
	(*packetIDs) = (uint32_t*)m_new(byte ,(*len) * 4 );	
	size_t j=0;
	for (size_t i=0; i<(*len); i++)
	{
		if (!getBitsetValue(missing, i)) {
			(*packetIDs)[j++] = i;
		}

	}
	(*len) = j;
	cursorExit(&cursor);
	return checkError(ec);
}



int fetchDataBlob(usqlite_connection_t* self, const char* tableName, uint32_t rowID, uint8_t* data, size_t len, size_t offset)
{
	sqlite3_blob* blob;
	
	int ec = sqlite3_blob_open( self->db, "main", tableName, "data", rowID, 0, &blob );
	int available = sqlite3_blob_bytes(blob) - offset;
	len = len > available ?  available : len;
	ec = sqlite3_blob_read(blob, data, len, offset);
	ec =  sqlite3_blob_close(blob);
	//should probably first check for errors, if error return error else return len
	return len;
}

int writeDataBlob(usqlite_connection_t* self, const char* tableName, uint32_t rowID, uint8_t* data, size_t len, size_t offset)
{
	sqlite3_blob* blob;
	
	int ec = sqlite3_blob_open( self->db, "main", tableName, "data", rowID, 1, &blob );
	int available = sqlite3_blob_bytes(blob) - offset;
	len = len > available ?  available : len;
	ec = sqlite3_blob_write(blob, data, len, offset);
	ec =  sqlite3_blob_close(blob);
	return checkError(ec);
}

int setUplinkPacketReceived(usqlite_connection_t* self, uint32_t rowID, uint32_t packetID, uint8_t val)
{
	
	sqlite3_blob* blob;
	
	int ec = sqlite3_blob_open( self->db, "main", "uplinks", "missing", rowID, 1, &blob );
	size_t missingLength = sqlite3_blob_bytes(blob);
	uint8_t* data = m_new(byte, missingLength);
	ec = sqlite3_blob_read(blob, data, missingLength, 0);

	setBitsetValue(data, packetID, val);
	ec = sqlite3_blob_write(blob, data, missingLength, 0);
	ec =  sqlite3_blob_close(blob);
	//should probably first check for errors, if error return error else return len
#if MICROPY_MALLOC_USES_ALLOCATED_SIZE
	m_free(data, missingLength);
#else
	m_free(data);
#endif
	return checkError(ec);

}

int insertConfig(usqlite_connection_t* self, uint8_t* data, size_t len)
{
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_config);
	sqlite3_bind_blob(cursor.stmt, 1, data, len, SQLITE_TRANSIENT);
	int ec = stepExecute(&cursor);
	cursorExit(&cursor);
	return checkError(ec); 


}

int fetchConfig(usqlite_connection_t* self, uint32_t index, void** data, size_t* len)
{
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_config_index);
	sqlite3_bind_int64(cursor.stmt, 1, index);
	int ec = stepExecute(&cursor);
	usqlite_printf("fetch config step execute ec %d\n", ec);
	fetchBlob(&cursor, 0, data, len);	
	usqlite_printf("data before cursor exit:%s\n", (char*)(*data));	
	cursorExit(&cursor);
	usqlite_printf("data after cursor exit:%s\n", (char*)(*data));	
	return checkError(ec); 


}


int getTableSize(usqlite_connection_t* self, const char* tableName) {

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_table_size);
	int ec = sqlite3_bind_text(cursor.stmt, 1, tableName, strlen(tableName), SQLITE_TRANSIENT); 
	ec = stepExecute(&cursor);
	uint32_t tableSize;
	fetchInt(&cursor, 0, &tableSize);	
	cursorExit(&cursor);
	return tableSize; 


}

