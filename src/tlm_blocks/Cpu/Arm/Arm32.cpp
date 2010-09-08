#include "GdbServerNone/GdbServerNone.h"
#include "GdbServerNone/GdbServerTcp.h"
#include "Cpu/Arm/Arm32.h"

template class Arm32<GdbServerNone>;
template class Arm32<GdbServerTcp>;
