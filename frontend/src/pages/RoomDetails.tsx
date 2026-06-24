import { useEffect, useState } from "react";
import { useParams } from "react-router-dom";

import {
  getUserRoom,
  getUserRoomDevices,
  getUserRoomAnomalies,
} from "../services/api";
import type { Anomaly, Device, Room } from "../services/api";

import AlertCard from "../components/AlertCard";

export default function RoomDetails() {
  const { id } = useParams();

  const [room, setRoom] = useState<Room | null>(null);

  const [devices, setDevices] = useState<Device[]>([]);

  const [anomalies, setAnomalies] = useState<Anomaly[]>([]);

  const [loading, setLoading] = useState(true);

  useEffect(() => {
    async function loadRoom() {
      try {
        const [roomData, devicesData, anomaliesData] = await Promise.all([
          getUserRoom(id!),
          getUserRoomDevices(id!),
          getUserRoomAnomalies(id!),
        ]);

        setRoom(roomData);

        setDevices(devicesData);

        setAnomalies(anomaliesData);
      } catch (error) {
        console.error(error);
      } finally {
        setLoading(false);
      }
    }

    if (id) {
      loadRoom();
    }
  }, [id]);

  if (loading) {
    return <div className="text-slate-300">Loading...</div>;
  }

  if (!room) {
    return (
      <div>
        <h1 className="text-4xl
              font-bold">Room Not Found</h1>
      </div>
    );
  }

  return (
    <div className="space-y-8">
      {/* HEADER */}

      <div>
        <h1 className="text-5xl
              font-bold
              mb-3">{room.name}</h1>

        <p className="text-slate-300">{room.buildingName}</p>
      </div>

      {/* LIVE STATUS */}

      <div
        className="grid
              grid-cols-1
              md:grid-cols-3
              gap-4"
      >
        <div
          className="bg-[#111827]
                rounded-2xl
                p-5"
        >
          <p className="text-slate-300">Motion</p>

          <h2
            className={
              room.motion
                ? "text-3xl font-bold text-emerald-400"
                : "text-3xl font-bold text-slate-300"
            }
          >
            {room.motion ? "Detected" : "No Motion"}
          </h2>
        </div>

        <div
          className="bg-[#111827]
                rounded-2xl
                p-5"
        >
          <p className="text-slate-300">Power</p>

          <h2 className="text-3xl
                font-bold
                text-cyan-400">{room.power} W</h2>
        </div>

        <div
          className="bg-[#111827]
                rounded-2xl
                p-5"
        >
          <p className="text-slate-300">Light</p>

          <h2 className="text-3xl
                font-bold
                text-yellow-400">
            {room.light} lux
          </h2>
        </div>
      </div>

      {/* ROOM INFO */}

      <div
        className="bg-[#111827]
              border
              border-cyan-700/40
              rounded-2xl
              p-5"
      >
        <h2 className="text-2xl
              font-bold
              mb-6">Room Information</h2>

        <div className="grid gap-4 text-lg md:grid-cols-2">
          <div className="flex justify-between gap-6 rounded-lg bg-slate-950/60 px-4 py-3">
            <span className="text-slate-300">Room Name</span>

            <span className="font-semibold text-right">{room.name}</span>
          </div>

          <div className="flex justify-between gap-6 rounded-lg bg-slate-950/60 px-4 py-3">
            <span className="text-slate-300">Building</span>

            <span className="font-semibold text-right">{room.buildingName || "Default building"}</span>
          </div>

          <div className="flex justify-between gap-6 rounded-lg bg-slate-950/60 px-4 py-3">
            <span className="text-slate-300">Devices</span>

            <span className="font-semibold text-right">{devices.length}</span>
          </div>
        </div>
      </div>

      {/* CONNECTED DEVICES */}

      <div>
        <h2 className="text-3xl
              font-bold
              mb-4">Connected Devices</h2>

        <div
          className="grid
                grid-cols-1
                md:grid-cols-2
                xl:grid-cols-3
                gap-4"
        >
          {devices.map((device) => (
            <div
              key={device.deviceId}
              className="bg-[#111827]
                    rounded-2xl
                    p-5"
            >
              <div className="text-xl
                    font-bold">{device.deviceName}</div>

              <div className="text-slate-300
                    mt-2">{device.deviceModel}</div>
            </div>
          ))}
        </div>
      </div>

      {/* ANOMALIES */}

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

          {anomalies.map((anomaly, index) => (
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
