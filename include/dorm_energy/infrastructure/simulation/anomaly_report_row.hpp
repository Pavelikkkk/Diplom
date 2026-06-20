#include "dorm_energy/core/room_state.hpp"
#include "dorm_energy/domain/detection/anomaly_info.hpp"

namespace dorm_energy::simulation
{
    struct AnomalyReportRow
    {
        core::RoomState state;
        detection::AnomalyInfo info;
    };
}// namespace  dorm_energy::simulation