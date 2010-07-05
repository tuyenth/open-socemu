#include "GdbServerNone.h"
#include "GdbServerTcp.h"
#include "Arm32.h"

template class Arm32<GdbServerNone>;
template class Arm32<GdbServerTcp>;
