#include "DataBaseInterface.h"
//#include "py/objlist.h"

mp_obj_t usqlite_insertSoh(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_initializeDatabase(mp_obj_t self_in);
mp_obj_t usqlite_fetchSoh(size_t n_args,size_t n_kw, const mp_obj_t* args);
mp_obj_t usqlite_logEvent(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_insertCommand(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_getNextCommand(mp_obj_t self_in);
mp_obj_t usqlite_deleteCommand(mp_obj_t self_in, mp_obj_t index);
mp_obj_t usqlite_insertPayloadData(size_t n_args, size_t n_kw, const mp_obj_t* args);
mp_obj_t usqlite_fetchPayloadData(size_t n_args,size_t n_kw, const mp_obj_t* args);
mp_obj_t usqlite_deletePayloadDataID(mp_obj_t self_in, mp_obj_t index);
mp_obj_t usqlite_createUplink(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_fetchDataBlobInto(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_writeDataBlob(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_setUplinkPacketReceived(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_getMissingPacketIds(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_deleteEntry(size_t n_args, const mp_obj_t* args);



/**
 * @brief This is a description of insert soh
 *
 * @param sohEnum: integer, required,  
 * @param sohBytes: bytes-like object, required
 *
 * @return status of command
 */
mp_obj_t usqlite_insertSoh(size_t n_args, const mp_obj_t* args)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint32_t sohTimestamp = mp_obj_get_int(args[1]);
	mp_buffer_info_t bufinfo;
	mp_get_buffer(args[2], &bufinfo, MP_BUFFER_READ);
	int ec = insertSoh(self, sohTimestamp, bufinfo.buf, bufinfo.len);
	return mp_obj_new_int(ec);
}


/**
 * @brief Initialize the database tables, no-op if run while database is active
 * or if tables have already been initialized
 *
 * @return status of initialization
 */
mp_obj_t usqlite_initializeDatabase(mp_obj_t self_in)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(self_in);
	int res = initializeDatabase(self);
	return mp_obj_new_int(res);

}


/**
 * @brief Fetch soh from the database, either specify start/end times or specific index
 * def fetchSoh(self, sohEnum, index, start_time=None, end_time=None):
 *
 * @param index: integer, negative indicies supported, required if start/end times not present
 * @param start_time: time to start search for soh enum
 * @param stop_time: time to stop search for soh enum
 *
 * @return bytes object containg soh entry 
 */
mp_obj_t usqlite_fetchSoh(size_t n_args,const mp_obj_t* args)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint32_t index = mp_obj_get_int(args[1]);
	uint32_t timeStart = 0;
	uint32_t timeStop = 0;
	if (n_args > 2) timeStart = mp_obj_get_int(args[2]);
	if (n_args > 3) timeStop  = mp_obj_get_int(args[3]);


	uint8_t* data;
	size_t len;
	
	if ((timeStart == 0) && (timeStop == 0)) {
		int ret = fetchSoh(self, handleNegativeIndex(self, "soh", index), &data, &len);
	}
	else {
		int ret = fetchSohTimestamp(self, timeStart, timeStop, &data, &len);
	}
	return mp_obj_new_bytearray(len, data);
}



/**
 * @brief Log event in the database
 * def logEvent(self, level, info, timestamp, uptime):
 *
 * @param level: integer, required, how severe the event was
 * @param info: bytes-like, required, any information to the event
 * @param timestamp: integer, optional, unix timestamp if available
 * @param uptime: integer, uptime of mainboard in seconds
 *
 * @return int status of command 
 */
mp_obj_t usqlite_logEvent(size_t n_args, const mp_obj_t* args)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint8_t level = mp_obj_get_int(args[1]);
	mp_buffer_info_t bufinfo;
	mp_get_buffer(args[2], &bufinfo, MP_BUFFER_READ);
	uint32_t timestamp = mp_obj_get_int(args[3]);
	uint32_t uptime = mp_obj_get_int(args[4]);
	
	int ret = logEvent(self, level, bufinfo.buf,bufinfo.len,  timestamp, uptime);
	
	return mp_obj_new_int(ret);
	
}

/**
 * @brief insert command into database to be run
 * def insertCommand(self, cmdID, args, timestamp):
 *
 * @param cmdID: integer, required, corresponding to command in cdh dispatch
 * @param args: optional, any arguments to the command
 * @param timestamp: integer, when to execute the command. Either specify
 * an offset to current time (0 will be run as soon as possible), or unix timestamp
 * in UTC
 *
 * @return int status of command 
 */
mp_obj_t usqlite_insertCommand(size_t n_args, const mp_obj_t* args)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint8_t cmdID = mp_obj_get_int(args[1]);
	mp_buffer_info_t bufinfo;
	mp_get_buffer(args[2], &bufinfo, MP_BUFFER_READ);
	uint32_t timestamp = mp_obj_get_int(args[3]);
	int ret = insertCommand(self, cmdID, timestamp, bufinfo.buf, bufinfo.len);	
	return mp_obj_new_int(ret);
	
}


/**
 * @brief Get the next command to be run from the database according
 * to the timestamp provided
 * def getNextCommand(self):
 *
 * @param None
 *
 * @return list containing [index in database of command, 
 * 							integer command id,
 * 							bytes-like arguments to command
 * 							]
 */
mp_obj_t usqlite_getNextCommand(mp_obj_t self_in)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(self_in);
	uint8_t* cmdArgs;
	uint8_t cmd;
	size_t len = 0;
	size_t ind;
	
	int ret = getNextCommand(self, &cmd, &cmdArgs, &len, &ind);
	mp_obj_t args = mp_obj_new_bytearray(len, cmdArgs);
	mp_obj_t CID = mp_obj_new_int(cmd);
	mp_obj_t index = mp_obj_new_int(ind);
	mp_obj_t list[3] = {index, CID, args};
	return mp_obj_new_list(3, list);

}


/**
 * @brief Remove command from the database given the index in the database
 * def deleteCommand(self, index):
 *
 * @param index: integer corrsponding to command to delete, should be whatever was 
 * returned by get_NextCommand()
 * negative indices supporetd 
 *
 * @return int status of command
 */
mp_obj_t usqlite_deleteCommand(mp_obj_t self_in, mp_obj_t index)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(self_in);
	uint32_t id = handleNegativeIndex(self, "commands", mp_obj_get_int(index));

	return mp_obj_new_int(deleteCommand(self, id));
}


/**
 * @brief insert payload data into database. Either pass in raw bytes
 * or length of expected data to be used for incremental fileio.
 * def insertPayloadData(self, timestamp, pos, len):
 * def insertPayloadData(self, timestamp, pos, data, len):
 *
 * @param timestamp: integer unix timestamp of when the payload
 * event occurred
 * @param pos: xyz position of where the payload ocurred
 * (I think this is useful, if not this can be changed to something else
 * pretty easily that is more pertinent to the payload)
 * @param data: optional bytes-like object containing raw payload bytes
 * @param len: optinal integer length of data
 *
 * @return integer id in the databse
 */
mp_obj_t usqlite_insertPayloadData(size_t n_args, size_t n_kw, const mp_obj_t* args)
{
	enum {ARG_self, ARG_timestamp, ARG_pos, ARG_data, ARG_len, NUM_ARGS};
	static const mp_arg_t allowed_args[] = {
		{MP_QSTR_self, MP_ARG_OBJ, {.u_obj = mp_const_none}},
		{MP_QSTR_timestamp, MP_ARG_INT, {.u_int = 0}},
		{MP_QSTR_pos, MP_ARG_OBJ, {.u_obj = mp_const_none}},
		{MP_QSTR_data, MP_ARG_OBJ, {.u_obj = mp_const_none}},
		{MP_QSTR_len, MP_ARG_INT, {.u_int = 0}},
	};
	
	mp_arg_val_t arg_vals[MP_ARRAY_SIZE(allowed_args)];
	mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, arg_vals);
	
	
	usqlite_connection_t* self = MP_OBJ_TO_PTR(arg_vals[ARG_self].u_obj);
	uint32_t timestamp = mp_obj_get_int(arg_vals[ARG_timestamp].u_int);
	//switch to numpy array ?
	mp_buffer_info_t bufinfo;
	mp_get_buffer(arg_vals[ARG_pos].u_obj, &bufinfo, MP_BUFFER_READ);
	
	mp_buffer_info_t bufinfo2;
	mp_get_buffer(arg_vals[ARG_data].u_obj, &bufinfo2, MP_BUFFER_READ);

	size_t len = arg_vals[ARG_len].u_obj;
	int ret;
	if (len ==0) {
	
		ret = insertPayloadData(self, bufinfo2.buf, bufinfo2.len, bufinfo.buf, timestamp);
	}
	else {
		ret = createEmptyPayloadEntry(self, len, bufinfo.buf, timestamp);
	}
	return mp_obj_new_int(ret);
	
}


/**
 * @brief fetch payload data from the database, like soh, specify either index
 * or range of times.
 * def fetchPayloadData(self, index, pos=None, startTime=0, endTime=0):
 *
 *
 * @param index: integer, optional index of command in database. Negative indices are supported
 * @param start_time: integer, optional time to start searching for payload data
 * @param stop_time: integer, optional time to stop searching for payload data
 * @return list containing [ integer start time of experiment,
 * 							 integer stop time of experiment,
 * 							 bytes-like xyz position of experiment,
 * 							 bytes-like raw payload bytes
 * 							 ] 
 */
mp_obj_t usqlite_fetchPayloadData(size_t n_args,size_t n_kw, const mp_obj_t* args)
{
	
	enum {ARG_self, ARG_index, ARG_pos, ARG_startTime, ARG_endTime, NUM_ARGS};
	static const mp_arg_t allowed_args[] = {
		{MP_QSTR_self, MP_ARG_OBJ, {.u_obj = mp_const_none}},
		{MP_QSTR_index, MP_ARG_INT, {.u_int = 0}},
		{MP_QSTR_pos, MP_ARG_OBJ, {.u_obj = mp_const_none}},
		{MP_QSTR_start_time, MP_ARG_INT, {.u_int = 0}},
		{MP_QSTR_end_time, MP_ARG_INT, {.u_int = 0}},
	};
	
	mp_arg_val_t arg_vals[MP_ARRAY_SIZE(allowed_args)];
	mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, arg_vals);
	
	
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint32_t index = arg_vals[ARG_index].u_int;
	double pos[3];
	uint32_t timeStart = arg_vals[ARG_startTime].u_int;
	uint32_t timeStop  = arg_vals[ARG_endTime].u_int;
	
	

	uint8_t* buf;
	size_t len = 0;
	
	if ((timeStart !=0) && (timeStop !=0 )) {
		int ret = fetchPayloadDataTime(self, &index, buf, &len, pos, &timeStart, &timeStop);
	}
	else if (arg_vals[ARG_pos].u_obj != mp_const_none) {
		int ret = fetchPayloadDataPos(self, &index, buf, &len, pos, &timeStart);
	}
	else {
		int ret = fetchPayloadDataID(self, handleNegativeIndex(self, "payload", index), buf, &len, pos, &timeStart);
	}


	mp_obj_t data = mp_obj_new_bytearray(len, buf);
	mp_obj_t t1 = mp_obj_new_int(timeStart);
	mp_obj_t t2 = mp_obj_new_int(timeStop);
	mp_obj_t r = mp_obj_new_bytearray(3 * sizeof(double), pos);
	mp_obj_t list[3] = {t1,t2, r, data};
	return mp_obj_new_list(4, list);

}



/**
 * @brief Remove payload data from the database by specifying index
 * def deletePayloadDataID(self, index):
 *
 * @param index: integer, required, corresponding to index in database, 
 * negative indices supported
 *
 * @return int status of command
 */
mp_obj_t usqlite_deletePayloadDataID(mp_obj_t self_in, mp_obj_t index)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(self_in);
	uint32_t id = handleNegativeIndex(self, "payload", mp_obj_get_int(index));

	return mp_obj_new_int(deletePayloadDataID(self, id));
}




/**
 * @brief Create an uplink in the database
 * def createUplink(self, txID, txSize, numPackets):
 *
 * @param txID: integer, required transmission id of the uplink, see serial_protocol. 
 * @param txSize integer, required size of (only data section) transmission in bytes, see serial_protocol.
 * @param numPackets, integer, required, number of packets in the transmission
 * @param missing_pacekts: bytes-like object where each bit corresponds to the presence or absence
 * of that packet in the transmission. example:
 * bytearray(10) -> packets 0-80 are missing
 * bytearray([1]) -> packetID 1 has been recieved 
 * @return integer status of command 
 */
mp_obj_t usqlite_createUplink(size_t n_args, const mp_obj_t* args)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint32_t txID = mp_obj_get_int(args[1]);
	uint32_t txSize = mp_obj_get_int(args[2]);
	uint32_t numPackets = mp_obj_get_int(args[3]);
	
		
	int ret = createUplink(self, txID, txSize, numPackets);
	return mp_obj_new_int(ret);
	
}

/**
 * @brief Read part of a blob from a table into a bytes-like object
 * def fetchDataBlobInto(self, tableName, rowID, len, offset):
 *
 * @param tableName: required string corresponding to one of the tables, case sensitive
 * @param rowID: required integer row in the table the data blob exists
 * negative indices supported
 * @param len: amount of data to try to read
 * @param offset: offset in the blob to start reading
 * @param data: bytes-like object to read the blob into
 *
 * @return number of bytes read or error code <0
 */
mp_obj_t usqlite_fetchDataBlobInto(size_t n_args, const mp_obj_t* args)
{

	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	const char* tableName = mp_obj_str_get_str(args[1]);
	uint32_t rowID = handleNegativeIndex(self, tableName, mp_obj_get_int(args[2]));
	uint32_t len = mp_obj_get_int(args[3]);
	uint32_t offset = mp_obj_get_int(args[4]);

	mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(args[5], &bufinfo, MP_BUFFER_WRITE);
	if (bufinfo.len < len) {
		mp_raise_ValueError(MP_ERROR_TEXT("Buffer smaller than desired size"));
	}
	return mp_obj_new_int(fetchDataBlob(self, tableName, rowID, bufinfo.buf, len, offset));
}


//possibly put all delete commands in one and use kwargs to select right thing

mp_obj_t usqlite_deleteEntry(size_t n_args, const mp_obj_t* args)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint32_t id = mp_obj_get_int(args[1]);
	const char* tableName = mp_obj_str_get_str(args[2]);
	return mp_obj_get_int(deleteEntry(self, id, tableName));
}


/**
 * @brief Write data from a bytes-like object into a blob
 * in a sqlite3 table
 * def writeDataBlob(self, tableName, rowID, len, offset):
 *
 * @param tableName: required string corresponding to name of table
 * in the database, case sensitive.
 * @param rowID, required integer corresponding to the row of the desired blob
 * negative indices supported
 * @param len, required integer length of data to write
 * @param offset, required offset into the data blob to start writing
 * @param data, required bytes-like object containing data to write, must be at least
 * n bytes long
 *
 * @return int status of command
 */
mp_obj_t usqlite_writeDataBlob(size_t n_args, const mp_obj_t* args)
{

	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	const char* tableName = mp_obj_str_get_str(args[1]);
	uint32_t rowID = handleNegativeIndex(self, tableName, mp_obj_get_int(args[2]));
	uint32_t len = mp_obj_get_int(args[3]);
	uint32_t offset = mp_obj_get_int(args[4]);

	mp_buffer_info_t bufinfo;
	mp_get_buffer(args[5], &bufinfo, MP_BUFFER_READ);
	if (bufinfo.len < len) {
		mp_raise_ValueError(MP_ERROR_TEXT("Buffer smaller than desired size"));
	}
	return mp_obj_new_int(writeDataBlob(self, tableName, rowID, bufinfo.buf, len, offset));

}
//CURRENTLY THIS FUNCTION IS BROKEN
/**
 * @brief Get missing packet indicies
 * def getMissingPacketIds(self, txID):
 *
 * @param txID: integer transmission id
 * 
 * @return list containing indicies of missing packets for hte given transmission
 */
mp_obj_t usqlite_getMissingPacketIds(size_t n_args, const mp_obj_t* args)
{

	//mp_raise_ValueError(MP_ERROR_TEXT("howdy Owen 1"));	
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint32_t txID = mp_obj_get_int(args[1]);
	uint32_t* packetIds;
	size_t numMissing;
	
	getMissingPacketIds(self, txID, &packetIds, &numMissing);
	
	
	
	mp_obj_list_t* list = m_new_obj(mp_obj_list_t);
	mp_obj_list_init(list, numMissing);
	for (size_t i=0; i<numMissing; i++) {
		list->items[i] = mp_obj_new_int(packetIds[i]);
		//mp_raise_ValueError(MP_ERROR_TEXT("howdy 2"));
	}
	//m_free(packetIds);
	return list;
	return mp_const_none;
}

/**
 * @brief Set that a missing packet has been recieved in the missing packet
 * bitset
 * def setUplinkPacketReceived(self, txID, packetID, val=1):
 *
 * @param txID: id of the uplink in the database 
 * @param packetID: id of the packet in the uplink
 * @param val: optional value to set, defaults to true
 *
 * @return status of command 
 */
mp_obj_t usqlite_setUplinkPacketReceived(size_t n_args, const mp_obj_t* args)
{

	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint32_t txID = mp_obj_get_int(args[1]);
	uint32_t packetID = mp_obj_get_int(args[2]);
	uint8_t val = true;
	if (n_args > 3) {
		val = mp_obj_get_int(args[3]);
	}
	return mp_obj_new_int(setUplinkPacketReceived(self, txID, packetID, val));
}


