import { request } from "../../shared/api/client";
import type { AdminOverview } from "../../shared/api/types";

export async function getAdminOverview() {
  return request<AdminOverview>("/admin/overview");
}

export async function createAdminBuilding(input: {
  organizationId: number;
  name: string;
  address: string;
  description: string;
}) {
  return request<{ id: number }>("/admin/buildings", {
    method: "POST",
    body: JSON.stringify(input),
  });
}

export async function createAdminRoom(input: {
  buildingId: number;
  roomName: string;
  roomType: string;
  floorNumber: number;
}) {
  return request<{ id: number }>("/admin/rooms", {
    method: "POST",
    body: JSON.stringify(input),
  });
}

export async function createAdminDevice(input: {
  deviceId: string;
  deviceName: string;
  deviceModel: string;
  firmwareVersion: string;
  roomId: number;
}) {
  return request<{ success: boolean }>("/admin/devices", {
    method: "POST",
    body: JSON.stringify(input),
  });
}

export async function updateAdminBuilding(input: {
  id: number;
  organizationId: number;
  name: string;
  address: string;
  description: string;
}) {
  return request<{ success: boolean }>("/admin/buildings/update", {
    method: "POST",
    body: JSON.stringify(input),
  });
}

export async function deleteAdminBuilding(id: number) {
  return request<{ success: boolean }>("/admin/buildings/delete", {
    method: "POST",
    body: JSON.stringify({ id }),
  });
}

export async function updateAdminRoom(input: {
  id: number;
  buildingId: number;
  roomName: string;
  roomType: string;
  floorNumber: number;
  minNormalPowerKw?: number;
  maxNormalPowerKw?: number;
  allowUnattendedPower?: boolean;
}) {
  return request<{ success: boolean }>("/admin/rooms/update", {
    method: "POST",
    body: JSON.stringify(input),
  });
}

export async function deleteAdminRoom(id: number) {
  return request<{ success: boolean }>("/admin/rooms/delete", {
    method: "POST",
    body: JSON.stringify({ id }),
  });
}

export async function updateAdminDevice(input: {
  deviceId: string;
  deviceName: string;
  deviceModel: string;
  firmwareVersion: string;
  roomId: number;
}) {
  return request<{ success: boolean }>("/admin/devices/update", {
    method: "POST",
    body: JSON.stringify(input),
  });
}

export async function deleteAdminDevice(deviceId: string) {
  return request<{ success: boolean }>("/admin/devices/delete", {
    method: "POST",
    body: JSON.stringify({ deviceId }),
  });
}
