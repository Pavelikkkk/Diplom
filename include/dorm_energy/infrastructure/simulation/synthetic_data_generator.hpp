// include/dorm_energy/infrastructure/simulation/synthetic_data_generator.hpp
#pragma once

#include "dorm_energy/core/measurement.hpp"
#include "dorm_energy/domain/simulation/idata_generator.hpp"
#include "dorm_energy/domain/storage/imeasurement_repository.hpp"

#include <chrono>
#include <random>

namespace dorm_energy::simulation
{
    class SyntheticDataGenerator : public IDataGenerator
    {
    public:
        explicit SyntheticDataGenerator(
            unsigned seed = 42, bool inject_anomalies = false, double anomaly_rate = 0.03,
            std::shared_ptr<storage::IMeasurementRepository> repository = nullptr);

        void setSeed(unsigned seed) override;

        core::ReadingsBatch generate() const override;
        core::ReadingsBatch generate_for_days(int days) const override;

    private:
        struct RoomState
        {
            bool motion;
            double power;
            double light;

            std::string deviceId;

            std::chrono::system_clock::time_point timestamp;
        };

        RoomState generate_room_state(const std::string &deviceId,
                                      std::chrono::system_clock::time_point timestamp) const;

        mutable std::mt19937 rng_;
        bool inject_anomalies_;
        double anomaly_rate_;
        std::shared_ptr<storage::IMeasurementRepository> repository_;

        core::SensorReading generate_one_reading(std::chrono::system_clock::time_point base_time,
                                                 int reading_index) const;

        RoomState generateRoomState(const std::string &deviceId,
                                    std::chrono::system_clock::time_point timestamp) const;

        int extract_hour(std::chrono::system_clock::time_point tp) const;
    };

} // namespace dorm_energy::simulation