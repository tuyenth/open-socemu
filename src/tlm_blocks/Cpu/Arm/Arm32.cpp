#include "Cpu/Arm/Arm32.h"
#include "GdbServerNone/GdbServerNone.h"
#include "GdbServerNone/GdbServerTcp.h"

template class Arm32<GdbServerNone>;
template class Arm32<GdbServerTcp>;
