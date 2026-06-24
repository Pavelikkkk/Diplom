import { useEffect, useState, type FormEvent } from "react";
import { Link } from "react-router-dom";

import { createUserRoom, getAccount, getUserRooms, getUserBuildings, getUserDevices } from "../services/api";
import type { Account, Building, Device, Room } from "../services/api";

const roomIcon = "\u{1F6AA}";
const arrow = "\u2192";
const namePattern = /^(?=.*[A-Za-z0-9])[A-Za-z0-9 #.,'_-]{2,80}$/;

export default function Rooms() {
  const [rooms, setRooms] = useState<Room[]>([]);
  const [buildings, setBuildings] = useState<Building[]>([]);
  const [devices, setDevices] = useState<Device[]>([]);
  const [account, setAccount] = useState<Account | null>(null);
  const [search, setSearch] = useState("");
  const [selectedBuilding, setSelectedBuilding] = useState("all");
  const [form, setForm] = useState({
    roomName: "",
  });
  const [message, setMessage] = useState("");

  async function loadData() {
      try {
        const [roomsData, buildingsData, devicesData] = await Promise.all([
          getUserRooms(),
          getUserBuildings(),
          getUserDevices(),
        ]);

        setRooms(roomsData);
        setBuildings(buildingsData);
        setDevices(devicesData);
      } catch (error) {
        console.error(error);
      }
    }

  useEffect(() => {
    const timeoutId = window.setTimeout(() => {
      void loadData();
    }, 0);

    getAccount()
      .then(setAccount)
      .catch(() => setAccount(null));

    return () => window.clearTimeout(timeoutId);
  }, []);

  async function handleCreateRoom(event: FormEvent) {
    event.preventDefault();
    setMessage("");

    const roomName = form.roomName.trim();

    if (!namePattern.test(roomName)) {
      setMessage("Use a readable room name with at least one letter or number.");
      return;
    }

    const result = await createUserRoom({
      roomName,
      buildingId: selectedBuilding === "all" ? undefined : Number(selectedBuilding),
    });

    setMessage(`Room created. ID: ${result.id}`);
    setForm({
      roomName: "",
    });
    await loadData();
  }

  const filteredRooms = rooms.filter((room) => {
    const matchesSearch = room.name.toLowerCase().includes(search.toLowerCase());
    const matchesBuilding =
      selectedBuilding === "all" || room.buildingId.toString() === selectedBuilding;

    return matchesSearch && matchesBuilding;
  });

  const showBuildingFilter = account?.accountType === "BUSINESS" || account?.role === "ADMIN";

  return (
    <div className="space-y-8 2xl:space-y-10">
      <div>
        <h1 className="mb-2 text-4xl font-bold md:text-5xl 2xl:text-6xl">
          Rooms
        </h1>
        <p className="text-lg text-slate-300 2xl:text-xl">
          Browse and monitor connected rooms.
        </p>
      </div>

      <div className={`grid grid-cols-1 gap-4 ${showBuildingFilter ? "md:grid-cols-2" : ""}`}>
        <input
          type="text"
          placeholder="Search rooms..."
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          className="rounded-lg border border-cyan-700/40 bg-[#111827] p-4 text-base outline-none focus:border-cyan-400 md:p-5 md:text-lg"
        />

        {showBuildingFilter && (
          <select
            value={selectedBuilding}
            onChange={(e) => setSelectedBuilding(e.target.value)}
            className="rounded-lg border border-cyan-700/40 bg-[#111827] p-4 text-base outline-none focus:border-cyan-400 md:p-5 md:text-lg"
          >
            <option value="all">All Buildings</option>
            {buildings.map((building) => (
              <option key={building.id} value={building.id}>
                {building.name}
              </option>
            ))}
          </select>
        )}
      </div>

      <form
        onSubmit={handleCreateRoom}
        className="grid grid-cols-1 gap-4 rounded-lg border border-cyan-700/40 bg-[#111827] p-5 md:grid-cols-[1fr_auto]"
      >
        <input
          value={form.roomName}
          onChange={(event) => setForm({ ...form, roomName: event.target.value })}
          placeholder="Room name"
          required
          className="rounded-lg border border-cyan-700/40 bg-slate-950 p-3 outline-none focus:border-cyan-400"
        />
        <button className="rounded-lg bg-cyan-500 px-5 py-3 font-semibold text-slate-950 transition hover:bg-cyan-400">
          Create Room
        </button>
        {message && <div className="text-cyan-300 md:col-span-2">{message}</div>}
      </form>

      <div className="grid grid-cols-1 gap-5 md:grid-cols-2 xl:grid-cols-3 2xl:gap-6">
        {filteredRooms.map((room) => {
          const building = buildings.find((b) => b.id === room.buildingId);
          const deviceCount = devices.filter((device) => device.roomId === room.roomId).length;

          return (
            <Link
              key={room.roomId}
              to={`/rooms/${room.roomId}`}
              className="rounded-lg border border-cyan-700/40 bg-[#111827] p-5 transition hover:-translate-y-1 hover:border-cyan-400 2xl:p-6"
            >
              <div className="mb-4 text-4xl 2xl:text-5xl">{roomIcon}</div>

              <h2 className="mb-3 text-2xl font-bold text-cyan-400 2xl:text-3xl">
                {room.name}
              </h2>

              <div className="space-y-2 text-base 2xl:text-lg">
                <p className="text-slate-300">Building:</p>
                <p>{building?.name ?? "Unknown"}</p>
                <p className="pt-2 text-slate-300">Devices:</p>
                <p>{deviceCount}</p>
              </div>

              <div className="mt-6 font-semibold text-cyan-400">
                Open Room {arrow}
              </div>
            </Link>
          );
        })}
      </div>
    </div>
  );
}
