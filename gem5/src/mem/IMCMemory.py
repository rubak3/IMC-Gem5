from m5.params import *
from m5.objects.MemObject import MemObject

class IMCMemory(MemObject):
    type = 'IMCMemory'
    cxx_header = "mem/IMCMemory.hh"
    cxx_class = "IMCMemory"

    range = Param.AddrRange("Address range for IMC memory")