#include "CpuBase.h"
#include "GdbServerNone/GdbServerNone.h"
#include "GdbServerNone/GdbServerTcp.h"

template class CpuBase<GdbServerNone>;
template class CpuBase<GdbServerTcp>;
