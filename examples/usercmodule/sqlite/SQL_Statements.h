#include "string.h"

/*
   * **********************************
   * DATABASE TABLE CREATION STATEMENTS
   * **********************************
   */

  /**
   * @brief Create the configs table
   *
   * @author Owen DelBene
   * 6/8/2024
   */
/*  static const  char* create_config =
      "CREATE TABLE configs("
      "id INTEGER PRIMARY KEY,"
      "value BLOB NOT NULL);";
*/
  /**
   * @brief Create the commands table
   *
   * @author Owen DelBene
   * 6/8/2024
   */
  static const  char* create_commands =
      "CREATE TABLE commands("
      "dbid INTEGER PRIMARY KEY AUTOINCREMENT,"
      "time INTEGER NOT NULL,"
      "cid INTEGER NOT NULL,"
      "params BLOB);";

  /**
   * @brief Create the SOH table
   *
   * TODO: Possibly add column describing the time of the SOH w.r.t.
   *       system uptime. Add a boolean describing if time has been
   *       set by GPS yet. When GPS time is found, do a bulk update
   *       of all SOH with relative time.
   *
   * @author Owen DelBene
   * 6/8/2024
   */
  static const  char* create_soh =
      "CREATE TABLE soh("
	  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "time INTEGER,"
      "data BLOB NOT NULL);";


  /**
   * @brief Create the downlinks table
   *
   * @author Owen DelBene
   * @date 6/8/2024
   */
  static const  char* create_downlinks =
      "CREATE TABLE downlinks("
      "dbid INTEGER PRIMARY KEY AUTOINCREMENT,"
      "priority INTEGER NOT NULL,"
      "npackets INTEGER NOT NULL,"
      "sent BLOB NOT NULL,"
      "data BLOB NOT NULL);";

  /**
   * @brief Create the downlinks table
   *
   * @author Owen DelBene
   * @date 6/8/2024

   */
  static const  char* create_uplinks =
      "CREATE TABLE uplinks("
      "dbid INTEGER PRIMARY KEY,"
      "numPckts INTEGER NOT NULL,"
      "data BLOB NOT NULL,"
      "missing BLOB NOT NULL);";

  /**
   * @brief Create the events table
   *
   * @author Owen DelBene
   * @date 6/8/2024
   */
  static const  char* create_events =
      "CREATE TABLE events("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "severity INTEGER NOT NULL,"
      "time INTEGER ,"
      "uptime INTEGER NOT NULL,"
      "info TEXT NOT NULL);";
  
  /**
   * @brief Create the commands table
   *
   * @author Owen DelBene
   * 6/8/2024
   */
  static const  char* create_payload =
      "CREATE TABLE payload("
      "dbid INTEGER PRIMARY KEY AUTOINCREMENT,"
      "time INTEGER ,"
      "pos BLOB ,"
      "data BLOB NOT NULL );";


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
  static const  char* query_command =
      "SELECT MIN(dbid), time, cid, params FROM commands WHERE "
      "time=(SELECT MIN(time) FROM commands);";

  /**
   * @brief Query the value of a config parameter
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The id of the configuration variable to fetch
   * @return Return the ID requested and the value of the parameter as a blob
   */
  static const  char* query_config =
      "SELECT id, value FROM configs WHERE id=?1;";

  /**
   * @brief Query the database for all configs and their values
   *
   * @author Owen DelBene
   * @date 6/8/2024
   */
  static const  char* query_all_config =
      "SELECT id, value FROM configs;";

  /**
   * @brief Query all SOH data newer than a given time
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp to fetch
   *
   * @return The time and SOH data for all SOHs occurring at or after the given
   * time
   */
  static const  char* query_soh_time =
      "SELECT time, data FROM soh WHERE time >= ?1 AND time <= ?2;";
  
  /**
   * @brief Query all SOH data newer than a given time
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp to fetch
   *
   * @return The time and SOH data for all SOHs occurring at or after the given
   * time
   */
  static const  char* query_soh_index =
      "SELECT time, data FROM soh WHERE id =?1;";

  /**
   * @brief Query all payload data newer than a given time
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp to fetch
   *
   * @return The time and SOH data for all SOHs occurring at or after the given
   * time
   */
  static const  char* query_payload_time =
      "SELECT time, data, pos FROM soh WHERE time >= ?1 AND time <= ?2;";
  
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
  static const  char* query_payload_index=
      "SELECT time, data, pos FROM payload WHERE dbid =?1;";
  
  /**
   * @brief Query all payload data by index
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The position to search for 
   *
   * @return The time and payload data for given indexn
   *
   */
  static const  char* query_payload_pos=
      "SELECT time, data, pos FROM payload WHERE pos =?1;";
  /**
   * @brief Query last given number of SOH structs
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The number of SOH structs to return
   *
   * @returns A queryset of SOH rows.
   */
  static const  char* query_soh_num =
      "SELECT time, data FROM soh ORDER BY time DESC LIMIT ?1;";

  /**
   * @brief Query the next downlink to send down
   *
   * Gets the next highest priority downlink
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @return The next data to be downlinked
   */
  static const  char* query_downlink =
      "SELECT MIN(dbid), data FROM downlinks "
      "WHERE priority = (SELECT MAX(priority) FROM downlinks);";

  static const  char* query_downlink_id =
      "SELECT MIN(dbid) FROM downlinks "
      "WHERE priority = (SELECT MAX(priority) FROM downlinks) AND "
      "AllBitsSet(npackets, sent) = 0;";

  /**
   * @brief Query the missing packets bitset from an uplink
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The transmission ID of the uplink
   */
  static const  char* query_missing_uplink =
      "SELECT numPckts, missing FROM uplinks WHERE rowid = ?1;";

  static const  char* query_resend_requests =
      "SELECT dbid FROM uplinks WHERE AllBitsUnset(numPckts, missing) = 0;";

  /**
   * @brief Query the data vector from an uplink
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The transmission ID of the uplink
   */
  static const  char* query_uplink_data =
      "SELECT data FROM uplinks WHERE rowid = ?1;";


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
  static const  char* query_table_size =
      " SELECT SUM(pgsize) FROM dbstat WHERE name LIKE ?1;";

  /*
   * **********************************
   * DATABASE TABLE INSERT STATEMENTS
   * **********************************
   */

  /**
   * @brief Insert a new command into the database
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The time the command should occur
   * @param ?2 The ID of the command
   * @param ?3 Any params for the command
   */
  static const  char* insert_payload =
      "INSERT INTO commands(time, cid, params) VALUES(?1, ?2, ?3);";
  
  /**
   * @brief Insert  payload data into the database
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The time the experiment took place
   * @param ?2 The gps position the experiment took place 
   * @param ?3 The raw payload data
   */
  static const  char* insert_command =
      "INSERT INTO commands(time, cid, params) VALUES(?1, ?2, ?3);";

  /**
   * @brief Insert or replace a config variable
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The ID of the config parameter
   * @param ?2 The new value of the parameter
   */
  static const  char* upsert_config =
      "REPLACE INTO configs VALUES(?1, ?2);";

  /**
   * @brief Insert an SOH datum
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp of the datum
   * @param ?2 The SOH Data blob
   */
  static const  char* insert_soh =
      "INSERT INTO soh(time, data) VALUES(?1, ?2);";


  /**
   * @brief Insert a downlink
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The downlink's raw byte vector
   * @param ?2 The downlink's priority
   */
  static const  char* insert_downlink =
      "INSERT INTO downlinks(data, priority, sent, npackets) VALUES(?1, ?2, "
      "zeroblob(?3), ?4) returning dbid;";

  static const  char* get_downlink_sent =
      "SELECT npackets, sent FROM downlinks WHERE dbid = ?1;";

  static const  char* set_downlink_sent =
      "UPDATE downlinks SET sent = ?1 WHERE dbid = ?2;";


  /**
   * @brief Insert an empty uplink into the database with the proper final size
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The desired transmission ID
   * @param ?2 The size of the final uplink in bytes
   * @param ?3 The "missing packets" bitset
   * @param ?4 The number of packets in the uplink
   */
  static const  char* insert_uplink =
      "INSERT INTO uplinks VALUES(?1, ?4, zeroblob(?2), ?3);";

  /**
   * @brief Insert a new event
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The severity of the event
   * @param ?2 The timestamp blob
   * @param ?3 The provided human readable information
   */
  static const  char* insert_event =
      "INSERT INTO events(severity,  time, uptime, info) VALUES(?1, ?2, ?3, ?4);";


  /**
   * @brief Update the missing packets section of an uplink
   *
   * @author Owen DelBene
   * @date 6/8/2024
   *
   * @param ?1 The new missing byte vector
   * @param ?2 The transmission ID of the uplink
   */
  static const  char* update_missing_uplink =
      "UPDATE uplinks SET missing = ?1 WHERE rowid = ?2;";

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
  static const  char* pop_last_command =
      "DELETE FROM commands WHERE "
      "dbid = (SELECT MIN(dbid) FROM commands WHERE "
      "time = (SELECT MIN(time) FROM commands)) "
      "RETURNING dbid, time, cid, params;";
  
  /**
   * @brief Delete command from the database given index 
   *
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The index of the command to delete.
   */
  static const  char* delete_command =
      "DELETE FROM COMMANDS WHERE dbid = ?1;";

  /**
   * @brief Delete payload from the database given index 
   *
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The index of the payload entry to delete.
   */
  static const  char* delete_payload =
      "DELETE FROM PAYLOAD WHERE dbid = ?1;";
  /**
   * @brief Delete all SOH data older than the given timestamp
   *
   * The delete is inclusive -- Any SOH with the given timestamp will be deleted.
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @param ?1 The UNIX timestamp of the newest SOH to delete.
   */
  static const  char* delete_soh =
      "DELETE FROM soh WHERE time <= ?2 AND time >= ?1;";

  /**
   * @brief Delete the oldest downlink (the one least recently downlinked) and
   * return it
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @returns The data for the downlink that was deleted
   */
  static const  char* delete_downlink =
      "DELETE FROM downlinks WHERE "
      "dbid = (SELECT MIN(dbid) FROM downlinks WHERE "
      "priority=(SELECT MAX(priority) FROM downlinks)"
      ") "
      "RETURNING dbid, data;";

  static const  char* delete_downlink_id =
      "DELETE FROM downlinks WHERE "
      "dbid = ?1;";

  /**
   * @brief Delete the oldest uplink (the one least recently uplinked) and
   * return it
   *
   * @author Owen DelBene
   * 6/8/2024
   *
   * @returns The data for the uplink that was deleted
   */
  static const  char* delete_uplink =
      "DELETE FROM uplinks WHERE "
      "dbid = (SELECT MIN(dbid) FROM uplinkss WHERE "
      "priority=(SELECT MAX(priority) FROM uplinks)"
      ") "
      "RETURNING dbid, data;";

  static const  char* delete_uplink_id =
      "DELETE FROM uplinks WHERE "
      "dbid = ?1;";


  static const  char* delete_all_uplinks =
      "DELETE FROM uplinks;";

