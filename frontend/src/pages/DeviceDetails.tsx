import { useEffect, useState } from "react";
import { useParams } from "react-router-dom";

import { getAccount, getUserDevice } from "../services/api";
import type { Account, Device } from "../services/api";

export default function DeviceDetails() {
  const { id } = useParams();

  const [device, setDevice] = useState<Device | null>(null);
  const [account, setAccount] = useState<Account | null>(null);

  const [loading, setLoading] = useState(true);

  useEffect(() => {
    async function loadDevice() {
      try {
        const [current, loadedAccount] = await Promise.all([
          getUserDevice(id!),
          getAccount().catch(() => null),
        ]);

        setDevice(current ?? null);
        setAccount(loadedAccount);
      } catch (error) {
        console.error(error);
      } finally {
        setLoading(false);
      }
    }

    if (id) {
      loadDevice();
    }
  }, [id]);

  if (loading) {
    return <div className="text-slate-300">Loading...</div>;
  }

  if (!device) {
    return (
      <div>
        <h1 className="text-4xl
              font-bold">Device Not Found</h1>

        <p className="text-slate-300
              mt-4">
          This device does not exist or is not assigned to your account.
        </p>
      </div>
    );
  }

  const isAdmin = account?.role === "ADMIN";
  const mqttTopic = device.mqttTopic ?? `devices/${device.deviceId}/+`;

  return (
    <div className="space-y-8">
      {/* HEADER */}

      <div>
        <div className="flex
              items-center
              gap-3
              mb-3">
          <span
            className={
              device.isOnline
                ? "text-emerald-400 text-2xl"
                : "text-rose-400 text-2xl"
            }
          >
            ●
          </span>

          <h1 className="text-5xl
                font-bold">{device.deviceName}</h1>
        </div>

        <p className="text-slate-300
              text-lg">{device.roomName}</p>
      </div>

      {/* STATS */}

      <div
        className="grid
              grid-cols-1
              md:grid-cols-2
              xl:grid-cols-4
              gap-4"
      >
        <div
          className="bg-[#111827]
                rounded-2xl
                p-5"
        >
          <p className="text-slate-300">Device ID</p>

          <h2
            className="text-xl
                  font-bold
                  text-cyan-400"
          >
            {device.deviceId}
          </h2>
        </div>

        <div
          className="bg-[#111827]
                rounded-2xl
                p-5"
        >
          <p className="text-slate-300">Model</p>

          <h2 className="text-xl
                font-bold">{device.deviceModel}</h2>
        </div>

        <div
          className="bg-[#111827]
                rounded-2xl
                p-5"
        >
          <p className="text-slate-300">Firmware</p>

          <h2 className="text-xl
                font-bold">{device.firmwareVersion}</h2>
        </div>

        <div
          className="bg-[#111827]
                rounded-2xl
                p-5"
        >
          <p className="text-slate-300">Status</p>

          <h2
            className={
              device.isOnline
                ? "text-xl font-bold text-emerald-400"
                : "text-xl font-bold text-rose-400"
            }
          >
            {device.isOnline ? "ONLINE" : "OFFLINE"}
          </h2>
        </div>
      </div>

      {/* DEVICE INFORMATION */}

      <div
        className="bg-[#111827]
              border
              border-cyan-700/40
              rounded-2xl
              p-5"
      >
        <h2 className="text-2xl
              font-bold
              mb-6">Device Information</h2>

        <div className="space-y-5">
          <div className="flex
                justify-between">
            <span className="text-slate-300">Device Name</span>

            <span>{device.deviceName}</span>
          </div>

          <div className="flex
                justify-between">
            <span className="text-slate-300">Room</span>

            <span>{device.roomName}</span>
          </div>

          {device.buildingName && (
            <div className="flex
                  justify-between">
              <span className="text-slate-300">Building</span>

              <span>{device.buildingName}</span>
            </div>
          )}

          <div className="flex
                justify-between">
            <span className="text-slate-300">Device Model</span>

            <span>{device.deviceModel}</span>
          </div>

          <div className="flex
                justify-between">
            <span className="text-slate-300">Firmware</span>

            <span>{device.firmwareVersion}</span>
          </div>

          <div className="flex
                justify-between">
            <span className="text-slate-300">Last Seen</span>

            <span>{device.lastSeenAt || "N/A"}</span>
          </div>

          {isAdmin && (
            <div className="flex
                  justify-between
                  gap-4">
              <span className="text-slate-300">MQTT Topic</span>

              <span className="break-all text-right text-cyan-300">{mqttTopic}</span>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
