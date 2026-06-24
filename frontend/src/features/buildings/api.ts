import { request } from "../../shared/api/client";
import { getUserAnomalies } from "../anomalies/api";
import { getUserDevices } from "../devices/api";
import { getUserRooms } from "../rooms/api";
import type { Building } from "../../shared/api/types";

export async function getUserBuildings() {
  return request<Building[]>("/buildings");
}

export async function createUserBuilding(input: {
  name: string;
  address?: string;
  description?: string;
}) {
  return request<{ id: number }>("/buildings", {
    method: "POST",
    body: JSON.stringify(input),
  });
}

export async function getUserBuilding(id: string) {
  const buildings = await getUserBuildings();

  return (
    buildings.find((building) => String(building.id) === String(id)) ?? null
  );
}

export async function getUserBuildingRooms(buildingId: string) {
  const rooms = await getUserRooms();

  return rooms.filter((room) => String(room.buildingId) === String(buildingId));
}

export async function getUserBuildingDevices(buildingId: string) {
  const rooms = await getUserBuildingRooms(buildingId);
  const roomIds = new Set(rooms.map((room) => room.roomId));
  const devices = await getUserDevices();

  return devices.filter((device) => roomIds.has(device.roomId));
}

export async function getUserBuildingAnomalies(buildingId: string) {
  const rooms = await getUserBuildingRooms(buildingId);
  const roomNames = new Set(rooms.map((room) => room.roomName));
  const anomalies = await getUserAnomalies();

  return anomalies.filter((anomaly) => roomNames.has(anomaly.room));
}
