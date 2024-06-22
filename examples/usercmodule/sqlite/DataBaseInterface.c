#include "DataBaseInterface.h"
#include "databaseUtils.h"




status initializeDatabase(usqlite_connection_t* self)
{
	if (self->active) return SUCCESS;
		
	usqlite_cursor_t cursor;
	cursor.connection = self;
	int result= SUCCESS;
	
	//need way of checking if tables have already been made
	updateTablesList(self);
	if (!(self->tables & 1)) {	
		result = executeStr(&cursor, create_config);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
            mp_raise_ValueError(MP_ERROR_TEXT("result is not success"));
			return result;
		}
	}

	if (!(self->tables & (1 << COMMANDS))) {
		result = executeStr(&cursor, create_commands);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
			mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Cannot create commands table"));
			return result;
		}
		self->tables |= (1 << COMMANDS);
	}

	if (!(self->tables & (1 << SOH))) {
		result = executeStr(&cursor, create_soh);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
			mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Cannot create soh table"));
			return result;
		}
		self->tables |= (1 << SOH);
	}

	if (!(self->tables & (1 << DOWNLINKS))) {
		result = executeStr(&cursor, create_downlinks);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
			mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Cannot create downlinks table"));
			return result;
		}
		self->tables |= (1 << DOWNLINKS);
	}

	if (!(self->tables & (1 << EVENTS))) {
		result = executeStr(&cursor, create_events);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
			mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Cannot create events table"));
			return result;
			}
		self->tables |= (1 << EVENTS);
		}
	if (!(self->tables & (1 << UPLINKS))) {
		result = executeStr(&cursor, create_uplinks);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
			mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Cannot create uplinks table"));
			return result;
		}
		self->tables |= (1 << UPLINKS);
	}
	
	if (!(self->tables & (1 << PAYLOAD))) {
		result = executeStr(&cursor, create_payload);
		if (result != SQLITE_DONE) {
			sqlite3_close(self->db);
			mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Cannot create uplinks table"));
			return result;
		}
		self->tables |= (1 << PAYLOAD);
	}

	self->active = 1;
	return result;


}

int insertSoh(usqlite_connection_t* self, uint8_t sohEnum, uint8_t* data, size_t len) {
	//not real just for test vvv
	sqlite3_blob* blob;
	uint32_t TXID = 1;
	int ec = sqlite3_blob_open( self->db, "main", "downlinks", "data", TXID, 1, &blob );
	ec = sqlite3_blob_write(blob, data, len, 0);
	sqlite3_blob_close(blob);

	uint8_t* addr;
	size_t length = 0;
	int prot = 0;
	int flags = 0;
	int fd = 0;
	int offset = 0;
	return ec;
	//end not real just for test ^^^
}









//TODO make sure negatie indicies work
int fetchSoh(usqlite_connection_t* self, uint8_t sohEnum, uint32_t index, uint8_t* data, size_t* len) {

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_soh_index);
	sqlite3_bind_int64(cursor.stmt, 0, index);
	stepExecute(&cursor);
	uint32_t timestamp = sqlite3_column_int64(cursor.stmt, 0);
	getBlob(&cursor, 1, &data, len);	
	return timestamp; //?
}

int fetchSohTimestamp(usqlite_connection_t* self, uint8_t sohEnum, uint32_t timeStart, uint32_t timeEnd, uint8_t* data, size_t* len) {
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_soh_time);
	sqlite3_bind_int64(cursor.stmt, 0, timeStart);
	sqlite3_bind_int64(cursor.stmt, 1, timeEnd);
	stepExecute(&cursor);
	uint32_t timestamp = sqlite3_column_int64(cursor.stmt, 0);
	getBlob(&cursor, 1, &data, len);	
	return timestamp; //?

}

int logEvent(usqlite_connection_t* self, uint8_t level, const char* msg, size_t n, uint32_t timestamp, double uptime ){
	
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_event);
	sqlite3_bind_int(cursor.stmt, 0, level);
	sqlite3_bind_int64(cursor.stmt, 1, timestamp);
	sqlite3_bind_int64(cursor.stmt, 1, uptime);
	sqlite3_bind_text(cursor.stmt, 2, msg, n, SQLITE_TRANSIENT); 
	stepExecute(&cursor);
	return SUCCESS; 
}


int insertCommand(usqlite_connection_t* self, uint8_t cmdID, double dueTime, uint8_t* args, size_t len ){

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_command);
	sqlite3_bind_int64(cursor.stmt, 0, dueTime);
	sqlite3_bind_int(cursor.stmt, 1, cmdID);
	sqlite3_bind_blob(cursor.stmt, 2, args, len, SQLITE_TRANSIENT);
	stepExecute(&cursor);
	return SUCCESS; 

}


int getNextCommand(usqlite_connection_t* self, uint8_t* cmdID, uint8_t* args, size_t* len, uint32_t* index){

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_command);
	stepExecute(&cursor);
	*index = getInt(self, 0);
	int32_t ctime = getInt(self, 1);
	*cmdID = getInt(self, 2);
	getBlob(self, 3, &args, len);
	return SUCCESS; 

}


int deleteCommand(usqlite_connection_t* self, uint32_t index){

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, delete_command);
	sqlite3_bind_int64(cursor.stmt, 0, index);
	stepExecute(&cursor);
	return SUCCESS; 

}



int insertPayloadData(usqlite_connection_t* self, uint8_t* data, size_t len,  double pos[3], uint32_t timestamp ) {
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_payload);
	sqlite3_bind_int64(cursor.stmt, 0, timestamp);
	sqlite3_bind_blob(cursor.stmt, 1, pos, 3 * sizeof(double), SQLITE_TRANSIENT);
	sqlite3_bind_blob(cursor.stmt, 2, data, len, SQLITE_TRANSIENT);
	stepExecute(&cursor);
	return SUCCESS; 

}


int fetchPayloadDataID(usqlite_connection_t* self, uint32_t index, uint8_t* data, size_t* len, double pos[3], uint32_t* timestamp) {


	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_payload_index);
	sqlite3_bind_int64(cursor.stmt, 0, index);
	stepExecute(&cursor);
	*timestamp = sqlite3_column_int64(cursor.stmt, 0);
	getBlob(&cursor, 1, &data, len);
	size_t poslen;	
	getBlob(&cursor, 2, &pos, &poslen);	
	return SUCCESS; //?
}

int fetchPayloadDataTime(usqlite_connection_t* self, uint32_t* index, uint8_t* data,size_t* len,  double pos[3], uint32_t* timeStart, uint32_t* timeStop) {

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_payload_time);
	sqlite3_bind_int64(cursor.stmt, 0, *timeStart);
	sqlite3_bind_int64(cursor.stmt, 0, *timeStop);
	stepExecute(&cursor);
	*timeStart= sqlite3_column_int64(cursor.stmt, 0);
	getBlob(&cursor, 1, &data, len);
	size_t poslen;	
	getBlob(&cursor, 2, &pos, &poslen);	
	return SUCCESS; //?
	
}


int fetchPayloadDataPos(usqlite_connection_t* self, uint32_t* index, uint8_t* data,size_t* len,  double pos[3], double* timestamp) {
	
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_payload_pos);
	sqlite3_bind_int64(cursor.stmt, 0, *timestamp);
	stepExecute(&cursor);
	*timestamp= sqlite3_column_int64(cursor.stmt, 0);
	getBlob(&cursor, 1, &data, len);
	size_t poslen;	
	getBlob(&cursor, 2, &pos, &poslen);	
	return SUCCESS; //?


}



int deletePayloadDataID(usqlite_connection_t* self, uint32_t index) {

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, delete_payload);
	sqlite3_bind_int64(cursor.stmt, 0, index);
	stepExecute(&cursor);
	return SUCCESS; 

}






int createUplink(usqlite_connection_t* self, uint32_t txID, uint32_t txSize, uint32_t numPackets, uint8_t* missing, uint32_t missingSize) {

	usqlite_cursor_t cursor;
	createStatement(self, &cursor, insert_uplink);
	sqlite3_bind_int64(cursor.stmt, 0, txID);
	sqlite3_bind_int64(cursor.stmt, 1, txSize);
	sqlite3_bind_blob(cursor.stmt, 2, missing, missingSize, SQLITE_TRANSIENT);
	sqlite3_bind_int64(cursor.stmt, 3, numPackets);
	stepExecute(&cursor);
	return SUCCESS; 

}



/*
int insertUplinkPacket(usqlite_connection_t* self,uint32_t txID, uint32_t packetID,  uint8_t* data, size_t len) {

 sqlite_blob_write ???
}
*/

int deleteUplink(usqlite_connection_t* self, uint32_t uplinkID) {
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, delete_uplink);
	sqlite3_bind_int64(cursor.stmt, 0, uplinkID);
	stepExecute(&cursor);
	return SUCCESS; 


}


int createDownlink(usqlite_connection_t* self, uint32_t downlinkID, uint8_t* data, size_t len) {


}


int getMissingPacketIds(usqlite_connection_t* self, uint32_t txID, uint8_t* packetIDs, size_t* len)
{
	usqlite_cursor_t cursor;
	createStatement(self, &cursor, query_missing_uplink);
	sqlite3_bind_int64(cursor.stmt, 0, txID);
	stepExecute(&cursor);
	getBlob(&cursor, 0, &packetIDs, len); 
}



int fetchDataBlob(usqlite_connection_t* self, const char* tableName, uint32_t rowID, uint8_t* data, size_t len, size_t offset)
{
	sqlite3_blob* blob;
	
	int ec = sqlite3_blob_open( self->db, "main", tableName, "data", rowID, 0, &blob );
	int available = sqlite3_blob_bytes(blob) - offset;
	len = len > available ?  available : len;
	ec = sqlite3_blob_read(blob, data, len, offset);
	ec =  sqlite3_blob_close(blob);
	return ec;
}

int writeDataBlob(usqlite_connection_t* self, const char* tableName, uint32_t rowID, uint8_t* data, size_t len, size_t offset)
{
	sqlite3_blob* blob;
	
	int ec = sqlite3_blob_open( self->db, "main", tableName, "data", rowID, 1, &blob );
	int available = sqlite3_blob_bytes(blob) - offset;
	len = len > available ?  available : len;
	ec = sqlite3_blob_write(blob, data, len, offset);
	ec =  sqlite3_blob_close(blob);
	return ec;
}


