import { useEffect, useState, type FormEvent } from "react";
import { Link } from "react-router-dom";

import {
  getUserBuildings,
  createUserBuilding,
  getUserRooms,
  getUserDevices,
} from "../services/api";
import type { Building, Device, Room } from "../services/api";

const buildingIcon = "\u{1F3E2}";
const roomIcon = "\u{1F6AA}";
const deviceIcon = "\u{1F4E1}";
const arrow = "\u2192";
const namePattern = /^(?=.*[A-Za-z0-9])[A-Za-z0-9 #.,'_-]{2,80}$/;
const addressPattern = /^(?=.*[A-Za-z0-9])[A-Za-z0-9 #.,'_-]{3,120}$/;

function normalize(value: string) {
  return value.trim().toLowerCase();
}

export default function Buildings() {
  const [buildings, setBuildings] = useState<Building[]>([]);
  const [rooms, setRooms] = useState<Room[]>([]);
  const [devices, setDevices] = useState<Device[]>([]);
  const [search, setSearch] = useState("");
  const [form, setForm] = useState({ name: "", address: "", description: "" });
  const [message, setMessage] = useState("");
  const [visibleCount, setVisibleCount] = useState(6);

  async function loadData() {
      try {
        const [buildingsData, roomsData, devicesData] = await Promise.all([
          getUserBuildings(),
          getUserRooms(),
          getUserDevices(),
        ]);

        setBuildings(buildingsData);
        setRooms(roomsData);
        setDevices(devicesData);
      } catch (error) {
        console.error(error);
      }
    }

  useEffect(() => {
    const timeoutId = window.setTimeout(() => {
      void loadData();
    }, 0);

    return () => window.clearTimeout(timeoutId);
  }, []);

  async function handleCreateBuilding(event: FormEvent) {
    event.preventDefault();
    setMessage("");

    const name = form.name.trim();
    const address = form.address.trim();
    const description = form.description.trim();

    if (!namePattern.test(name)) {
      setMessage("Use a readable building name with at least one letter or number.");
      return;
    }

    if (!addressPattern.test(address)) {
      setMessage("Use a readable address with at least one letter or number.");
      return;
    }

    const duplicate = buildings.some(
      (building) =>
        normalize(building.name) === normalize(name) &&
        normalize(building.address) === normalize(address),
    );

    if (duplicate) {
      setMessage("A building with this name and address already exists.");
      return;
    }

    const result = await createUserBuilding({ name, address, description });
    setMessage(`Building created. ID: ${result.id}`);
    setForm({ name: "", address: "", description: "" });
    await loadData();
  }

  const filteredBuildings = buildings.filter((building) =>
    building.name.toLowerCase().includes(search.toLowerCase()),
  );

  return (
    <div className="space-y-8 2xl:space-y-10">
      <div>
        <h1 className="mb-2 text-4xl font-bold md:text-5xl 2xl:text-6xl">
          Buildings
        </h1>
        <p className="text-lg text-slate-300 2xl:text-xl">
          Manage monitored buildings and energy analytics.
        </p>
      </div>

      <div className="grid grid-cols-1 gap-4 md:grid-cols-3 2xl:gap-6">
        <Stat icon={buildingIcon} label="Buildings" value={buildings.length} />
        <Stat icon={roomIcon} label="Rooms" value={rooms.length} />
        <Stat icon={deviceIcon} label="Devices" value={devices.length} />
      </div>

      <input
        type="text"
        placeholder="Search buildings..."
        value={search}
        onChange={(e) => setSearch(e.target.value)}
        className="w-full rounded-lg border border-cyan-700/40 bg-[#111827] p-4 text-base outline-none focus:border-orange-300 md:p-5 md:text-lg"
      />

      <form
        onSubmit={handleCreateBuilding}
        className="grid grid-cols-1 gap-4 rounded-lg border border-cyan-700/40 bg-[#111827] p-5 md:grid-cols-[1fr_1fr_1fr_auto]"
      >
        <input
          value={form.name}
          onChange={(event) => setForm({ ...form, name: event.target.value })}
          placeholder="Building name"
          required
          minLength={2}
          maxLength={80}
          pattern="(?=.*[A-Za-z0-9])[A-Za-z0-9 #.,'_-]{2,80}"
          className="rounded-lg border border-cyan-700/40 bg-slate-950 p-3 outline-none focus:border-cyan-400"
        />
        <input
          value={form.address}
          onChange={(event) => setForm({ ...form, address: event.target.value })}
          placeholder="Address"
          required
          minLength={3}
          maxLength={120}
          pattern="(?=.*[A-Za-z0-9])[A-Za-z0-9 #.,'_-]{3,120}"
          className="rounded-lg border border-cyan-700/40 bg-slate-950 p-3 outline-none focus:border-cyan-400"
        />
        <input
          value={form.description}
          onChange={(event) => setForm({ ...form, description: event.target.value })}
          placeholder="Description"
          className="rounded-lg border border-cyan-700/40 bg-slate-950 p-3 outline-none focus:border-cyan-400"
        />
        <button className="rounded-lg bg-cyan-500 px-5 py-3 font-semibold text-slate-950 transition hover:bg-cyan-400">
          Create Building
        </button>
        {message && <div className="text-cyan-300 md:col-span-4">{message}</div>}
      </form>

      <div className="grid grid-cols-1 gap-5 md:grid-cols-2 xl:grid-cols-3 2xl:gap-6">
        {filteredBuildings.slice(0, visibleCount).map((building) => (
          <Link
            key={building.id}
            to={`/buildings/${building.id}`}
            className="rounded-lg border border-cyan-700/40 bg-[#111827] p-5 transition hover:-translate-y-1 hover:border-cyan-400 2xl:p-6"
          >
            <div className="mb-4 text-4xl 2xl:text-5xl">{buildingIcon}</div>

            <h2 className="mb-2 text-2xl font-bold text-cyan-400 2xl:text-3xl">
              {building.name}
            </h2>

            <p className="mb-4 text-slate-300">{building.address}</p>
            <p className="mb-6 text-slate-300">{building.description}</p>

            <div className="font-semibold text-cyan-400">
              Open Dashboard {arrow}
            </div>
          </Link>
        ))}
      </div>

      {visibleCount < filteredBuildings.length && (
        <div className="flex justify-center">
          <button
            onClick={() => setVisibleCount(visibleCount + 6)}
            className="rounded-lg bg-orange-400 px-8 py-4 font-semibold text-slate-950 transition hover:bg-orange-300"
          >
            Show More
          </button>
        </div>
      )}
    </div>
  );
}

function Stat({
  icon,
  label,
  value,
}: {
  icon: string;
  label: string;
  value: number;
}) {
  return (
    <div className="rounded-lg border border-cyan-700/40 bg-[#111827] p-5 2xl:p-6">
      <p className="text-slate-300">
        <span className="mr-2">{icon}</span>
        {label}
      </p>
      <h2 className="text-3xl font-bold 2xl:text-4xl">{value}</h2>
    </div>
  );
}
