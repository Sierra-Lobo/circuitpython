#include "DataBaseInterface.h"


mp_obj_t usqlite_insertSoh(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_initializeDatabase(mp_obj_t self_in);
mp_obj_t usqlite_fetchSoh(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_logEvent(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_insertCommand(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_getNextCommand(mp_obj_t self_in);
mp_obj_t usqlite_deleteCommand(mp_obj_t self_in, mp_obj_t index);
mp_obj_t usqlite_insertPayloadData(size_t n_args, const mp_obj_t* args);
mp_obj_t usqlite_fetchPayloadDataID(mp_obj_t self_in, mp_obj_t index);
mp_obj_t usqlite_deletePayloadDataID(mp_obj_t self_in, mp_obj_t index);
mp_obj_t usqlite_createUplink(size_t n_args, const mp_obj_t* args);




mp_obj_t usqlite_insertSoh(size_t n_args, const mp_obj_t* args)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint8_t sohEnum = mp_obj_get_int(args[1]);
	mp_buffer_info_t bufinfo;
	mp_get_buffer(args[2], &bufinfo, MP_BUFFER_RW);
	if (insertSoh(self, sohEnum, bufinfo.buf, bufinfo.len) !=0){
        mp_raise_msg(&usqlite_Error, MP_ERROR_TEXT("Error inserting soh"));
		return mp_obj_new_int(-1);
	}	

	return mp_obj_new_int(0);

}


mp_obj_t usqlite_initializeDatabase(mp_obj_t self_in)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(self_in);
	int res = initializeDatabase(self);
	return mp_obj_new_int(res);

}

//check if can use keyword args to specify either timestamp or index
mp_obj_t usqlite_fetchSoh(size_t n_args, const mp_obj_t* args)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint8_t sohEnum = mp_obj_get_int(args[1]);
	uint32_t index = mp_obj_get_int(args[2]);
	uint8_t* data;
	size_t len;
	int ret = fetchSoh(self, sohEnum, index, data, &len);
	
	return mp_obj_new_bytearray(len, data);
	
}

mp_obj_t usqlite_logEvent(size_t n_args, const mp_obj_t* args)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint8_t level = mp_obj_get_int(args[1]);
	mp_buffer_info_t bufinfo;
	mp_get_buffer(args[2], &bufinfo, MP_BUFFER_RW);
	uint32_t timestamp = mp_obj_get_int(args[3]);
	uint32_t uptime = mp_obj_get_int(args[4]);
	
	int ret = logEvent(self, level, bufinfo.buf,bufinfo.len,  timestamp, uptime);
	
	return mp_obj_new_int(ret);
	
}

mp_obj_t usqlite_insertCommand(size_t n_args, const mp_obj_t* args)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint8_t cmdID = mp_obj_get_int(args[1]);
	mp_buffer_info_t bufinfo;
	mp_get_buffer(args[2], &bufinfo, MP_BUFFER_RW);
	uint32_t timestamp = mp_obj_get_int(args[3]);
	
	int ret = insertCommand(self, cmdID, timestamp, bufinfo.buf, bufinfo.len);	
	return mp_obj_new_int(ret);
	
}


mp_obj_t usqlite_getNextCommand(mp_obj_t self_in)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(self_in);
	uint8_t* buf;
	size_t len = 0;
	
	int ret = getNextCommand(self, buf, buf+2, &len, buf+1);
	mp_obj_t args = mp_obj_new_bytearray(len, buf+2);
	mp_obj_t CID = mp_obj_new_int(buf[0]);
	mp_obj_t index = mp_obj_new_int(buf[1]);
	mp_obj_t list[3] = {index, CID, args};
	return mp_obj_new_list(3, list);

}


mp_obj_t usqlite_deleteCommand(mp_obj_t self_in, mp_obj_t index)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(self_in);
	uint32_t id = mp_obj_get_int(index);

	return mp_obj_get_int(deleteCommand(self, id));
}


mp_obj_t usqlite_insertPayloadData(size_t n_args, const mp_obj_t* args)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint32_t timestamp = mp_obj_get_int(args[1]);
	//switch to numpy array ?
	mp_buffer_info_t bufinfo;
	mp_get_buffer(args[2], &bufinfo, MP_BUFFER_RW);
	
	mp_buffer_info_t bufinfo2;
	mp_get_buffer(args[2], &bufinfo2, MP_BUFFER_RW);
	int ret = insertPayloadData(self, bufinfo2.buf, bufinfo2.len, bufinfo.buf, timestamp);
	return mp_obj_new_int(ret);
	
}


//TODO same as soh, keword args to specify ID or timestamp
mp_obj_t usqlite_fetchPayloadDataID(mp_obj_t self_in, mp_obj_t index)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(self_in);
	uint32_t id = mp_obj_get_int(index);

	uint8_t* buf;
	size_t len = 0;
	double pos[3];
	uint32_t timestamp;
	
	int ret = fetchPayloadDataID(self, id, buf, &len, pos, &timestamp);
	mp_obj_t data = mp_obj_new_bytearray(len, buf);
	mp_obj_t t = mp_obj_new_int(timestamp);
	mp_obj_t r = mp_obj_new_bytearray(3 * sizeof(double), pos);
	mp_obj_t list[3] = {t, r, data};
	return mp_obj_new_list(3, list);

}



mp_obj_t usqlite_deletePayloadDataID(mp_obj_t self_in, mp_obj_t index)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(self_in);
	uint32_t id = mp_obj_get_int(index);

	return mp_obj_get_int(deletePayloadDataID(self, id));
}




mp_obj_t usqlite_createUplink(size_t n_args, const mp_obj_t* args)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(args[0]);
	uint32_t txID = mp_obj_get_int(args[1]);
	uint32_t txSize = mp_obj_get_int(args[2]);
	uint32_t numPackets = mp_obj_get_int(args[3]);
	//switch to numpy array ?
	mp_buffer_info_t bufinfo;
	mp_get_buffer(args[4], &bufinfo, MP_BUFFER_RW);
		
	int ret = createUplink(self, txID, txSize, numPackets, bufinfo.buf, bufinfo.len);
	return mp_obj_new_int(ret);
	
}

//possibly put all delete commands in one and use kwargs to select right thing
/*
mp_obj_t usqlite_deleteUplink(mp_obj_t self_in, mp_obj_t index)
{
	usqlite_connection_t* self = MP_OBJ_TO_PTR(self_in);
	uint32_t id = mp_obj_get_int(index);

	return mp_obj_get_int(deleteUplink(self, id));
}
*/




