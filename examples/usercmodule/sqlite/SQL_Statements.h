/**
 * @file SQL_Statements.h
 * @brief SQL statements used frequently by functions
 * @author Owen DelBene
 * @date 2024-07-12
 */
#include "string.h"




  /*
   * **********************************
   * DATABASE TABLE CREATION STATEMENTS
   * **********************************
   */

  /**
   * @brief get max number of rows in a table ordered by id
   * used for negative index support
   * @author Owen DelBene
   * 7/12/2024
   */



  /**
   * @brief Create the configs table
   *
   * @author Owen DelBene
   * 6/8/2024
   */
  #define create_config  \
      "CREATE TABLE configs("\
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
      "data BLOB NOT NULL);"

  /**
   * @brief Create the commands table
   *
   * @author Owen DelBene
   * 6/8/2024
   */
  #define create_commands \
      "CREATE TABLE commands("\
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
      "time INTEGER NOT NULL,"\
      "cid INTEGER NOT NULL,"\
      "data BLOB);"

  /**
   * @brief Create the SOH table
   *
   * @author Owen DelBene
   * 6/8/2024
   */
  #define create_soh \
      "CREATE TABLE soh("\
	  "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
      "time INTEGER,"\
      "data BLOB NOT NULL);"


  /**
   * @brief Create the downlinks table
   *
   * @author Owen DelBene
   * @date 6/8/2024
   */
  #define create_downlinks  \
      "CREATE TABLE downlinks("\
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
      "data BLOB NOT NULL," \
      "priority INTEGER NOT NULL,"\
      "sent BLOB NOT NULL,"\
      "npackets INTEGER NOT NULL);"

  /**
   * @brief Create the uplinks table
   *
   * @author Owen DelBene
   * @date 6/8/2024

   */
  #define create_uplinks \
      "CREATE TABLE uplinks("\
      "id INTEGER PRIMARY KEY,"\
      "numPckts INTEGER NOT NULL,"\
      "data BLOB NOT NULL," \
      "missing BLOB NOT NULL);"

  /**
   * @brief Create the events table
   *
   * @author Owen DelBene
   * @date 6/8/2024
   */
  #define create_events  \
      "CREATE TABLE events(" \
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
      "severity INTEGER NOT NULL,"\
      "time INTEGER ,"\
      "uptime INTEGER NOT NULL,"\
      "data TEXT NOT NULL);"
  
  /**
   * @brief Create the payload data table
   *
   * @author Owen DelBene
   * 6/8/2024
   */
  #define create_payload \
      "CREATE TABLE payload("\
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
      "time INTEGER ,"\
      "pos BLOB ,"\
      "data BLOB NOT NULL );"


  /*
   * **********************************
   * DATABASE TABLE QUERY STATEMENTSS
   * **********************************
   */

  /**
   * @brief Query the next command from the database
   *
   * This takes no parameters, as it returns the next command
   * to be run. The command returned may not actually be due
   * to be executed.
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @returns The ID, time, command ID, and parameters of the next command to be
   * run
   */
  #define query_command \
      "SELECT MIN(id), time, cid, data FROM commands WHERE "\
      "time=(SELECT MIN(time) FROM commands);"

  /**
   * @brief Query the value of a config parameter
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The id of the configuration variable to fetch
   * @return Return the ID requested and the value of the parameter as a blob
   */
  #define query_config \
      "SELECT id, value FROM configs WHERE id=?1;"
  /**
   * @brief Query the database for all configs and their values
   *
   * @author Owen DelBene
   * @date 6/8/2024
   */
  #define query_all_config \
      "SELECT id, value FROM configs;"

  /**
   * @brief Query all SOH data between two time intervals 
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp to fetch
   *
   * @return The time and SOH data for all SOHs occurring at or after the given
   * time
   */
  #define query_soh_time \
      "SELECT time, data FROM soh WHERE time >= ?1 AND time <= ?2;"
  
  /**
   * @brief Query all SOH data given an index 
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp to fetch
   *
   * @return The time and SOH data for all SOHs occurring at or after the given
   * time
   */
  #define query_soh_index \
      "SELECT time, data FROM soh WHERE id =?1;"



  /**
   * @brief Query all config data given an index 
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp to fetch
   *
   * @return The time and SOH data for all SOHs occurring at or after the given
   * time
   */
  #define query_config_index \
      "SELECT data FROM configs WHERE id =?1;"



  /**
   * @brief Query all payload data between two time intervals 
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp to fetch
   *
   * @return The time and SOH data for all SOHs occurring at or after the given
   * time
   */
  #define query_payload_time  \
      "SELECT time, data, pos FROM soh WHERE time >= ?1 AND time <= ?2;"
  
  /**
   * @brief Query all payload data by index
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The index to fetch
   *
   * @return The time and payload data for given indexn
   *
   */
  #define query_payload_index \
      "SELECT time, data, pos FROM payload WHERE id =?1;"
  
  /**
   * @brief Query all payload data by position 
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The position to search for 
   *
   * @return The time and payload data for given indexn
   *
   */
  #define query_payload_pos \
      "SELECT time, data, pos FROM payload WHERE pos =?1;"
  /**
   * @brief Query last number of SOH entries 
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The number of SOH structs to return
   *
   * @returns A queryset of SOH rows.
   */
  #define query_soh_num \
      "SELECT time, data FROM soh ORDER BY time DESC LIMIT ?1;"

  /**
   * @brief Query the next downlink to send 
   *
   * Gets the next highest priority downlink
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @return The next data to be downlinked
   */
  #define query_downlink \
      "SELECT MIN(id), data FROM downlinks "\
      "WHERE priority = (SELECT MAX(priority) FROM downlinks);"

  #define query_downlink_id \
      "SELECT MIN(id) FROM downlinks "\
      "WHERE priority = (SELECT MAX(priority) FROM downlinks)" 

  /**
   * @brief Query the missing packets bitset from an uplink
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The transmission ID of the uplink
   */
  #define query_missing_uplink \
      "SELECT numPckts, missing FROM uplinks WHERE id = ?1;"

  #define query_resend_requests \
      "SELECT id FROM uplinks WHERE AllBitsUnset(numPckts, missing) = 0;"

  /**
   * @brief Query the data vector from an uplink
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The transmission ID of the uplink
   */
  #define query_uplink_data \
      "SELECT data FROM uplinks WHERE id = ?1;"


  /**
   * @brief Query the database statistics table and get the sum of the all allocated page sizes for the queried table.
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The name of the table to query.
   *
   * @returns an integer number of bytes, rounded to the page size of OS.
   */
  #define query_table_size \
      " SELECT SUM(pgsize) FROM dbstat WHERE name LIKE ?1;"

  /*
   * **********************************
   * DATABASE TABLE INSERT STATEMENTS
   * **********************************
   */

  /**
   * @brief Insert experiment data into the database
   * returns the id in the database 
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The time th experiment occured
   * @param ?2 The position the experiment occurred
   * @param ?3 The raw experiment data
   */
  #define insert_payload \
      "INSERT INTO payload(time, pos, data) VALUES(?1, ?2, ?3) RETURNING id;"
 
  /**
   * @brief Insert empty experiment data into database
   * designed to write the data incrementally via blob
   * if data will consume too much ram at once 
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The time the experiment occurred 
   * @param ?2 The position the experiment occurred
   * @param ?3 The length of the raw experiment data
   */
  #define insert_payload_empty \
      "INSERT INTO payload(time, pos, data) VALUES(?1, ?2, zeroblob(?3)) RETURNING id;"

  /**
   * @brief Insert command into database 
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The time the command should execute 
   * @param ?2 The id of the command to execute in cdh dispatch
   * @param ?3 The arguments to the command
   */
  #define insert_command \
      "INSERT INTO commands(time, cid, data) VALUES(?1, ?2, ?3);"

  /**
   * @brief Insert or replace a config variable
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The ID of the config parameter
   * @param ?2 The new value of the parameter
   */
  #define upsert_config \
      "REPLACE INTO configs VALUES(?1, ?2);"

  /**
   * @brief Insert an SOH 
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp of the soh 
   * @param ?2 The SOH Data blob
   */
  #define insert_soh \
      "INSERT INTO soh(time, data) VALUES(?1, ?2);"

  /**
   * @brief Insert a config into database
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp of the soh 
   * @param ?2 The SOH Data blob
   */
  #define insert_config \
      "INSERT INTO configs(data) VALUES(?1);"

  /**
   * @brief Insert a downlink into the database
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The downlink's raw byte vector
   * @param ?2 The downlink's priority
   */
  #define insert_downlink \
      "INSERT INTO downlinks(data, priority, sent, npackets) VALUES(?1, ?2, "\
      "?3, ?4) returning id;"

  #define get_downlink_sent \
      "SELECT npackets, sent FROM downlinks WHERE id = ?1;"

  #define set_downlink_sent \
      "UPDATE downlinks SET sent = ?1 WHERE id = ?2;"

  #define insert_downlink_empty \
      "INSERT INTO downlinks(data, priority, sent, npackets) VALUES(zeroblob(?1), ?2, "\
      "zeroblob(?3), ?4) returning id;"


  /**
   * @brief Insert an empty uplink into the database with the final size
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The desired transmission ID
   * @param ?2 The size of the final uplink in bytes
   * @param ?3 The "missing packets" bitset
   * @param ?4 The number of packets in the uplink
   */
  #define insert_uplink \
      "INSERT INTO uplinks VALUES(?1, ?4, zeroblob(?2), ?3);"

  /**
   * @brief Insert a new event
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The severity of the event
   * @param ?2 The timestamp 
   * @param ?2 The uptime 
   * @param ?3 The provided human readable information
   */
  #define insert_event \
      "INSERT INTO events(severity,  time, uptime, data) VALUES(?1, ?2, ?3, ?4);"


  /**
   * @brief Update the missing packets section of an uplink
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The new missing byte vector
   * @param ?2 The transmission ID of the uplink
   */
  #define update_missing_uplink \
      "UPDATE uplinks SET missing = ?1 WHERE id = ?2;"

  /*
   * **********************************
   * DATABASE TABLE DELETE STATEMENTS
   * **********************************
   */

  /**
   * @brief Delete the command to be run most recently, and return it.
   *
   * Warning: This will not check that the command is due to be executed
   * or has been executed before being deleted.
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @returns The ID, time, command ID, and parameters of the deleted command
   */
  #define pop_last_command \
      "DELETE FROM commands WHERE "\
      "id = (SELECT MIN(id) FROM commands WHERE "\
      "time = (SELECT MIN(time) FROM commands)) "\
      "RETURNING id, time, cid, data;"
  
  /**
   * @brief Delete command from the database given index 
   *
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The index of the command to delete.
   */
  #define delete_command \
      "DELETE FROM COMMANDS WHERE id = ?1;"

  /**
   * @brief Delete payload from the database given index 
   *
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The index of the payload entry to delete.
   */
  #define delete_payload \
      "DELETE FROM PAYLOAD WHERE id = ?1;"
  /**
   * @brief Delete all SOH data between the given times 
   *
   * The delete is inclusive -- Any SOH with the given timestamp will be deleted.
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp of the newest SOH to delete.
   */
  #define delete_soh \
      "DELETE FROM soh WHERE time <= ?2 AND time >= ?1;"
  
/**
   * @brief Delete all SOH data given id 
   *
   * The delete is inclusive -- Any SOH with the given timestamp will be deleted.
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp of the newest SOH to delete.
   */
  #define delete_soh_id \
      "DELETE FROM SOH WHERE id=?1;"

/**
 * @brief Delete entry from table specified by tableName and index
 *
 * @author Owen DelBene
 * 7/21/2024
 */
  #define delete_from_table \
	  "DELETE FROM ?1 WHERE id=?2;"

  /**
   * @brief Delete the oldest downlink (the one least recently downlinked) and
   * return it
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @returns The data for the downlink that was deleted
   */
  #define delete_downlink \
      "DELETE FROM downlinks WHERE "\
      "id = (SELECT MIN(id) FROM downlinks WHERE "\
      "priority=(SELECT MAX(priority) FROM downlinks)"\
      ") "\
      "RETURNING id, data;"

  #define delete_downlink_id \
      "DELETE FROM downlinks WHERE "\
      "id = ?1;"

  /**
   * @brief Delete the oldest uplink (the one least recently uplinked) and
   * return it
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @returns The data for the uplink that was deleted
   */
  #define delete_uplink \
      "DELETE FROM uplinks WHERE "\
      "id = (SELECT MIN(id) FROM uplinkss WHERE "\
      "priority=(SELECT MAX(priority) FROM uplinks)"\
      ") "\
      "RETURNING id, data;"

  #define delete_uplink_id \
      "DELETE FROM uplinks WHERE "\
      "id = ?1;"


  #define delete_all_uplinks \
      "DELETE FROM uplinks;"

	#define delete_config \
		"DELETE FROM configs WHERE id=?1;"

	#define delete_event \
		"DELETE FROM events WHERE id=?1;"



