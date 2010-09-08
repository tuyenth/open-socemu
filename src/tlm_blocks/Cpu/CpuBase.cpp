#include "GdbServerNone/GdbServerNone.h"
#include "GdbServerNone/GdbServerTcp.h"
#include "CpuBase.h"

template class CpuBase<GdbServerNone>;
template class CpuBase<GdbServerTcp>;
