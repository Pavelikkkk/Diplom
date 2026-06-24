import { useEffect, useState } from "react";
import { Link } from "react-router-dom";

import { getUserDevices } from "../services/api";

type Device = {
  deviceId: string;
  deviceName: string;
  deviceModel: string;
  firmwareVersion: string;
  roomName: string;
  roomId: number;
  mqttTopic?: string;
  isOnline: boolean;
  lastSeenAt: string;
};

const deviceIcon = "\u{1F4E1}";
const greenDot = "\u25CF";
const arrow = "\u2192";

export default function Devices() {
  const [devices, setDevices] = useState<Device[]>([]);
  const [search, setSearch] = useState("");
  const [statusFilter, setStatusFilter] = useState<"all" | "online" | "offline">("all");

  async function loadDevices() {
    try {
      const deviceData = await getUserDevices();
      setDevices(deviceData);
    } catch (err) {
      console.error(err);
    }
  }

  useEffect(() => {
    const timeoutId = window.setTimeout(() => {
      void loadDevices();
    }, 0);

    return () => window.clearTimeout(timeoutId);
  }, []);

  const filteredDevices = devices.filter((device) => {
    const haystack = `${device.deviceName} ${device.deviceId} ${device.roomName}`.toLowerCase();
    const matchesSearch = haystack.includes(search.toLowerCase());
    const matchesStatus =
      statusFilter === "all" ||
      (statusFilter === "online" && device.isOnline) ||
      (statusFilter === "offline" && !device.isOnline);

    return matchesSearch && matchesStatus;
  });

  const onlineCount = devices.filter((d) => d.isOnline).length;
  const offlineCount = devices.filter((d) => !d.isOnline).length;

  return (
    <div className="space-y-8 2xl:space-y-10">
      <div>
        <h1 className="mb-2 text-4xl font-bold md:text-5xl 2xl:text-6xl">
          Devices
        </h1>
        <p className="text-lg text-slate-300 2xl:text-xl">
          Monitor and manage IoT devices.
        </p>
      </div>

      <div className="grid grid-cols-1 gap-4 sm:grid-cols-3 2xl:gap-6">
        <Stat icon={deviceIcon} label="Devices" value={devices.length} />
        <Stat label="Online" value={onlineCount} accent="text-emerald-400" />
        <Stat label="Offline" value={offlineCount} accent="text-rose-400" />
      </div>

      <input
        type="text"
        placeholder="Search devices..."
        value={search}
        onChange={(e) => setSearch(e.target.value)}
        className="w-full rounded-lg border border-cyan-700/40 bg-[#111827] p-4 text-base outline-none focus:border-cyan-400 md:p-5 md:text-lg"
      />

      <div className="flex flex-wrap gap-3">
        {(["all", "online", "offline"] as const).map((value) => (
          <button
            key={value}
            onClick={() => setStatusFilter(value)}
            className={`rounded-lg px-5 py-2 capitalize transition ${
              statusFilter === value && value === "online"
                ? "bg-emerald-400 text-slate-950"
                : statusFilter === value && value === "offline"
                  ? "bg-rose-400 text-slate-950"
                  : statusFilter === value
                    ? "bg-cyan-500 text-slate-950"
                    : "bg-[#111827] text-slate-200 hover:bg-slate-800"
            }`}
          >
            {value}
          </button>
        ))}
      </div>

      <div className="grid grid-cols-1 gap-5 md:grid-cols-2 xl:grid-cols-3 2xl:gap-6">
        {filteredDevices.map((device) => (
          <Link
            key={device.deviceId}
            to={`/devices/${device.deviceId}`}
            className="rounded-lg border border-cyan-700/40 bg-[#111827] p-5 transition hover:-translate-y-1 hover:border-cyan-400 2xl:p-6"
          >
            <div className="mb-4 flex items-start justify-between gap-3">
              <div>
                <div className="mb-3 text-4xl 2xl:text-5xl">{deviceIcon}</div>
                <h2 className="break-words text-2xl font-bold text-cyan-400 2xl:text-3xl">
                  {device.deviceName}
                </h2>
                <p className="mt-1 break-words text-sm text-slate-400">
                  {device.deviceId}
                </p>
              </div>
              <span className={device.isOnline ? "text-emerald-400" : "text-rose-400"}>
                {greenDot}
              </span>
            </div>

            <p className="mb-3 text-slate-300">{device.roomName || "No room"}</p>

            <div className="space-y-2 text-base 2xl:text-lg">
              <div>{device.deviceModel}</div>
              <div>{device.firmwareVersion}</div>
              <div className="break-all text-sm text-cyan-300">
                {device.mqttTopic ?? `devices/${device.deviceId}/+`}
              </div>
            </div>

            <div className="mt-6 font-semibold text-cyan-400">
              View Device {arrow}
            </div>
          </Link>
        ))}
      </div>
    </div>
  );
}

function Stat({
  icon,
  label,
  value,
  accent = "text-white",
}: {
  icon?: string;
  label: string;
  value: number;
  accent?: string;
}) {
  return (
    <div className="rounded-lg border border-cyan-700/40 bg-[#111827] p-5 2xl:p-6">
      <p className="text-slate-300">
        {icon && <span className="mr-2">{icon}</span>}
        {label}
      </p>
      <h2 className={`text-3xl font-bold 2xl:text-4xl ${accent}`}>{value}</h2>
    </div>
  );
}
