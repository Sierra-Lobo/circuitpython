import usqlite


con = usqlite.connect('main.db')
con.initializeDatabase()


#test config
con.insertConfig(b'howdy config')
config = con.fetchConfig(1)


#test soh
timestamp = 1234
soh_bytes = b'howdy soh'
con.insertSoh(timestamp, soh_bytes)
soh_bytes = con.fetchSoh(1)




#test commands
cmdID = 5
args = b'arguments'
execution_time = 123
con.insertCommand(cmdID, args, execution_time)

cmdList = con.getNextCommand()
rowid = cmdList[0]
cmdID = cmdList[1]
args = cmdList[2]

con.deleteEntry(rowid, "commands")

#test uplinks
txID = 2
txSize = 100
numPackets = 3
uplink_data = b'howdy world uplinks'
offset = 0
resp = bytearray(len(uplink_data))

con.createUplink(txID, txSize, numPackets)
con.writeDataBlob("uplinks", txID, len(uplink_data), offset, uplink_data)
con.fetchDataBlobInto("uplinks", txID, len(uplink_data), offset, resp)


#test events
severity = 1
msg = b'first event'
uptime = 12
unix_time = 123456
con.logEvent(severity, msg, uptime, unix_time)


#test downlinks
npackets = 1
priority = 1
data = b'first downlink'
data2 = b'second downlink'
l = len(data2)
offset = 0
dbid = con.createDownlink(data, npackets, priority)
dbid2 = con.createDownlinkEmpty(l, npackets, priority)

con.writeDataBlob("downlinks", 2, len(data2), offset, data2)

res = con.getNextDownlink()
dbid = res[0]
data = res[1]

dbid = con.getNextDownlinkID()
dest = bytearray(12)
offset = 0
con.fetchDataBlobInto("downlinks", dbid, len(dest), offset, dest)

#test payload
pos = bytearray(8*3)
dataLen = 12
timestamp = 1234
resp = bytearray(dataLen)
index = con.insertPayloadData(timestamp, pos, dataLen)
con.writeDataBlob("payload", index, dataLen, offset, b"howdy world1")

res = con.fetchDataBlobInto("payload", index, dataLen, offset, resp)



