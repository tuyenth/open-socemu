########################################################################################
# MAC HW RX path tool box
########################################################################################

import struct
import ethernet
import wifi

class rx_mpdu:
    """Class of an MPDU sent received from the air in the MAC HW format"""
    def __init__(self, body, **kwargs):
        """
        This function encapsulates a payload in a wifi MPDU for reception from air in the MAC 
        HW and to be passed to the software.  It takes into account the specificities of the 
        HW to ease the  implementation of the model.
        
        Mandatory arguments:
        body -- frame body
        
        Keyword arguments:
        da -- MAC address of the destination
        sa -- MAC address of the source
        bssid -- MAC address of the BSSID
        fctl -- frame type + subtype (default data)
        tods -- to DS boolean (default False)
        fromds -- from DS boolean (default True)
        morefrag -- more fragment boolean (default False)
        retry -- retry boolean (default False)
        ps -- power save boolean (default False)
        moredata -- more data boolean (default False)
        protected -- protected boolean (default False)
        order -- order boolean (default False)
        sctl -- sequence control field of the frame (see the protocol standard).  If not
            specified, using default value.

        Note that all the elements are stored in little endian
        """
        # save the body
        self.bd = body
        
        # frame type
        if "fctl" in kwargs:
            self.typesubtype = kwargs["fctl"]
            # sanity check
            assert(isinstance(self.typesubtype, int))
            assert((self.typesubtype | (wifi.fctl_type | wifi.fctl_subtype)) == 
                   (wifi.fctl_type | wifi.fctl_subtype))
        else:
            # default to data frame type
            self.typesubtype = wifi.fctl_data
        
        # extract the type and subtypes
        self.type = self.typesubtype & wifi.fctl_type
        self.subtype = self.typesubtype & wifi.fctl_subtype
        
        # to DS
        if "tods" in kwargs:
            self.tods = kwargs["tods"]
            # sanity check
            assert(isinstance(self.tods, bool))
        else:
            self.tods = False

        # from DS
        if "fromds" in kwargs:
            self.fromds = kwargs["fromds"]
            # sanity check
            assert(isinstance(self.fromds, bool))
        else:
            self.fromds = True

        # morefrag
        if "morefrag" in kwargs:
            self.morefrag = kwargs["morefrag"]
            # sanity check
            assert(isinstance(self.morefrag, bool))
        else:
            self.morefrag = False

        # retry
        if "retry" in kwargs:
            self.retry = kwargs["retry"]
            # sanity check
            assert(isinstance(self.retry, bool))
        else:
            self.retry = False

        # ps
        if "ps" in kwargs:
            self.ps = kwargs["ps"]
            # sanity check
            assert(isinstance(self.ps, bool))
        else:
            self.ps = False

        # moredata
        if "moredata" in kwargs:
            self.moredata = kwargs["moredata"]
            # sanity check
            assert(isinstance(self.moredata, bool))
        else:
            self.moredata = False

        # protected
        if "protected" in kwargs:
            self.protected = kwargs["protected"]
            # sanity check
            assert(isinstance(self.protected, bool))
        else:
            self.protected = False

        # moredata
        if "order" in kwargs:
            self.order = kwargs["order"]
            # sanity check
            assert(isinstance(self.order, bool))
        else:
            self.order = False

        if self.type == wifi.fctl_type_data:
            if self.tods == False and self.fromds == False:
                self.addr1 = kwargs["da"]
                self.addr2 = kwargs["sa"]
                self.addr3 = kwargs["bssid"]
                self.addr4 = ethernet.addr(0, 0, 0, 0, 0, 0)
            elif self.tods == False and self.fromds == True:
                self.addr1 = kwargs["da"]
                self.addr2 = kwargs["bssid"]
                self.addr3 = kwargs["sa"]
                self.addr4 = ethernet.addr(0, 0, 0, 0, 0, 0)
            elif self.tods == True and self.fromds == False:
                self.addr1 = kwargs["bssid"]
                self.addr2 = kwargs["sa"]
                self.addr3 = kwargs["da"]
                self.addr4 = ethernet.addr(0, 0, 0, 0, 0, 0)
            else:
                self.addr1 = kwargs["ra"]
                self.addr2 = kwargs["ta"]
                self.addr3 = kwargs["da"]
                self.addr4 = kwargs["sa"]
        elif self.type == wifi.fctl_type_mgt:
                self.addr1 = kwargs["da"]
                self.addr2 = kwargs["sa"]
                self.addr3 = kwargs["bssid"]
                self.addr4 = ethernet.addr(0, 0, 0, 0, 0, 0)
        else:
            # for the time being not implemented
            assert(False)
            
        # sequence control field
        if "sctl" in kwargs:
            self.sctl = kwargs["sctl"]
        else:
            self.sctl = 0x500
        
        # build the frame
        self.__build()

    def __len__(self):
        return self.length
    
    def __str__(self):
        return self.machwformat
    
    def add_retry(self):
        """
        Add the retry bit in the frame.
        """
        self.retry = True
        self.__build()

    def __build(self):
        """
        Build the MAC HW frame format frame based on the member variables of the object.
        """
        # pack the frame control
        framectl = wifi.fctl_pack(self.typesubtype, self.tods, self.fromds, 
                                  self.morefrag, self.retry, self.ps, self.moredata,
                                  self.protected, self.order)

        # initialize the first reserved
        f = struct.pack("<H", 0xDEAD)
        
        f += struct.pack("<H", framectl)
        
        # duration ID field (use a dummy value for the time being)
        f += struct.pack("<H", 0x500)
        
        # addr1 field
        f += self.addr1
        
        # addr2 field
        f += self.addr2
        
        # addr3 field
        f += self.addr3
        
        # sequence control field
        f += struct.pack("<H", self.sctl)
        
        # save the last sequence control field used
        last_seq_control = self.sctl
        
        # addr4 field
        f += self.addr4
        
        # QoS control field
        f += struct.pack("<H", 0x500)
        
        # carried FC field
        f += struct.pack("<H", 0x500)
    
        # HTC field
        f += struct.pack("<L", 0x500)
        
        # IV
        f += struct.pack("<L", 0x500)
        
        # Extended IV
        f += struct.pack("<L", 0x500)
        
        # retrieve the payload
        length = len(self.bd)
        f += self.bd
    
        # add padding payload bytes to align the end of the buffer on a word boundary
        f += '\x00'*((4-(length%4))%4)
        
        # ICV
        f += struct.pack("<L", 0x500)
    
        # MIC 1
        f += struct.pack("<L", 0x500)
        
        # MIC 2
        f += struct.pack("<L", 0x500)
    
        # FCS
        f += struct.pack("<L", 0x500)
        
        # frame body length
        f += struct.pack("<H", length)
        
        # reserved
        f += struct.pack("<H", 0xDEAD)
        
        # TSFLo
        f += struct.pack("<L", 0x500)
        
        # TSFHi
        f += struct.pack("<L", 0x500)
        
        # Receive vector 1a
        f += struct.pack("<L", 0x500)
        
        # Receive vector 1b
        f += struct.pack("<L", 0x500)
        
        # Receive vector 1c
        f += struct.pack("<L", 0x500)
        
        # Receive vector 2a
        f += struct.pack("<L", 0x500)
        
        # Receive vector 2b
        f += struct.pack("<L", 0x500)
        
        # status (successful frame reception bit set)
        f += struct.pack("<L", 0x2000)
        
        # save the MPDU in the MAC HW format
        self.machwformat = f
        self.length = len(f)
        
        #  build the information string
        self.info = wifi.framename[self.typesubtype] + "("
        self.info += "sc=0x%X"%self.sctl
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
