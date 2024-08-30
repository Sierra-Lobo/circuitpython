#pragma once
#include "usqlite.h"
#include "py/obj.h"
#include "py/objarray.h"
///#include "py/binary.h"

/**
 * @brief Create non persistnent statement from the string and store in cursor
 *
 * @param self: database cursor object
 * @param sql: raw sql string to prepare
 *
 * @return status
 */
int createStatement(usqlite_connection_t* conn, usqlite_cursor_t* self, const char* sql);


/**
 * @brief Create persistnent statement from the string and store in cursor
 *
 * @param self: database cursor object
 * @param sql: raw sql string to prepare
 *
 * @return status
 */
int createStatementPersistent(usqlite_connection_t* conn, usqlite_cursor_t* self, const char* sql);


/**
 * @brief Get text from a sqlite3 row
 *
 * @param self usqlite cursor object
 * @param column: column in the row (0 baed index)
 * @param text: text output
 * @param len: output text length
 *
 * @return status
 */
status getText(usqlite_cursor_t* self, int column,  char** text, size_t* len);


/**
 * @brief Get blob from sqlite3 row, returns pointer
 *
 * @param self: usqlite cursor object
 * @param column: column in the row (0 based index)
 * @param blob: output pointer to the blob
 * @param n: output number of bytes in the blob
 *
 * @return status
 */
status getBlob(usqlite_cursor_t* self, int column,  uint8_t** blob, size_t* n);


/**
 * @brief Get blob from sqlite3 row, contents are copied to blob pointer
 *
 * @param self: usqlite cursor object
 * @param column: column in the row (0 based index)
 * @param blob: output array, contents are copied to blob
 * @param n: output number of bytes
 *
 * @return status
 */
status copyBlob(usqlite_cursor_t* self, int column,  uint8_t** blob, size_t* n);


/**
 * @brief Get double from sqlite3 row
 *
 * @param self: usqlite cursor object
 * @param column: column in the row output (0 based index)
 *
 * @return status
 */
double getDouble(usqlite_cursor_t* self, int column);


/**
 * @brief Get integer from sqlite3 row
 *
 * @param self: uslqite cursor object
 * @param column: column in the row output (0 based index)
 *
 * @return status
 */
int getInt(usqlite_cursor_t* self, int column);


/**
 * @brief Reset cursors statement and clear sqlite3 bindings 
 *
 * @param self: usqlite cursor object
 */
void resetStatement(usqlite_cursor_t* self);


/**
 * @brief Update the list of tables that have been initialized in the connection object
 * The table record is kept in a 32 bit interger and each table is bitmasked
 * using the tables enum in usqlite.h
 *
 * @param conn: usqlite connection object to store table record
 */
void updateTablesList(usqlite_connection_t* conn); 


/**
 * @brief get Maximum number of rows from table, assumes id is primary key
 *
 * @param self usqlite connection object
 * @param tableName string name of the table
 *
 * @return max number of rows
 */
uint32_t getMaxRows(usqlite_connection_t* self, const char* tableName);


/**
 * @brief Handle negative indices helper function. No-ops if the index is positive. Else returns
 * the id corresponding to the negative index supplied (ex -1 corresponds to last row in table, 
 * -2 second to last, etc)
 *
 * @param self usqlite connection object
 * @param tableName: string name of the table
 * @param index: positive or negative index
 *
 * @return interpreted index
 */
uint32_t handleNegativeIndex(usqlite_connection_t* self, const char* tableName,  int index);



void createBitset(uint8_t** bitset, size_t n);


bool getBitsetValue(uint8_t* bitset, size_t index);


void setBitsetValue(uint8_t* bitset, size_t idx, bool value);


int cursorExit(usqlite_cursor_t* self);

int handleRc(usqlite_cursor_t* self);
int fetchBlob(usqlite_cursor_t* self, size_t col, void** data, size_t* len);
int fetchInt(usqlite_cursor_t* self, size_t col, uint32_t* res);

int executeStmtNoReturn(usqlite_cursor_t* self);
int executeStr(usqlite_cursor_t* self, const char* sql);
mp_obj_t mp_obj_bytearray_move(size_t n, void *items);
