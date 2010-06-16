#include "GdbServerNone.h"
#include "GdbServerTcp.h"
#include "CpuBase.h"

template class CpuBase<GdbServerNone>;
template class CpuBase<GdbServerTcp>;
