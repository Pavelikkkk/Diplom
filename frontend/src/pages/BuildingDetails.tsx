import { useParams, Link } from "react-router-dom";
import { useEffect, useState } from "react";

import { getBuildings, getRooms, getDevices } from "../services/api";
import type { Building, Device, Room } from "../services/api";

export default function BuildingDetails() {
  const { id } = useParams();

  const [building, setBuilding] = useState<Building | null>(null);

  const [rooms, setRooms] = useState<Room[]>([]);

  const [devices, setDevices] = useState<Device[]>([]);

  const [search, setSearch] = useState("");

  const [visibleRooms, setVisibleRooms] = useState(12);

  useEffect(() => {
    async function loadData() {
      try {
        const [buildings, roomsData, devicesData] = await Promise.all([
          getBuildings(),
          getRooms(),
          getDevices(),
        ]);

        const currentBuilding = buildings.find((b) => String(b.id) === id);

        setBuilding(currentBuilding ?? null);

        setRooms(roomsData.filter((r) => String(r.buildingId) === id));

        const buildingRoomIds = new Set(
          roomsData
            .filter((room) => String(room.buildingId) === id)
            .map((room) => room.roomId),
        );

        setDevices(devicesData.filter((device) => buildingRoomIds.has(device.roomId)));
      } catch (error) {
        console.error(error);
      }
    }

    loadData();
  }, [id]);

  if (!building) {
    return <div>Loading...</div>;
  }

  const filteredRooms = rooms.filter((room) =>
    room.roomName.toLowerCase().includes(search.toLowerCase()),
  );

  return (
    <div className="space-y-8">
      {/* HEADER */}

      <div>
        <h1 className="text-5xl
              font-bold
              mb-3">{building.name}</h1>

        <p className="text-slate-300">{building.address}</p>
      </div>

      {/* STATS */}

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
          <p className="text-slate-300">🚪 Rooms</p>

          <h2 className="text-3xl
                font-bold
                text-cyan-400">{rooms.length}</h2>
        </div>

        <div
          className="bg-[#111827]
                rounded-2xl
                p-5"
        >
          <p className="text-slate-300">📡 Devices</p>

          <h2 className="text-3xl
                font-bold
                text-cyan-400">{devices.length}</h2>
        </div>

        <div
          className="bg-[#111827]
                rounded-2xl
                p-5"
        >
          <p className="text-slate-300">⚡ Status</p>

          <h2 className="text-3xl
                font-bold
                text-emerald-400">ACTIVE</h2>
        </div>
      </div>

      {/* ANALYTICS */}

      <Link
        to={`/buildings/${id}/analytics`}
        className="block
              bg-cyan-500
              text-slate-900
              rounded-2xl
              p-5
              font-semibold
              text-center
              hover:bg-cyan-400
              transition"
      >
        Open Analytics →
      </Link>

      {/* SEARCH */}

      <input
        type="text"
        placeholder="Search room..."
        value={search}
        onChange={(e) => setSearch(e.target.value)}
        className="w-full
              bg-[#111827]
              border
              border-cyan-700/40
              rounded-2xl
              p-5
              outline-none
              focus:border-orange-300"
      />

      {/* ROOMS */}

      <div>
        <h2 className="text-3xl
              font-bold
              mb-5">Rooms</h2>

        <div
          className="grid
                grid-cols-1
                md:grid-cols-2
                xl:grid-cols-3
                gap-5"
        >
          {filteredRooms.slice(0, visibleRooms).map((room) => (
            <div
              key={room.id}
              className="bg-[#111827]
                    border
                    border-cyan-700/40
                    rounded-2xl
                    p-5
                    hover:border-cyan-400
                    transition"
            >
              <h3
                className="text-xl
                      font-bold
                      mb-2"
              >
                {room.roomName}
              </h3>

              <p className="text-slate-300
                    mb-4">{room.roomType}</p>

              <div className="space-y-2">
                <div>🏢 Floor {room.floorNumber}</div>

                <div>
                  📡 Devices:{" "}
                  {devices.filter((device) => device.roomId === room.roomId).length}
                </div>
              </div>
            </div>
          ))}
        </div>
      </div>

      {/* SHOW MORE */}

      {visibleRooms < filteredRooms.length && (
        <div className="flex
              justify-center">
          <button
            onClick={() => setVisibleRooms(visibleRooms + 12)}
            className="px-8
                  py-4
                  rounded-2xl
                  bg-cyan-500
                  text-slate-900
                  font-semibold
                  hover:bg-cyan-400
                  transition"
          >
            Show More
          </button>
        </div>
      )}
    </div>
  );
}
