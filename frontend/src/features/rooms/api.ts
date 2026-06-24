import { request } from "../../shared/api/client";
import { getUserAnomalies } from "../anomalies/api";
import { getUserDevices } from "../devices/api";
import type { Room } from "../../shared/api/types";

export async function getUserRooms() {
  const rooms = await request<Array<Partial<Room>>>("/rooms");

  return rooms.map((room) => ({
    id: Number(room.id ?? room.roomId ?? 0),
    roomId: Number(room.id ?? room.roomId ?? 0),
    buildingId: Number(room.buildingId ?? 0),
    name: String(room.roomName ?? room.name ?? ""),
    roomName: String(room.roomName ?? room.name ?? ""),
    roomType: String(room.roomType ?? ""),
    floorNumber: Number(room.floorNumber ?? 0),
    buildingName: String(room.buildingName ?? ""),
    minNormalPowerKw: Number(room.minNormalPowerKw ?? 0),
    maxNormalPowerKw: Number(room.maxNormalPowerKw ?? 2.8),
    allowUnattendedPower: Boolean(room.allowUnattendedPower ?? false),
    motion: Boolean(room.motion ?? false),
    power: Number(room.power ?? 0),
    light: Number(room.light ?? 0),
  }));
}

export async function createUserRoom(input: {
  buildingId?: number;
  roomName: string;
  roomType?: string;
  floorNumber?: number;
}) {
  return request<{ id: number }>("/rooms", {
    method: "POST",
    body: JSON.stringify(input),
  });
}

export async function getUserRoom(id: string) {
  const rooms = await getUserRooms();

  return rooms.find((room) => String(room.roomId) === String(id)) ?? null;
}

export async function getUserRoomDevices(roomId: string) {
  const room = await getUserRoom(roomId);

  if (!room) {
    return [];
  }

  const devices = await getUserDevices();

  return devices.filter((device) => device.roomId === room.roomId);
}

export async function getUserRoomAnomalies(roomId: string) {
  const room = await getUserRoom(roomId);

  if (!room) {
    return [];
  }

  const anomalies = await getUserAnomalies();

  return anomalies.filter((anomaly) => anomaly.room === room.roomName);
}
