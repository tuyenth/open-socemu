########################################################################################
# MAC HW TX path tool box
########################################################################################

import struct
import wifi

def tx_machwqueue(mpdu):
    """
    This function returns the MAC HW queue on which the frame was transmitted.
    """
    return ord(mpdu[0])

def tx_channel(mpdu):
    """
    This function returns the channel value configured at the time of frame transmission.
    """
    return ord(mpdu[1])

def tx_framelen(mpdu):
    """
    This function returns the framelen value provided by the software to the MAC HW.
    """
    return struct.unpack("<H", mpdu[2:4])[0]

def tx_machwformat(packet):
    """
    Returns the MAC HW format of the received packet
    """
    return packet[32:]

def tx_policytable(mpdu):
    return mpdu[4:32]

def uPatternPT(pt):
    return struct.unpack("<LLLLLLL", pt)[0]

def nTxProtPT(pt):
    return (struct.unpack("<LLLLLLL", pt)[1] & 0x30000) >> 16

def nTxPT(pt):
    return (struct.unpack("<LLLLLLL", pt)[1] & 0xC000) >> 14

def shortGIPT(pt):
    return (struct.unpack("<LLLLLLL", pt)[1] & 0x2000) != 0

def txPwrLevelPT(pt):
    return (struct.unpack("<LLLLLLL", pt)[1] & 0xE00) >> 9

def stbcPT(pt):
    return (struct.unpack("<LLLLLLL", pt)[1] & 0x180) >> 7

def ldpcPT(pt):
    return (struct.unpack("<LLLLLLL", pt)[1] & 0x40) != 0

def numExtnSSPT(pt):
    return (struct.unpack("<LLLLLLL", pt)[1] & 0x30) >> 4

def BfFrmExPT(pt):
    return (struct.unpack("<LLLLLLL", pt)[1] & 0x8) != 0

def csiPrecisionPT(pt):
    return (struct.unpack("<LLLLLLL", pt)[1] & 0x4) != 0

def cfbTypePT(pt):
    return (struct.unpack("<LLLLLLL", pt)[1] & 0x3) >> 0

def cfbSizePT(pt):
    return (struct.unpack("<LLLLLLL", pt)[2] & 0xFF0000) >> 16

def smmIndexPT(pt):
    return (struct.unpack("<LLLLLLL", pt)[2] & 0xFF00) >> 8

def antennaSetPT(pt):
    return (struct.unpack("<LLLLLLL", msdu[0:28])[2] & 0xFF) >> 0

def keySRamIndexRA(pt):
    return (struct.unpack("<LLLLLLL", pt)[3] & 0x3FF000) >> 12

def keySRamIndex(pt):
    return (struct.unpack("<LLLLLLL", pt)[3] & 0xFFF) >> 0

def rtsThreshold(pt):
    return (struct.unpack("<LLLLLLL", pt)[4] & 0xFFF0000) >> 16

def shortRetryLimit(pt):
    return (struct.unpack("<LLLLLLL", pt)[4] & 0xFF00) >> 8

def longRetryLimit(pt):
    return (struct.unpack("<LLLLLLL", pt)[4] & 0xFF) >> 0

def mcsIndex1(pt):
    return (struct.unpack("<LLLLLLL", pt)[5] & 0xFF0000) >> 16

def mcsIndex2(pt):
    return (struct.unpack("<LLLLLLL", pt)[5] & 0xFF00) >> 8

def mcsIndex3(pt):
    return (struct.unpack("<LLLLLLL", pt)[5] & 0xFF) >> 0

def tx_machdr(mpdu):
    """
    This function returns the MAC header string from the packet transmitted by the MAC HW.
    """
    return mpdu[32:32+48]

def framectl(mh):
    return (struct.unpack("<HH", mh[0:4]))[1]

def initializationvector(mh):
    return (struct.unpack("<LLLLLLLLLLLL", mh))[10]

def initializationvector_unpack(iv):
    """
    This function unpacks all the subfields from the initialization vector field, they are 
    returned as the following tuple:
      (extended IV : bool, )
      
    """
    
    return ((iv & 0x20) != 0, )

def addr1(mh):
    return mh[6:12]

def addr2(mh):
    return mh[12:18]

def addr3(mh):
    return mh[18:24]

def tx_body(mpdu):
    """
    This function returns the frame body from the MPDU
    """
    return mpdu[32+48:]



class tx_mpdu:
    """Class of an MPDU sent over the air in the MAC HW format"""
    def __init__(self, airqueue, packet):
        """constructor"""
        # store the air queue on which it was sent
        self.airqueue = airqueue
        # store the queue name
        self.airqueuename = wifi.AIR_NAME[self.airqueue]
        # store the MAC HW queue on which it was sent
        self.machwqueue = tx_machwqueue(packet)
        # store the MAC HW queue name
        self.machwqueuename = wifi.Q_NAME[self.machwqueue]
        # extract the channel
        self.channel = tx_channel(packet)
        # extract the MAC HW format of the TX frame
        self.machwformat = tx_machwformat(packet)
        # extract the frame length
        self.framelen = tx_framelen(packet)
        # extract the policy table
        self.pt = tx_policytable(packet)
        # extract the MAC header
        self.mh = tx_machdr(packet)
        # extract the frame body
        self.bd = tx_body(packet)
        
        # extract information from the policy table
        self.uPatternPT = uPatternPT(self.pt)

        # extract the frame control field
        self.framectl = framectl(self.mh)
        # unpack the frame control field
        (self.type, self.subtype, self.typesubtype, self.tods, self.fromds, self.morefrag,
         self.retry, self.ps, self.moredata, self.protected, self.order) \
         = wifi.fctl_unpack(self.framectl)
         
        # extract the IV field
        self.iv = initializationvector(self.mh)

        # extract the subfield from the IV field
        (self.extendediv, ) = initializationvector_unpack(self.iv)
        
        # extract the addresses 
        self.addr1 = addr1(self.mh)
        self.addr2 = addr2(self.mh)
        self.addr3 = addr3(self.mh)

        # compute the theoretical length of the frame
        self.compute_theorylen()
        
        # build the info string
        self.build_info()
    
    def build_info(self):
        # print the type of frame
        self.info = wifi.framename[self.typesubtype] + "("
        self.info += "ch=%d"%self.channel
        if self.tods:
            self.info += ",TDS"
            
        if self.fromds:
            self.info += ",FDS"
    
        if self.morefrag:
            self.info += ",MF"
    
        if self.retry:
            self.info += ",R"
     
        if self.ps:
            self.info += ",PM"
        
        if self.moredata:
            self.info += ",MD"
    
        if self.protected:
            self.info += ",PF"
        
        if self.order:
            self.info += ",O"
    
        self.info += ")"

    def compute_theorylen(self):
        """
        This function returns the actual length of the field that the MAC HW is adding to the
        payload taking into account the various bits set in the mac header.
        """

        # initialize the length (frame control + duration/id + addr1 always included)
        length = 10
        
        # addr2 included for control and mgt frames and for RTS, PSPOLL, CFEND, BA and BAR
        if (self.type in (wifi.fctl_type_data, wifi.fctl_type_mgt) or 
            self.typesubtype in (wifi.fctl_rts, wifi.fctl_pspoll, wifi.fctl_cfend, 
                                 wifi.fctl_ba, wifi.fctl_bar)):
            length += 6
        
        # addr3 included for control and mgt frames
        if self.type in (wifi.fctl_type_data, wifi.fctl_type_mgt):
            length += 6
        
        # sequence control only added in data frames
        if self.type in (wifi.fctl_type_data, ):
            length += 2
            
        # address 4 only included in very tods and fromds set
        if self.tods and self.fromds:
            length += 6
        
        # qos control only included for qos data frames
        if self.typesubtype in (wifi.fctl_qosdata, wifi.fctl_qosdatacfack, 
                                wifi.fctl_qosdatacfpoll, wifi.fctl_qosdatacfackcfpoll):
            length += 2
            
        # cfc only included for control wrapper frames
        if self.typesubtype in (wifi.fctl_controlwrapper, ):
            length += 2
            
        # HTC only included for control wrapper frames and ordered frames
        if self.typesubtype in (wifi.fctl_controlwrapper,) or self.order:
            length += 4
    
        # IV only included for protected frames
        if self.protected:
            length += 4
            
        # extended IV only included in protected frames with extended IV
        if self.protected and self.extendediv:
            length += 4
    
        # the remaining elements (MIC, FCS) are included in the body
        length += len(self.bd)
        
        # record the computed theoretical length
        self.theorylen = length
