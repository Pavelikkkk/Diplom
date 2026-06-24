import { useEffect, useMemo, useState } from "react";

import {
  deleteAdminBuilding,
  deleteAdminDevice,
  deleteAdminRoom,
  type AdminOverview,
  type AdminBuilding,
  type AdminDevice,
  type AdminRoom,
  createAdminBuilding,
  createAdminDevice,
  createAdminRoom,
  getAccount,
  getAdminOverview,
  updateAdminBuilding,
  updateAdminDevice,
  updateAdminRoom,
} from "../services/api";
import {
  CreateBuildingForm,
  CreateDeviceForm,
  CreateRoomForm,
} from "./admin/AdminForms";
import { AdminPicker } from "./admin/AdminPicker";
import { AdminUsersTable } from "./admin/AdminUsersTable";

const visibleLimit = 5;
const namePattern = /^(?=.*[A-Za-z0-9])[A-Za-z0-9 #.,'_-]{2,80}$/;
const addressPattern = /^(?=.*[A-Za-z0-9])[A-Za-z0-9 #.,'_-]{3,120}$/;

function isReadableName(value: string) {
  return namePattern.test(value.trim());
}

function isReadableAddress(value: string) {
  return addressPattern.test(value.trim());
}

function includesQuery(values: Array<string | number | undefined>, query: string) {
  if (!query) {
    return true;
  }

  return values.some((value) => String(value ?? "").toLowerCase().includes(query));
}

function normalize(value: string) {
  return value.trim().toLowerCase();
}

export default function Admin() {
  const [overview, setOverview] = useState<AdminOverview | null>(null);
  const [error, setError] = useState("");
  const [status, setStatus] = useState("");
  const [selectedOrganizationId, setSelectedOrganizationId] = useState(0);
  const [buildingName, setBuildingName] = useState("");
  const [buildingAddress, setBuildingAddress] = useState("");
  const [buildingDescription, setBuildingDescription] = useState("");
  const [selectedBuildingId, setSelectedBuildingId] = useState(0);
  const [roomName, setRoomName] = useState("");
  const [roomType, setRoomType] = useState("ROOM");
  const [floorNumber, setFloorNumber] = useState(1);
  const [selectedRoomId, setSelectedRoomId] = useState(0);
  const [deviceId, setDeviceId] = useState("");
  const [deviceName, setDeviceName] = useState("");
  const [deviceModel, setDeviceModel] = useState("ESP32");
  const [firmwareVersion, setFirmwareVersion] = useState("1.0.0");
  const [buildingFormError, setBuildingFormError] = useState("");
  const [roomFormError, setRoomFormError] = useState("");
  const [deviceFormError, setDeviceFormError] = useState("");
  const [buildingSearch, setBuildingSearch] = useState("");
  const [roomSearch, setRoomSearch] = useState("");
  const [deviceSearch, setDeviceSearch] = useState("");
  const [showAllBuildings, setShowAllBuildings] = useState(false);
  const [showAllRooms, setShowAllRooms] = useState(false);
  const [showAllDevices, setShowAllDevices] = useState(false);
  const [editingBuilding, setEditingBuilding] = useState<AdminBuilding | null>(null);
  const [editingRoom, setEditingRoom] = useState<AdminRoom | null>(null);
  const [editingDevice, setEditingDevice] = useState<AdminDevice | null>(null);
  const [deleteTarget, setDeleteTarget] = useState<
    | { type: "building"; label: string; action: () => Promise<void> }
    | { type: "room"; label: string; action: () => Promise<void> }
    | { type: "device"; label: string; action: () => Promise<void> }
    | null
  >(null);

  async function fetchAdminOverview() {
    const account = await getAccount();

    if (account.role !== "ADMIN") {
      throw new Error("Admin role is required");
    }

    return getAdminOverview();
  }

  function applyAdminOverview(data: AdminOverview) {
    setOverview(data);

    const firstUser = data.users.find((user) => user.organizationId > 0);

    if (firstUser && !selectedOrganizationId) {
      setSelectedOrganizationId(firstUser.organizationId);
    }
  }

  async function loadAdmin() {
    setError("");

    try {
      const data = await fetchAdminOverview();
      applyAdminOverview(data);
    } catch (err) {
      setError(
        err instanceof Error ? err.message : "Failed to load admin data",
      );
    }
  }

  useEffect(() => {
    async function loadInitialAdmin() {
      try {
        const data = await fetchAdminOverview();

        setOverview(data);

        const firstUser = data.users.find((user) => user.organizationId > 0);

        if (firstUser) {
          setSelectedOrganizationId(firstUser.organizationId);
        }
      } catch (err) {
        setError(
          err instanceof Error ? err.message : "Failed to load admin data",
        );
      }
    }

    loadInitialAdmin();
  }, []);

  const organizationBuildings = useMemo(() => {
    if (!overview) {
      return [];
    }

    return overview.buildings.filter(
      (building) => building.organizationId === selectedOrganizationId,
    );
  }, [overview, selectedOrganizationId]);

  const organizationRooms = useMemo(() => {
    if (!overview) {
      return [];
    }

    const buildingIds = new Set(organizationBuildings.map((building) => building.id));
    return overview.rooms.filter((room) => buildingIds.has(room.buildingId));
  }, [organizationBuildings, overview]);

  const organizationDevices = useMemo(() => {
    if (!overview) {
      return [];
    }

    const roomIds = new Set(organizationRooms.map((room) => room.id));
    return overview.devices.filter(
      (device) =>
        device.organizationId === selectedOrganizationId ||
        (device.roomId > 0 && roomIds.has(device.roomId)),
    );
  }, [organizationRooms, overview, selectedOrganizationId]);

  const filteredBuildings = useMemo(() => {
    const query = buildingSearch.trim().toLowerCase();

    return organizationBuildings.filter((building) =>
      includesQuery([building.name, building.address, building.description], query),
    );
  }, [buildingSearch, organizationBuildings]);

  const filteredRooms = useMemo(() => {
    const query = roomSearch.trim().toLowerCase();

    return organizationRooms.filter((room) =>
      includesQuery([room.roomName, room.roomType, room.floorNumber], query),
    );
  }, [organizationRooms, roomSearch]);

  const filteredDevices = useMemo(() => {
    const query = deviceSearch.trim().toLowerCase();

    return organizationDevices.filter((device) =>
      includesQuery([device.deviceName, device.deviceId, device.roomName], query),
    );
  }, [deviceSearch, organizationDevices]);

  const visibleBuildings = showAllBuildings
    ? filteredBuildings
    : filteredBuildings.slice(0, visibleLimit);
  const visibleRooms = showAllRooms ? filteredRooms : filteredRooms.slice(0, visibleLimit);
  const visibleDevices = showAllDevices
    ? filteredDevices
    : filteredDevices.slice(0, visibleLimit);

  const workspaceOptions = overview?.users
    .filter((user) => user.organizationId > 0)
    .map((user) => ({
      value: user.organizationId,
      label: user.username,
      detail: user.organizationName || `Workspace ${user.organizationId}`,
    })) ?? [];
  const roomOptions = organizationRooms.map((room) => ({
    value: room.id,
    label: room.roomName,
    detail: `${room.roomType} - floor ${room.floorNumber}`,
  }));
  const selectedUser = overview?.users.find(
    (user) => user.organizationId === selectedOrganizationId,
  );

  function handleOrganizationChange(organizationId: number) {
    setSelectedOrganizationId(organizationId);
    setSelectedBuildingId(0);
    setSelectedRoomId(0);
    setBuildingSearch("");
    setRoomSearch("");
    setDeviceSearch("");
    setEditingBuilding(null);
    setEditingRoom(null);
    setEditingDevice(null);
    setDeleteTarget(null);
  }

  async function handleCreateBuilding(event: React.FormEvent) {
    event.preventDefault();
    setStatus("");
    setBuildingFormError("");

    const name = buildingName.trim();
    const address = buildingAddress.trim();
    const description = buildingDescription.trim();

    if (!selectedOrganizationId) {
      setBuildingFormError("Choose a user workspace first.");
      return;
    }

    if (!isReadableName(name)) {
      setBuildingFormError("Building name must be readable and include at least one letter or number.");
      return;
    }

    if (!isReadableAddress(address)) {
      setBuildingFormError("Address must be readable and include at least one letter or number.");
      return;
    }

    const duplicate = organizationBuildings.some(
      (building) =>
        normalize(building.name) === normalize(name) &&
        normalize(building.address) === normalize(address),
    );

    if (duplicate) {
      setBuildingFormError("This workspace already has a building with the same name and address.");
      return;
    }

    try {
      await createAdminBuilding({
        organizationId: selectedOrganizationId,
        name,
        address,
        description,
      });
    } catch (err) {
      setBuildingFormError(err instanceof Error ? err.message : "Failed to create building");
      return;
    }

    setBuildingName("");
    setBuildingAddress("");
    setBuildingDescription("");
    setStatus("Building created");
    await loadAdmin();
  }

  async function handleCreateRoom(event: React.FormEvent) {
    event.preventDefault();
    setStatus("");
    setRoomFormError("");

    const normalizedRoomName = roomName.trim();
    const normalizedRoomType = roomType.trim();

    if (!selectedBuildingId) {
      setRoomFormError("Choose a building first.");
      return;
    }

    if (!isReadableName(normalizedRoomName)) {
      setRoomFormError("Room name must be readable and include at least one letter or number.");
      return;
    }

    if (!isReadableName(normalizedRoomType)) {
      setRoomFormError("Room type must contain letters or numbers.");
      return;
    }

    const duplicate = organizationRooms.some(
      (room) =>
        room.buildingId === selectedBuildingId &&
        normalize(room.roomName) === normalize(normalizedRoomName),
    );

    if (duplicate) {
      setRoomFormError("This building already has a room with this name.");
      return;
    }

    try {
      await createAdminRoom({
        buildingId: selectedBuildingId,
        roomName: normalizedRoomName,
        roomType: normalizedRoomType,
        floorNumber,
      });
    } catch (err) {
      setRoomFormError(err instanceof Error ? err.message : "Failed to create room");
      return;
    }

    setRoomName("");
    setRoomType("ROOM");
    setFloorNumber(1);
    setStatus("Room created");
    await loadAdmin();
  }

  async function handleCreateDevice(event: React.FormEvent) {
    event.preventDefault();
    setStatus("");
    setDeviceFormError("");

    const normalizedDeviceId = deviceId.trim();
    const normalizedDeviceName = deviceName.trim();
    const alreadyExists = overview?.devices.some((device) => device.deviceId === normalizedDeviceId);

    if (!isReadableName(normalizedDeviceId) || !isReadableName(normalizedDeviceName)) {
      setDeviceFormError("Device ID and name must contain letters or numbers.");
      return;
    }

    if (!selectedRoomId) {
      setDeviceFormError("Choose a room for this device.");
      return;
    }

    if (!deviceModel.trim() || !firmwareVersion.trim()) {
      setDeviceFormError("Device model and firmware version are required.");
      return;
    }

    try {
      await createAdminDevice({
        roomId: selectedRoomId,
        deviceId: normalizedDeviceId,
        deviceName: normalizedDeviceName,
        deviceModel: deviceModel.trim(),
        firmwareVersion: firmwareVersion.trim(),
      });
    } catch (err) {
      setDeviceFormError(err instanceof Error ? err.message : "Failed to create device");
      return;
    }

    setDeviceId("");
    setDeviceName("");
    setStatus(alreadyExists ? "Device already existed, details updated" : "Device created");
    await loadAdmin();
  }

  async function handleUpdateBuilding(building: AdminBuilding) {
    if (!isReadableName(building.name) || !isReadableAddress(building.address)) {
      setStatus("Building name and address must contain letters or numbers.");
      return;
    }

    const duplicate = organizationBuildings.some(
      (item) =>
        item.id !== building.id &&
        normalize(item.name) === normalize(building.name) &&
        normalize(item.address) === normalize(building.address),
    );

    if (duplicate) {
      setStatus("This workspace already has a building with the same name and address.");
      return;
    }

    await updateAdminBuilding({
      id: building.id,
      organizationId: building.organizationId,
      name: building.name.trim(),
      address: building.address.trim(),
      description: building.description.trim(),
    });
    setEditingBuilding(null);
    setStatus("Building updated");
    await loadAdmin();
  }

  async function handleDeleteBuilding(building: AdminBuilding) {
    await deleteAdminBuilding(building.id);
    setDeleteTarget(null);
    setStatus("Building deleted");
    await loadAdmin();
  }

  async function handleUpdateRoom(room: AdminRoom) {
    if (!isReadableName(room.roomName) || !isReadableName(room.roomType)) {
      setStatus("Room name and type must contain letters or numbers.");
      return;
    }

    await updateAdminRoom({
      ...room,
      roomName: room.roomName.trim(),
      roomType: room.roomType.trim(),
    });
    setEditingRoom(null);
    setStatus("Room updated");
    await loadAdmin();
  }

  async function handleDeleteRoom(room: AdminRoom) {
    await deleteAdminRoom(room.id);
    setDeleteTarget(null);
    setStatus("Room deleted");
    await loadAdmin();
  }

  async function handleUpdateDevice(device: AdminDevice, roomId = device.roomId) {
    await updateAdminDevice({
      deviceId: device.deviceId,
      deviceName: device.deviceName,
      deviceModel: device.deviceModel,
      firmwareVersion: device.firmwareVersion,
      roomId,
    });
    setEditingDevice(null);
    setStatus("Device updated");
    await loadAdmin();
  }

  async function handleRenameDevice(device: AdminDevice) {
    if (!isReadableName(device.deviceName)) {
      setStatus("Device name must contain letters or numbers.");
      return;
    }

    await updateAdminDevice({
      deviceId: device.deviceId,
      deviceName: device.deviceName.trim(),
      deviceModel: device.deviceModel,
      firmwareVersion: device.firmwareVersion,
      roomId: device.roomId,
    });
    setEditingDevice(null);
    setStatus("Device updated");
    await loadAdmin();
  }

  async function handleDeleteDevice(device: AdminDevice) {
    await deleteAdminDevice(device.deviceId);
    setDeleteTarget(null);
    setStatus("Device deleted");
    await loadAdmin();
  }

  if (error) {
    return <div className="text-red-400">{error}</div>;
  }

  if (!overview) {
    return <div className="text-slate-300">Loading admin...</div>;
  }

  return (
    <div className="space-y-8">
      <h1
        className="text-5xl
        font-bold"
      >
        Admin
      </h1>

      {status && (
        <div
          className="bg-cyan-500/10
          border
          border-cyan-700/40
          rounded-2xl
          p-4
          text-cyan-300"
        >
          {status}
        </div>
      )}

      <AdminUsersTable overview={overview} />

      <section className="rounded-lg border border-cyan-700/40 bg-[#111827] p-5">
        <div className="mb-4 flex flex-col gap-2 lg:flex-row lg:items-end lg:justify-between">
          <div>
            <h2 className="text-2xl font-bold">Manage Workspace</h2>
            <p className="text-slate-300">
              Create, edit and delete resources only for the selected user workspace.
            </p>
          </div>
          {selectedUser && (
            <div className="text-left lg:text-right">
              <div className="text-lg font-semibold text-cyan-300">{selectedUser.username}</div>
              <div className="text-sm text-slate-400">{selectedUser.email}</div>
            </div>
          )}
        </div>
        <AdminPicker
          value={selectedOrganizationId}
          onChange={handleOrganizationChange}
          placeholder="Select workspace"
          options={workspaceOptions}
        />
      </section>

      <div
        className="grid
        grid-cols-1
        xl:grid-cols-3
        gap-5"
      >
        <CreateBuildingForm
          overview={overview}
          selectedOrganizationId={selectedOrganizationId}
          buildingName={buildingName}
          buildingAddress={buildingAddress}
          buildingDescription={buildingDescription}
          error={buildingFormError}
          onSubmit={handleCreateBuilding}
          onOrganizationChange={handleOrganizationChange}
          onBuildingNameChange={setBuildingName}
          onBuildingAddressChange={setBuildingAddress}
          onBuildingDescriptionChange={setBuildingDescription}
        />

        <CreateRoomForm
          buildings={organizationBuildings}
          selectedBuildingId={selectedBuildingId}
          roomName={roomName}
          roomType={roomType}
          floorNumber={floorNumber}
          error={roomFormError}
          onSubmit={handleCreateRoom}
          onBuildingChange={setSelectedBuildingId}
          onRoomNameChange={setRoomName}
          onRoomTypeChange={setRoomType}
          onFloorNumberChange={setFloorNumber}
        />

        <CreateDeviceForm
          rooms={organizationRooms}
          selectedRoomId={selectedRoomId}
          deviceId={deviceId}
          deviceName={deviceName}
          deviceModel={deviceModel}
          firmwareVersion={firmwareVersion}
          error={deviceFormError}
          onSubmit={handleCreateDevice}
          onRoomChange={setSelectedRoomId}
          onDeviceIdChange={setDeviceId}
          onDeviceNameChange={setDeviceName}
          onDeviceModelChange={setDeviceModel}
          onFirmwareVersionChange={setFirmwareVersion}
        />
      </div>

      <div className="grid grid-cols-1 gap-5 xl:grid-cols-3">
        <section className="rounded-lg border border-cyan-700/40 bg-[#111827] p-5">
          <div className="mb-4 flex items-center justify-between gap-3">
            <h2 className="text-2xl font-bold">Buildings</h2>
            <span className="text-sm text-slate-400">{filteredBuildings.length} total</span>
          </div>
          <input
            value={buildingSearch}
            onChange={(event) => setBuildingSearch(event.target.value)}
            placeholder="Search buildings..."
            className="mb-4 w-full rounded-lg border border-slate-700 bg-slate-950 px-4 py-3 outline-none focus:border-cyan-400"
          />
          <div className="space-y-3">
            {visibleBuildings.map((building) => (
              <div key={building.id} className="rounded-lg border border-slate-700 p-3">
                <div className="font-semibold text-cyan-300">{building.name}</div>
                <div className="text-sm text-slate-400">{building.address || "No address"}</div>
                {editingBuilding?.id === building.id && (
                  <div className="mt-3 space-y-2 rounded-lg border border-cyan-700/40 bg-slate-950 p-3">
                    <input value={editingBuilding.name} onChange={(event) => setEditingBuilding({ ...editingBuilding, name: event.target.value })} className="w-full rounded-lg border border-slate-700 bg-[#020617] px-3 py-2" />
                    <input value={editingBuilding.address} onChange={(event) => setEditingBuilding({ ...editingBuilding, address: event.target.value })} className="w-full rounded-lg border border-slate-700 bg-[#020617] px-3 py-2" />
                    <input value={editingBuilding.description} onChange={(event) => setEditingBuilding({ ...editingBuilding, description: event.target.value })} className="w-full rounded-lg border border-slate-700 bg-[#020617] px-3 py-2" />
                    <div className="flex gap-2">
                      <button onClick={() => void handleUpdateBuilding(editingBuilding)} className="rounded-lg bg-cyan-500 px-3 py-2 text-slate-950">Save</button>
                      <button onClick={() => setEditingBuilding(null)} className="rounded-lg bg-slate-700 px-3 py-2">Cancel</button>
                    </div>
                  </div>
                )}
                <div className="mt-3 flex gap-2">
                  <button onClick={() => setEditingBuilding(building)} className="rounded-lg bg-slate-700 px-3 py-2">
                    Edit
                  </button>
                  <button onClick={() => setDeleteTarget({ type: "building", label: building.name, action: () => handleDeleteBuilding(building) })} className="rounded-lg bg-rose-500 px-3 py-2 text-slate-950">
                    Delete
                  </button>
                </div>
                {deleteTarget?.type === "building" && deleteTarget.label === building.name && (
                  <div className="mt-3 rounded-lg border border-rose-500/40 bg-rose-500/10 p-3 text-sm text-rose-200">
                    <div className="mb-3">Delete {building.name}?</div>
                    <div className="flex gap-2">
                      <button onClick={() => void deleteTarget.action()} className="rounded-lg bg-rose-500 px-3 py-2 text-slate-950">Confirm</button>
                      <button onClick={() => setDeleteTarget(null)} className="rounded-lg bg-slate-700 px-3 py-2 text-white">Cancel</button>
                    </div>
                  </div>
                )}
              </div>
            ))}
          </div>
          {filteredBuildings.length > visibleLimit && (
            <button onClick={() => setShowAllBuildings(!showAllBuildings)} className="mt-4 w-full rounded-lg border border-cyan-700/40 px-3 py-2 text-cyan-300">
              {showAllBuildings ? "Show first 5" : "Show more"}
            </button>
          )}
        </section>

        <section className="rounded-lg border border-cyan-700/40 bg-[#111827] p-5">
          <div className="mb-4 flex items-center justify-between gap-3">
            <h2 className="text-2xl font-bold">Rooms</h2>
            <span className="text-sm text-slate-400">{filteredRooms.length} total</span>
          </div>
          <input
            value={roomSearch}
            onChange={(event) => setRoomSearch(event.target.value)}
            placeholder="Search rooms..."
            className="mb-4 w-full rounded-lg border border-slate-700 bg-slate-950 px-4 py-3 outline-none focus:border-cyan-400"
          />
          <div className="space-y-3">
            {visibleRooms.map((room) => (
              <div key={room.id} className="rounded-lg border border-slate-700 p-3">
                <div className="font-semibold text-cyan-300">{room.roomName}</div>
                <div className="text-sm text-slate-400">
                  {room.roomType} - floor {room.floorNumber}
                </div>
                <div className="mt-3 flex gap-2">
                  <button onClick={() => setEditingRoom(room)} className="rounded-lg bg-slate-700 px-3 py-2">
                    Edit
                  </button>
                  <button onClick={() => setDeleteTarget({ type: "room", label: room.roomName, action: () => handleDeleteRoom(room) })} className="rounded-lg bg-rose-500 px-3 py-2 text-slate-950">
                    Delete
                  </button>
                </div>
                {editingRoom?.id === room.id && (
                  <div className="mt-3 space-y-2 rounded-lg border border-cyan-700/40 bg-slate-950 p-3">
                    <input value={editingRoom.roomName} onChange={(event) => setEditingRoom({ ...editingRoom, roomName: event.target.value })} className="w-full rounded-lg border border-slate-700 bg-[#020617] px-3 py-2" />
                    <input value={editingRoom.roomType} onChange={(event) => setEditingRoom({ ...editingRoom, roomType: event.target.value })} className="w-full rounded-lg border border-slate-700 bg-[#020617] px-3 py-2" />
                    <input type="number" value={editingRoom.floorNumber} onChange={(event) => setEditingRoom({ ...editingRoom, floorNumber: Number(event.target.value) })} className="w-full rounded-lg border border-slate-700 bg-[#020617] px-3 py-2" />
                    <div className="flex gap-2">
                      <button onClick={() => void handleUpdateRoom(editingRoom)} className="rounded-lg bg-cyan-500 px-3 py-2 text-slate-950">Save</button>
                      <button onClick={() => setEditingRoom(null)} className="rounded-lg bg-slate-700 px-3 py-2">Cancel</button>
                    </div>
                  </div>
                )}
                {deleteTarget?.type === "room" && deleteTarget.label === room.roomName && (
                  <div className="mt-3 rounded-lg border border-rose-500/40 bg-rose-500/10 p-3 text-sm text-rose-200">
                    <div className="mb-3">Delete {room.roomName}? Move or delete its devices first.</div>
                    <div className="flex gap-2">
                      <button onClick={() => void deleteTarget.action()} className="rounded-lg bg-rose-500 px-3 py-2 text-slate-950">Confirm</button>
                      <button onClick={() => setDeleteTarget(null)} className="rounded-lg bg-slate-700 px-3 py-2 text-white">Cancel</button>
                    </div>
                  </div>
                )}
              </div>
            ))}
          </div>
          {filteredRooms.length > visibleLimit && (
            <button onClick={() => setShowAllRooms(!showAllRooms)} className="mt-4 w-full rounded-lg border border-cyan-700/40 px-3 py-2 text-cyan-300">
              {showAllRooms ? "Show first 5" : "Show more"}
            </button>
          )}
        </section>

        <section className="rounded-lg border border-cyan-700/40 bg-[#111827] p-5">
          <div className="mb-4 flex items-center justify-between gap-3">
            <h2 className="text-2xl font-bold">Devices</h2>
            <span className="text-sm text-slate-400">{filteredDevices.length} total</span>
          </div>
          <input
            value={deviceSearch}
            onChange={(event) => setDeviceSearch(event.target.value)}
            placeholder="Search devices..."
            className="mb-4 w-full rounded-lg border border-slate-700 bg-slate-950 px-4 py-3 outline-none focus:border-cyan-400"
          />
          <div className="space-y-3">
            {visibleDevices.map((device) => (
              <div key={device.deviceId} className="rounded-lg border border-slate-700 p-3">
                <div className="break-all font-semibold text-cyan-300">{device.deviceName}</div>
                <div className="break-all text-sm text-slate-400">{device.deviceId}</div>
                <AdminPicker
                  value={device.roomId}
                  onChange={(roomId) => void handleUpdateDevice(device, roomId)}
                  placeholder="Select room"
                  options={roomOptions}
                />
                <div className="mt-3 flex gap-2">
                  <button onClick={() => setEditingDevice(device)} className="rounded-lg bg-slate-700 px-3 py-2">
                    Edit
                  </button>
                  <button onClick={() => setDeleteTarget({ type: "device", label: device.deviceId, action: () => handleDeleteDevice(device) })} className="rounded-lg bg-rose-500 px-3 py-2 text-slate-950">
                    Delete
                  </button>
                </div>
                {editingDevice?.deviceId === device.deviceId && (
                  <div className="mt-3 space-y-2 rounded-lg border border-cyan-700/40 bg-slate-950 p-3">
                    <input value={editingDevice.deviceName} onChange={(event) => setEditingDevice({ ...editingDevice, deviceName: event.target.value })} className="w-full rounded-lg border border-slate-700 bg-[#020617] px-3 py-2" />
                    <AdminPicker
                      value={editingDevice.roomId}
                      onChange={(roomId) => setEditingDevice({ ...editingDevice, roomId })}
                      placeholder="Select room"
                      options={roomOptions}
                    />
                    <div className="flex gap-2">
                      <button onClick={() => void handleRenameDevice(editingDevice)} className="rounded-lg bg-cyan-500 px-3 py-2 text-slate-950">Save</button>
                      <button onClick={() => setEditingDevice(null)} className="rounded-lg bg-slate-700 px-3 py-2">Cancel</button>
                    </div>
                  </div>
                )}
                {deleteTarget?.type === "device" && deleteTarget.label === device.deviceId && (
                  <div className="mt-3 rounded-lg border border-rose-500/40 bg-rose-500/10 p-3 text-sm text-rose-200">
                    <div className="mb-3">Delete {device.deviceId}?</div>
                    <div className="flex gap-2">
                      <button onClick={() => void deleteTarget.action()} className="rounded-lg bg-rose-500 px-3 py-2 text-slate-950">Confirm</button>
                      <button onClick={() => setDeleteTarget(null)} className="rounded-lg bg-slate-700 px-3 py-2 text-white">Cancel</button>
                    </div>
                  </div>
                )}
              </div>
            ))}
          </div>
          {filteredDevices.length > visibleLimit && (
            <button onClick={() => setShowAllDevices(!showAllDevices)} className="mt-4 w-full rounded-lg border border-cyan-700/40 px-3 py-2 text-cyan-300">
              {showAllDevices ? "Show first 5" : "Show more"}
            </button>
          )}
        </section>
      </div>
    </div>
  );
}

