#this file is purely to document what commands
#have been run, in what way and with what arguments
#probably not exhaustive.


import board
import sdcardio
import busio
import storage
import usqlite

##need to implement: soh insert/fetch

##need to fix: 



def test_initialization():
    new_database_name = "/sd/main.db"
    con = usqlite.connect(new_database_name)
    con.initializeDatabase()

#this function shows the basic functionality 
#of the commands related functions
def test_commands(con):
    cmdID = 5
    args = b'howdy arguments'
    executeTime = 1
    con.insert(cmdID, args, executeTime)

    cmdList = con.getNextCommand()
    rowid = cmdList[0]
    
    con.deleteCommand(rowid)


def test_uplinks(con):
    txID = 2
    txSize = 100
    numPackets = 3
    uplink_data = b'howdy world uplinks'

    con.createUplink(txID, txSize, numPackets);
    
    offset = 0
    con.writeDataBlob("uplinks",txID,len(uplink_data), offset , msg)

    resp = bytearray(len(uplink_data))
    con.fetchDataBlobInto("uplinks", txID, len(uplink_data), offset, resp)
    print(resp)


    missing_ids = con.getMissingPacketIds(txID)
    for packet_id in missing_ids:
        con.setUplinkPacketReceived(txID, packetID)

def test_events(con):

    severity = 1
    msg = b'howdy events'
    uptime = 12
    unix_time = 123456
    con.logEvent(severity, msg, uptime, unix_time)




if __name__ == "__main__":

    cs = board.D5
    clk = board.D18
    miso = board.D19
    mosi = board.D23

    spi = busio.SPI(clk, MOSI = mosi, MISO = miso )

    sd = sdcardio.SDCard(spi, cs)
    vfs = storage.VfsFat(sd)
    storage.mount(vfs, "/sd")

    con = usqlite.connect('/sd/main.db')
