import { useEffect, useState } from "react";
import { useParams } from "react-router-dom";

import {
  getUserBuilding,
  getUserBuildingRooms,
  getUserBuildingDevices,
  getUserBuildingAnomalies,
} from "../services/api";
import type { Anomaly, Building, Device, Room } from "../services/api";

import StatCard from "../components/StatCard";
import AlertCard from "../components/AlertCard";
import PowerChart from "../components/PowerChart";
import EnergyByRoomChart from "../components/EnergyByRoomChart";
import AnomaliesChart from "../components/AnomaliesChart";
import SeverityDistributionChart from "../components/SeverityDistributionChart";

export default function BuildingAnalytics() {
  const { id } = useParams();

  const [building, setBuilding] = useState<Building | null>(null);

  const [rooms, setRooms] = useState<Room[]>([]);

  const [devices, setDevices] = useState<Device[]>([]);

  const [anomalies, setAnomalies] = useState<Anomaly[]>([]);

  useEffect(() => {
    async function loadData() {
      try {
        const [buildingData, roomsData, devicesData, anomaliesData] =
          await Promise.all([
            getUserBuilding(id!),
            getUserBuildingRooms(id!),
            getUserBuildingDevices(id!),
            getUserBuildingAnomalies(id!),
          ]);

        setBuilding(buildingData);

        setRooms(roomsData);

        setDevices(devicesData);

        setAnomalies(anomaliesData);
      } catch (error) {
        console.error(error);
      }
    }

    loadData();
  }, [id]);

  if (!building) {
    return <div className="text-slate-300">Loading...</div>;
  }

  const onlineDevices = devices.filter((device) => device.isOnline).length;

  const criticalAnomalies = anomalies.filter(
    (anomaly) => anomaly.severity === "CRITICAL",
  ).length;

  const averageScore =
    anomalies.length > 0
      ? (
          anomalies.reduce(
            (sum, anomaly) => sum + Number(anomaly.score ?? 0),
            0,
          ) / anomalies.length
        ).toFixed(2)
      : "0.00";

  const devicesByRoom = rooms.map((room) => ({
    roomName: room.roomName,
    count: devices.filter((device) => device.roomId === room.roomId).length,
  }));

  const anomaliesByType = anomalies.reduce<Record<string, number>>(
    (acc, anomaly) => {
      const type = anomaly.type ?? "Unknown";

      acc[type] = (acc[type] ?? 0) + 1;

      return acc;
    },
    {},
  );

  return (
    <div className="space-y-8">
      {/* HEADER */}

      <div>
        <h1 className="text-5xl
              font-bold
              mb-3">{building.name}</h1>

        <p className="text-slate-300">
          Building energy analytics and anomaly monitoring dashboard.
        </p>
      </div>

      {/* BUILDING INFO */}

      <div
        className="bg-[#111827]
              border
              border-cyan-700/40
              rounded-2xl
              p-5"
      >
        <h2 className="text-3xl
              font-bold">{building.name}</h2>

        <p className="text-slate-300
              mt-2">{building.address}</p>

        <p className="mt-4">{building.description}</p>
      </div>

      {/* STATS */}

      <div
        className="grid
              grid-cols-1
              md:grid-cols-2
              xl:grid-cols-4
              gap-4"
      >
        <StatCard title="Rooms" value={rooms.length} />

        <StatCard title="Devices" value={devices.length} />

        <StatCard title="Anomalies" value={anomalies.length} />

        <StatCard title="Online" value={onlineDevices} />
      </div>

      <div>
        <h2 className="mb-4 text-3xl font-bold">Building Power Consumption</h2>

        <div className="rounded-2xl border border-cyan-700/40 bg-[#111827] p-5">
          <PowerChart buildingId={id} />
        </div>
      </div>

      <div>
        <h2 className="mb-4 text-3xl font-bold">Energy by Room</h2>

        <EnergyByRoomChart buildingId={id} />
      </div>

      <div className="grid grid-cols-1 gap-5 xl:grid-cols-2">
        <AnomaliesChart buildingId={id} />

        <SeverityDistributionChart buildingId={id} />
      </div>

      <div
        className="grid
              grid-cols-1
              xl:grid-cols-2
              gap-5"
      >
        <div
          className="bg-[#111827]
                border
                border-cyan-700/40
                rounded-2xl
                p-5"
        >
          <h2 className="text-2xl
                font-bold
                mb-6">Devices by Room</h2>

          <div className="space-y-4">
            {devicesByRoom.map((item) => (
              <div key={item.roomName}>
                <div className="flex
                      justify-between
                      mb-2">
                  <span>{item.roomName}</span>
                  <span className="text-cyan-400">{item.count}</span>
                </div>

                <div className="h-3
                      rounded-full
                      bg-[#020617]
                      overflow-hidden">
                  <div
                    className="h-full
                          bg-cyan-500"
                    style={{
                      width: `${
                        devices.length > 0
                          ? (item.count / devices.length) * 100
                          : 0
                      }%`,
                    }}
                  />
                </div>
              </div>
            ))}
          </div>
        </div>

        <div
          className="bg-[#111827]
                border
                border-cyan-700/40
                rounded-2xl
                p-5"
        >
          <h2 className="text-2xl
                font-bold
                mb-6">Building Anomalies</h2>

          <div className="grid
                grid-cols-2
                gap-4
                mb-6">
            <div className="bg-[#020617]
                  rounded-xl
                  p-4">
              <div className="text-slate-300
                    text-sm">Critical</div>
              <div className="text-3xl
                    font-bold
                    text-red-400">
                {criticalAnomalies}
              </div>
            </div>

            <div className="bg-[#020617]
                  rounded-xl
                  p-4">
              <div className="text-slate-300
                    text-sm">Avg Score</div>
              <div className="text-3xl
                    font-bold
                    text-cyan-400">
                {averageScore}
              </div>
            </div>
          </div>

          <div className="space-y-3">
            {Object.entries(anomaliesByType).map(([type, count]) => (
              <div
                key={type}
                className="flex
                      justify-between
                      border-b
                      border-slate-800
                      pb-2"
              >
                <span>{type}</span>
                <span className="text-cyan-400">{count}</span>
              </div>
            ))}

            {anomalies.length === 0 && (
              <div className="text-slate-300">
                No anomalies for this building
              </div>
            )}
          </div>
        </div>
      </div>

      {/* RECENT ANOMALIES */}

      <div>
        <h2 className="text-3xl
              font-bold
              mb-4">Recent Anomalies</h2>

        <div className="grid
              gap-4">
          {anomalies.length === 0 && (
            <div
              className="bg-[#111827]
                    rounded-2xl
                    p-5
                    text-slate-300"
            >
              No anomalies detected
            </div>
          )}

          {anomalies.slice(0, 5).map((anomaly, index) => (
            <AlertCard
              key={`${anomaly.room}-${anomaly.type}-${index}`}
              room={anomaly.room}
              type={anomaly.type}
              severity={anomaly.severity}
              score={anomaly.score}
              status={anomaly.status ?? "ACTIVE"}
            />
          ))}
        </div>
      </div>
    </div>
  );
}
