########################################################################################
# Agent class
########################################################################################

import struct
import socket
import ethernet
import wifi_tx
import wifi_rx
import common.bytes

class Agent:
    """Agent class"""
    def __init__(self, verbosity):
        """Create the main application"""
        
        # create the socket instance
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # connect to the localhost
        self.sock.connect(('127.0.0.1', 12346))
        
        self.verbosity = verbosity
        
    def send_msg(self, action, message, title):
        # debug
        if self.verbosity > 0:
            print("- SEND MESSAGE: " + title)
        if self.verbosity > 1:
            print("     > MSG: length = %d\n%s"%
                  (len(message),common.bytes.dump(message[0:60])))

        # build the packet
        packet = struct.pack("<LLL", 0xdeadc0de, action, len(message)) + message

        # send the packet
        self.sock.send(packet)

    def wait(self, delay_in_ns):
        """
        Send a command to the reference platform to wait for some time.
        
        Mandatory arguments:
        delay_in_ns -- time to wait before resuming command reception
        """
        # pack a message : delay 4 bytes
        message = struct.pack("L", delay_in_ns)
        self.send_msg(0, message, "wait(%d ns)"%(delay_in_ns,))
        
    def nxapi_reset_req(self, set_defmib):
        """
        Send a command to the MACSW on reference platform to reset.
        
        Mandatory arguments:
        set_defmib -- 0 to reset without resetting the MIBs, other value to reset'em
        """
        message = struct.pack("B", set_defmib)
        self.tc_hsend("KERNEL(RESET_REQ)")
        self.send_msg(0x10, message, "nxapi_reset_req")
        if self.verbosity > 1:
            print("- WAIT KERNEL MESSAGE")
        packet = self.sock.recv(4096)
        self.tc_hrecv("KERNEL(RESET_CFM)")
        assert(packet == "OK")
        
    def nxapi_dbg_macaddr_set_req(self, macaddr):
        message = struct.pack("6s", macaddr)
        self.tc_hsend("KERNEL(DBG_MACADDR_SET_REQ)")
        self.send_msg(0x11, message, "nxapi_dbg_macaddr_set_req")
        packet = self.sock.recv(4096)
        self.tc_hrecv("KERNEL(DBG_MACADDR_SET_CFM)")
        assert(packet == "OK")
        
    def nxapi_dbg_staadd_req(self, macaddr = "\0\0\0\0\0\0", mode = 1):
        """
        Send a debug command to the reference platform to open the data path for a 
        specific station address.
        
        Keyword arguments:
        macaddr -- address of the station to add (default = only 0's)
        mode -- working mode of the reference platform code (0=STA in IBSS,
                1=STA in ESS(default), 2=AP)
        """
        message = struct.pack("6sB", macaddr, mode)
        self.tc_hsend("KERNEL(DBG_STAADD_REQ)")
        self.send_msg(0x12, message, "nxapi_dbg_staadd_req")
        
    def nxapi_mibget_req(self, mibname):
        message = struct.pack("B", len(mibname)) + mibname
        self.tc_hsend("KERNEL(MIBGET_REQ)")
        self.send_msg(0x18, message, "nxapi_mibget_req")
        if self.verbosity > 1:
            print("- WAIT KERNEL MESSAGE")
        packet = self.sock.recv(4096)
        self.tc_hrecv("KERNEL(MIBGET_CFM)")
        if self.verbosity > 1:
            print("  -> received msg(%s)"%(common.bytes.convert_hex(packet)))
        return packet
        
    def nxapi_mibset_req(self, mibname, mibvalue):
        message = struct.pack("B", len(mibname)) + mibname + \
                  struct.pack("B", len(mibvalue)) + mibvalue
        self.tc_hsend("KERNEL(MIBSET_REQ)")
        self.send_msg(0x19, message, "nxapi_mibset_req")
        if self.verbosity > 1:
            print("- WAIT KERNEL MESSAGE")
        packet = self.sock.recv(4096)
        self.tc_hrecv("KERNEL(MIBSET_CFM)")
        assert(packet == "OK")
        
    def nxapi_scan_req(self, type, mintime, maxtime):
        message = struct.pack("HHH", type, mintime, maxtime)
        self.tc_hsend("KERNEL(SCAN_REQ)")
        self.send_msg(0x20, message, "nxapi_scan_req")

    def nxapi_scan_cfm(self):
        message = ""
        self.send_msg(0x21, message, "nxapi_scan_cfm")
        packet = self.sock.recv(4096)
        self.tc_hrecv("KERNEL(SCAN_CFM)")
        assert(packet == "OK")
        
    def nxapi_scangetresult_req(self):
        """
        Return a tuple containing:
        (status, number of results, result array)
        """
        message = ""
        self.tc_hsend("KERNEL(SCANGETRESULT_REQ)")
        self.send_msg(0x22, message, "nxapi_scangetresult_req")
        packet = self.sock.recv(4096)
        self.tc_hrecv("KERNEL(SCANGETRESULT_CFM)")
        
        if self.verbosity > 1:
            print("     > length = %d\n%s"% (len(packet),common.bytes.dump(packet)))
        # extract the number of results
        (status, count) = struct.unpack("HH", packet[0:4])

        return (status, count, packet[4:])
        
    def nxapi_sta_connect_req(self, bssid):
        message = struct.pack("6s", bssid)
        self.tc_hsend("KERNEL(STA_CONNECT_REQ)")
        self.send_msg(0x30, message, "nxapi_sta_connect_req")

    def nxapi_sta_connect_cfm(self):
        message = ""
        self.send_msg(0x31, message, "nxapi_sta_connect_cfm")
        packet = self.sock.recv(4096)
        self.tc_hrecv("KERNEL(STA_CONNECT_CFM)")
        return (packet == "OK")

    def nxapi_disassociate_req(self, addr):
        message = struct.pack("6s", addr)
        self.tc_hsend("KERNEL(DISASSOCIATE_REQ)")
        self.send_msg(0x32, message, "nxapi_sta_disassociate_req")

    def nxapi_disassociate_cfm(self):
        message = ""
        self.send_msg(0x33, message, "nxapi_disassociate_cfm")
        packet = self.sock.recv(4096)
        self.tc_hrecv("KERNEL(DISASSOCIATE_CFM)")
        return (packet == "OK")

    def nxapi_disassociate_ind(self):
        message = ""
        self.send_msg(0x34, message, "nxapi_disassociate_ind")
        packet = self.sock.recv(4096)
        self.tc_hrecv("KERNEL(DISASSOCIATE_IND)")
        return (packet == "OK")

    def air_receive_data(self, mpdu):
        message = struct.pack("L"+str(len(mpdu))+"s", len(mpdu), str(mpdu))
        self.send_msg(0x100, message, "air_receive_data")
        # print a message
        self.tc_arecv(mpdu)

    def host_receive_data(self):
        message = ""
        self.send_msg(0x101, message, "host_receive_data")
        if self.verbosity > 0:
            print("- WAIT HOST DATA")
        packet = self.sock.recv(4096)
        # print a message
        self.tc_hrecv("MSDU")
        
        # build an RX MSDU object instance to analyze the received frame
        msdu = ethernet.rx_msdu(packet)

        return msdu

    def host_send_data(self, msdu, service_class = 0):
        """
        Send a command to the reference platform to push an MSDU from the host.
        
        Mandatory arguments:
        msdu -- tx_msdu object to send
        
        Keyword arguments:
        service_class -- indicate if policy kind (ack'd or not, default = ack'd)
        """
        # sanity check
        assert(isinstance(msdu, ethernet.tx_msdu))
        
        # pack a message with information to build a senddatareq API message
        message = struct.pack("HBBB"+str(msdu.length)+"s", msdu.length,
                              msdu.prio, service_class, msdu.vlanid, str(msdu))
        self.send_msg(0x200, message, "host_send_data")
        # print a message
        self.tc_hsend("MSDU")

    def host_send_data_cfm(self, msdu):
        """
        Wait for the send confirmation of an MSDU.
        
        Mandatory arguments:
        msdu -- tx_msdu object to wait the confirmation from
        """
        # sanity check
        assert(isinstance(msdu, ethernet.tx_msdu))

        message = struct.pack("B", msdu.prio)
        self.send_msg(0x201, message, "host_send_data_cfm")
        if self.verbosity > 0:
            print("- WAIT HOST TX CONFIRM")
        packet = self.sock.recv(msdu.length)
        if self.verbosity > 1:
            print("    < received TX confirmation")
        if packet != str(msdu):
            print("rx=\n%sexpected=\n%s"%
                  (common.bytes.dump(packet), common.bytes.dump(str(msdu))))
            assert(packet == str(msdu))

    def air_send_data(self, queue):
        message = struct.pack("B", queue)
        self.send_msg(0x202, message, "air_send_data")
        if self.verbosity > 0:
            print("- WAIT AIR MPDU")
        packet = self.sock.recv(4096)
        
        # build a TX MPDU object instance to analyze the sent frame
        mpdu = wifi_tx.tx_mpdu(queue, packet)
        
        if self.verbosity > 1:
            print("TX MAC HW format =\n"+common.bytes.dump(mpdu.machwformat))

        # print a message
        self.tc_asend(mpdu)
        return mpdu
    
    def ap_start(self, bssid, channel, period, mpdu):
        message = struct.pack("6sBHL"+str(len(mpdu))+"s", bssid, channel, period, 
                              len(mpdu), str(mpdu))
        self.send_msg(0x1000, message, "ap_start(channel %d, period %d)"%(channel, period))
    
    def ap_stop(self, bssid):
        message = struct.pack("6s", bssid)
        self.send_msg(0x1001, message, "ap_stop")
    
    def end(self):
        message = ""
        self.send_msg(0xFFFFFFFF, message, "end DUT")
    
    def close(self):
        # close the socket
        self.sock.close()

    def error(self, message):
        print("ERROR: " + message)
        exit(-1)

    def tc_h1(self, name):
        """
        Header 1 testcase outline. Big square box.
        """
        print("*"*79)
        print("*" + " "*((79-2-len(name))/2) + name + " "*((80-2-len(name))/2) +"*")
        print("*"*79)

    def tc_h2(self, name):
        """
        Header 2 testcase outline.  Underlined box.
        """
        print("* " + name)
        print("*"*(len(name) + 2))

    def tc_h3(self, name):
        """
        Header 3 testcase outline.  Star inited line.
        """
        print("* " + name)

    def tc_h4(self, name):
        """
        Header 4 testcase outline.  2 Star inited line.
        """
        print("** " + name)

    def tc_hsend(self, message):
        print("|-(" + message + ")->" + " "*(79-7-len(message)) + "|")
    
    def tc_hrecv(self, message):
        print("|<-(" + message + ")-" + " "*(79-7-len(message)) + "|")

    def tc_asend(self, mpdu):
        message = "%s(%s):%s"%(mpdu.machwqueuename, mpdu.airqueuename, mpdu.info)
        print("|" + " "*(79-7-len(message)) + "-(" + message + ")->|")
    
    def tc_arecv(self, mpdu):
        message = mpdu.info
        print("|" + " "*(79-7-len(message)) + "<-(" + message + ")-|")
        