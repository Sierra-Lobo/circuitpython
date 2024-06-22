#include "SQL_Statements.h"
#include "stdint.h"
#include "usqlite.h"
#include "usqlite_mem.h"
#include "usqlite_cursor.h"

#define SOH_SIZE 512
#define MAX_CMD 512

#ifndef DATABASE_INTERFACE_H
#define DATABASE_INTERFACE_H


/**
 * @brief Initialize all tables in the database
 *
 * @param self usqlite connection object
 *
 * @return status
 */
status initializeDatabase(usqlite_connection_t* self);





/**
 * @brief execute raw sql cmd
 *
 * @param self: usqlite connection object with database info
 * @param raw: Raw sql command string
 *
 * @return status
 */
int executeStr(usqlite_cursor_t* self, const char* raw);


/**
 * @brief execute raw sql stmt
 *
 * @param self: usqlite connection object with database info
 *
 * @return status
 */
int executeStmtNoReturn(usqlite_cursor_t* self); 


/**
 * @brief Insert bytes into soh table
 *
 * @param self: usqlite connection object with database info
 * @param sohEnum: unsinged integer corresponding to appropriate soh subsystem
 * @param data: pointer to raw soh struct bytes
 * @param len: number of bytes to write
 *
 * @return status
 */
int insertSoh(usqlite_connection_t* self, uint8_t sohEnum, uint8_t* data, size_t len);


/**
 * @brief Return soh
 *
 * @param self: usqlite connection object with database info
 * @param sohEnum: enum correspoding to appropriate soh subsystem
 * @param index: index of soh entry, negative entries work relative to most recent
 * @param data: output soh bytes
 * @param len: number of bytes written to data
 *
 * @return status
 */
int fetchSoh(usqlite_connection_t* self, uint8_t sohEnum, uint32_t index, uint8_t* data, size_t* len);


/**
 * @brief Return soh by timestamp
 *
 * @param self: usqlite connection object with database info
 * @param sohEnum: enum corresponding to appropriate soh subsystem
 * @param timestamp: timestamp of desired soh entry
 * @param data: output soh bytes
 * @param len: number of bytes written to data
 *
 * @return status
 */
int fetchSohTimestamp(usqlite_connection_t* self, uint8_t sohEnum, uint32_t timeStart, uint32_t timeEnd, uint8_t* data, size_t* len);


/**
 * @brief Log notable event in the database
 *
 * @param self: usqlite connection object with database info
 * @param level: severity of the event, critial, warning, anomaly
 * @param msg: succint string describing the event
 * @param timestamp: when the event occured
 * @param uptime: when the event occured relative to the last boot
 *
 * @return status
 */
int logEvent(usqlite_connection_t* self, uint8_t level, const char* msg, size_t n, uint32_t timestamp, double uptime );


/**
 * @brief Insert command in the database to be executed. If both dueTime and offset are
 * DOUBLE_MIN then the commands are executed immediately. If both are specified whichever
 * occurs first is when the command executes. Only use duetime if accurate timekeeping 
 * onboard has been verified
 *
 * @param self: usqlite connection object with database info
 * @param cmdID: ID of the command in the cdh cmd dispatch
 * @param dueTime: unix timestamp in UTC of when the command should execute.
 * @param args: arguments to the command
 * @param len: length of the arguments
 *
 * @return status
 */
int insertCommand(usqlite_connection_t* self, uint8_t cmdID, double dueTime, uint8_t* args, size_t len );


/**
 * @brief Check the database for any commands that are due (see insertCommand)
 * cmdID will be <0 if no commands are due
 *
 * @param self: usqlite connection object with database info
 * @param cmdID: output id of the command to execute
 * @param args: output args of the command to execute
 * @param len: output len of the args
 * @param index: output index in the commands table of the command (for deletion)
 *
 * @return status
 */
int getNextCommand(usqlite_connection_t* self, uint8_t* cmdID, uint8_t* args, size_t* len, uint32_t* index);


/**
 * @brief delete command from the database
 *
 * @param self: usqlite connection object with database info
 * @param index: row index of the command to delete
 *
 * @return status
 */
int deleteCommand(usqlite_connection_t* self, uint32_t index);


/**
 * @brief Insert payload data into the database TODO: anything else desired?
 *
 * @param self: usqlite connection object with database info
 * @param data: raw payload data bytes
 * @param len: number of payload data bytes
 * @param pos[3]: position of the paylaod (I think this is important)
 * @param timestamp: timestamp of the experiment
 *
 * @return status
 */
int insertPayloadData(usqlite_connection_t* self, uint8_t* data, size_t len,  double pos[3], uint32_t timestamp );


/**
 * @brief Return payload data corresponding to the given index
 *
 * @param self: usqlite connection object with database info
 * @param index: index in the datatabase, negative indices are relative to most recent entry
 * @param data: output raw payload data
 * @param len: output number of raw payload bytes
 * @param pos[3]: position where experiment occured
 * @param timestamp: unix time where experiment occured
 *
 * @return status
 */
int fetchPayloadDataID(usqlite_connection_t* self, uint32_t index, uint8_t* data, size_t* len, double pos[3], uint32_t* timestamp);


/**
 * @brief Return payload data corresponding to the given time
 *
 * @param self: usqlite connection object with database info
 * @param index : output index in the database
 * @param data: output paylaod bytes
 * @param len: output number of payload bytes
 * @param pos[3]: output position of the payload
 * @param timestamp: timestamp to search for (start bound inclusive), returns timestamp found
 * @param timeStop: timestamp to search for (end bound inclusive), returns timestamp found
 *
 * @return status
 */
int fetchPayloadDataTime(usqlite_connection_t* self, uint32_t* index, uint8_t* data, size_t* len, double pos[3], uint32_t* timestamp, uint32_t* timeStop);


/**
 * @brief Return payload data corresponding to the given position
 *
 * @param self: usqlite connection object with database info
 * @param index: output index in the database
 * @param data: output raw payload bytes
 * @param len: output number of payload bytes
 * @param pos[3]: specified position to search, nearest entry will be output
 * @param timestamp: output timestamp of the payload
 *
 * @return status
 */
int fetchPayloadDataPos(usqlite_connection_t* self, uint32_t* index, uint8_t* data,size_t* len,  double pos[3], double* timestamp);


/**
 * @brief Delete payload data by specifying index in the database
 *
 * @param self: usqlite connection object with database info
 * @param index: index to delete, negative will be in reference to the most recent
 *
 * @return status
 */
int deletePayloadDataID(usqlite_connection_t* self, uint32_t index);


/**
 * @brief Delete payload data by specifying timestamp in the database
 *
 * @param self: usqlite connection object with database info
 * @param timestamp: timestamp to search for and delete entry
 *
 * @return status
 */
int deletePayloadDataTime(usqlite_connection_t* self, double timestamp);


/**
 * @brief Delete payload data by specifying position in the database
 *
 * @param self: usqlite connection object with database info
 * @param pos[3]: position to search for and delete entry
 *
 * @return status
 */
int deletePayloadDataPos(usqlite_connection_t* self, double pos[3]);


/**
 * @brief Create empty uplink in database
 *
 * @param self usqlite connection object
 * @param txID: id of the transmission
 * @param txSize: total size of transmission in bytes
 * @param numPackets: number of packets in bytes
 * @param missing: missing packet bitset
 * @param missingSize: size of missing packet bitset
 *
 * @return status
 */
int createUplink(usqlite_connection_t* self, uint32_t txID, uint32_t txSize, uint32_t numPackets, uint8_t* missing, uint32_t missingSize);


/**
 * @brief Insert uplink into database
 *
 * @param self: usqlite connection object with database info
 * @param data: raw uplink bytes
 * @param len: number of uplink bytes
 *
 * @return status
 */
int insertUplinkPacket(usqlite_connection_t* self,uint32_t txID, uint32_t packetID,  uint8_t* data, size_t len);


/**
 * @brief Delete uplink corresponding to the given uplink ID
 *
 * @param self: usqlite connection object with database info
 * @param uplinkID: ID to search for and delete
 *
 * @return status
 */
int deleteUplink(usqlite_connection_t* self, uint32_t uplinkID);


/**
 * @brief Add new downlink in he database
 *
 * @param self: usqlite connection object with database info
 * @param downlinkID: Unique identifier for the downlink
 * @param data: raw bytes for the transmission
 * @param len: number of bytes for the transmission
 *
 * @return status
 */
int createDownlink(usqlite_connection_t* self, uint32_t downlinkID, uint8_t* data, size_t len);


/**
 * @brief Return downlink specified by the downlink ID
 *
 * @param self: usqlite connection object with database info
 * @param downlinkID: unique idenfitifer to search database for
 * @param data: output raw bytes for the transmission
 * @param len: output number of bytes for the transmission
 *
 * @return status
 */
int fetchDownlink(usqlite_connection_t* self, uint32_t downlinkID, uint8_t* data, size_t* len);


/**
 * @brief Search the database for the next downlink to send
 *
 * @param self: usqlite connection object with database info
 * @param data: output raw transmission bytes
 * @param len: output number of bytes
 *
 * @return status
 */
int fetchNextDownlink(usqlite_connection_t* self,  uint8_t* data, size_t* len);


/**
 * @brief Remove downlink from the database with the given ID
 *
 * @param self: usqlite connection object with database info
 * @param downlinkID: ID to search for and remove from database
 *
 * @return status
 */
int deleteDownlink(usqlite_connection_t* self, uint32_t downlinkID);


/**
 * @brief Return a specific packet from a downlink 
 *
 * @param self: usqlite connection object with database info
 * @param packetID: identifier of the packet within a transmission
 * @param downlinkID: identifier of the downlink for a transmission
 * @param data: output bytes of the specified packet
 * @param len: output len of the specified packet
 *
 * @return status
 */
int fetchDownlinkPacket(usqlite_connection_t* self, uint32_t packetID,uint32_t downlinkID, uint8_t* data, size_t* len);


/**
 * @brief Remove all entries in the specified table
 *
 * @param self: usqlite connection object with database info
 * @param tableName: Name of the table in the database
 * 
 * @return status
 */
int clearTable(usqlite_connection_t* self, char* tableName);





int getMissingPacketIds(usqlite_connection_t* self, uint32_t txID, uint8_t* packetIDs, size_t* len);


/**
 * @brief Read a blob from a table for incremental io
 *
 * @param self usqlite connection object
 * @param tableName name of the table
 * @param rowID row index to select (primary key)
 * @param data output data
 * @param len desired len (if available)
 * @param offset desired offset
 *
 * @return status
 */
int fetchDataBlob(usqlite_connection_t* self, const char* tableName, uint32_t rowID, uint8_t* data, size_t len, size_t offset);


/**
 * @brief Write a blob of data into a table in the database
 *
 * @param self usqlite connection object
 * @param tableName name of the table
 * @param rowID row index in table
 * @param data input data bytes
 * @param len number of data bytes
 * @param offset offset in the database blob
 *
 * @return status
 */
int writeDataBlob(usqlite_connection_t* self, const char* tableName, uint32_t rowID, uint8_t* data, size_t len, size_t offset);
#endif //DATABASE_INTERFACE_H
