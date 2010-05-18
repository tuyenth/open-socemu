########################################################################################
# SaxParser class definition
########################################################################################

import xml.parsers.expat
from legalexception import *

class SaxParser:
    "SaxParser class"
    def __init__(self, start_first):
        # create the XML parser entity
        self.xmlparser = xml.parsers.expat.ParserCreate()

        # create empty lists which will be used as FIFO
        self.start_element_fifo = []
        self.end_element_fifo = []
        self.character_data_fifo = []

        # initialize the element handlers
        self.xmlparser.StartElementHandler = start_first
        self.xmlparser.EndElementHandler = self.end_standard
        self.xmlparser.CharacterDataHandler = None
        
    def parse(self, file):
        "parse a file"
        # open the input file
        try:
            file_in = open(file, 'r')
        except:
            raise LegalException("Error: can't open file %s" % file, 1)

        # parse the input file (read the XML input files)
        try:
            self.xmlparser.ParseFile(file_in)
        except LegalException, e:
            print "Exception raised while reading file: " + file + " at line " + repr(self.xmlparser.CurrentLineNumber)
            raise e
        except:
            print "Exception raised while reading file: " + file + " at line " + repr(self.xmlparser.CurrentLineNumber)
            raise
        # close the input file
        file_in.close()

    #   - standard error handler
    def start_error(self, name, att):
        raise LegalException("Error: unexpected tag '%s'" % (name), 1)

    #   - standard end element handler
    def end_standard(self, name):
        # check that we have not reached the last element
        if len(self.start_element_fifo) == 0:
            raise LegalException("Error: more closing tags than opening tags", 1)
        if len(self.end_element_fifo) == 0:
            raise LegalException("Error: more closing tags than opening tags", 1)
        # pop the last handlers
        self.pop_handlers()

    def push_handlers(self, start_handler = None, end_handler = None, data_handler = None):
        "push the start and end handlers"
        #print "Push tag : start(" + str(len(obj.start_element_fifo)) + ") data (" + str(len(obj.character_data_fifo)) + ")"
        self.start_element_fifo.insert(0, self.xmlparser.StartElementHandler)
        self.end_element_fifo.insert(0, self.xmlparser.EndElementHandler)
        self.character_data_fifo.insert(0, self.xmlparser.CharacterDataHandler)
        if start_handler != None:
            self.xmlparser.StartElementHandler = start_handler
        else:
            self.xmlparser.StartElementHandler = self.start_error
        if end_handler != None:
            self.xmlparser.EndElementHandler = end_handler
        else:
            self.xmlparser.EndElementHandler = self.end_standard
        self.xmlparser.CharacterDataHandler = data_handler

    def pop_handlers(self):
        "pop the start and end handlers"
        #print "Pop tag : start(" + str(len(obj.start_element_fifo)) + ") data (" + str(len(obj.character_data_fifo)) + ")"
        self.xmlparser.StartElementHandler = self.start_element_fifo[0]
        self.xmlparser.EndElementHandler = self.end_element_fifo[0]
        self.xmlparser.CharacterDataHandler = self.character_data_fifo[0]
        del self.start_element_fifo[0]
        del self.end_element_fifo[0]
        del self.character_data_fifo[0]

    def wrong_attributes(self, att, mandatory, optional):
        "function used to check that the attributes provided are correct"
        # check that the mandatory attributes are present
        for i in mandatory:
            if i not in att:
                return True
        # check that all the attributes are supported
        for i in att:
            if i not in mandatory and i not in optional:
                return True
        return False


