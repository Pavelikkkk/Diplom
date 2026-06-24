import type { AdminBuilding, AdminOverview, AdminRoom } from "../../services/api";
import { AdminPicker } from "./AdminPicker";

const fieldClass = `w-full
px-4
py-3
rounded-xl
bg-[#020617]
border
border-slate-700`;

const formClass = `bg-[#111827]
border
border-cyan-700/40
rounded-3xl
p-5
space-y-4`;

const buttonClass = `w-full
py-3
rounded-xl
bg-cyan-500
text-slate-900
font-semibold
disabled:opacity-50`;

type CreateBuildingFormProps = {
  overview: AdminOverview;
  selectedOrganizationId: number;
  buildingName: string;
  buildingAddress: string;
  buildingDescription: string;
  error: string;
  onSubmit: (event: React.FormEvent) => void;
  onOrganizationChange: (value: number) => void;
  onBuildingNameChange: (value: string) => void;
  onBuildingAddressChange: (value: string) => void;
  onBuildingDescriptionChange: (value: string) => void;
};

export function CreateBuildingForm({
  overview,
  selectedOrganizationId,
  buildingName,
  buildingAddress,
  buildingDescription,
  error,
  onSubmit,
  onOrganizationChange,
  onBuildingNameChange,
  onBuildingAddressChange,
  onBuildingDescriptionChange,
}: CreateBuildingFormProps) {
  return (
    <form
      onSubmit={onSubmit}
      className={formClass}
    >
      <h2
        className="text-2xl
        font-bold"
      >
        Create Building
      </h2>

      <AdminPicker
        value={selectedOrganizationId}
        onChange={onOrganizationChange}
        placeholder="Select workspace"
        options={overview.users
          .filter((user) => user.organizationId > 0)
          .map((user) => ({
            value: user.organizationId,
            label: user.username,
            detail: user.organizationName || `Workspace ${user.organizationId}`,
          }))}
      />

      <input
        required
        minLength={2}
        maxLength={80}
        pattern="(?=.*[A-Za-z0-9])[A-Za-z0-9 #.,'_-]{2,80}"
        title="Use a readable name with at least one letter or number."
        placeholder="Building name"
        value={buildingName}
        onChange={(event) => onBuildingNameChange(event.target.value)}
        className={fieldClass}
      />

      <input
        required
        minLength={3}
        maxLength={120}
        pattern="(?=.*[A-Za-z0-9])[A-Za-z0-9 #.,'_-]{3,120}"
        title="Use a readable address with letters or numbers."
        placeholder="Address"
        value={buildingAddress}
        onChange={(event) => onBuildingAddressChange(event.target.value)}
        className={fieldClass}
      />

      <input
        placeholder="Description"
        value={buildingDescription}
        onChange={(event) => onBuildingDescriptionChange(event.target.value)}
        className={fieldClass}
      />

      {error && <div className="text-sm text-rose-300">{error}</div>}

      <button className={buttonClass}>Create</button>
    </form>
  );
}

type CreateRoomFormProps = {
  buildings: AdminBuilding[];
  selectedBuildingId: number;
  roomName: string;
  roomType: string;
  floorNumber: number;
  error: string;
  onSubmit: (event: React.FormEvent) => void;
  onBuildingChange: (value: number) => void;
  onRoomNameChange: (value: string) => void;
  onRoomTypeChange: (value: string) => void;
  onFloorNumberChange: (value: number) => void;
};

export function CreateRoomForm({
  buildings,
  selectedBuildingId,
  roomName,
  roomType,
  floorNumber,
  error,
  onSubmit,
  onBuildingChange,
  onRoomNameChange,
  onRoomTypeChange,
  onFloorNumberChange,
}: CreateRoomFormProps) {
  return (
    <form
      onSubmit={onSubmit}
      className={formClass}
    >
      <h2
        className="text-2xl
        font-bold"
      >
        Create Room
      </h2>

      <AdminPicker
        value={selectedBuildingId}
        onChange={onBuildingChange}
        placeholder="Select building"
        allowEmpty
        emptyLabel="Select building"
        options={buildings.map((building) => ({
          value: building.id,
          label: building.name,
          detail: building.address,
        }))}
      />

      <input
        required
        minLength={2}
        maxLength={80}
        pattern="(?=.*[A-Za-z0-9])[A-Za-z0-9 #.,'_-]{2,80}"
        title="Use a readable name with at least one letter or number."
        placeholder="Room name"
        value={roomName}
        onChange={(event) => onRoomNameChange(event.target.value)}
        className={fieldClass}
      />

      <input
        required
        minLength={2}
        maxLength={40}
        pattern="(?=.*[A-Za-z0-9])[A-Za-z0-9 _-]{2,40}"
        title="Use a readable room type."
        placeholder="Room type"
        value={roomType}
        onChange={(event) => onRoomTypeChange(event.target.value)}
        className={fieldClass}
      />

      <input
        type="number"
        min={-10}
        max={200}
        value={floorNumber}
        onChange={(event) => onFloorNumberChange(Number(event.target.value))}
        className={fieldClass}
      />

      {error && <div className="text-sm text-rose-300">{error}</div>}

      <button
        disabled={!selectedBuildingId}
        className={buttonClass}
      >
        Create
      </button>
    </form>
  );
}

type CreateDeviceFormProps = {
  rooms: AdminRoom[];
  selectedRoomId: number;
  deviceId: string;
  deviceName: string;
  deviceModel: string;
  firmwareVersion: string;
  error: string;
  onSubmit: (event: React.FormEvent) => void;
  onRoomChange: (value: number) => void;
  onDeviceIdChange: (value: string) => void;
  onDeviceNameChange: (value: string) => void;
  onDeviceModelChange: (value: string) => void;
  onFirmwareVersionChange: (value: string) => void;
};

export function CreateDeviceForm({
  rooms,
  selectedRoomId,
  deviceId,
  deviceName,
  deviceModel,
  firmwareVersion,
  error,
  onSubmit,
  onRoomChange,
  onDeviceIdChange,
  onDeviceNameChange,
  onDeviceModelChange,
  onFirmwareVersionChange,
}: CreateDeviceFormProps) {
  return (
    <form
      onSubmit={onSubmit}
      className={formClass}
    >
      <h2
        className="text-2xl
        font-bold"
      >
        Create Device
      </h2>

      <AdminPicker
        value={selectedRoomId}
        onChange={onRoomChange}
        placeholder="Select room"
        options={rooms.map((room) => ({
          value: room.id,
          label: room.roomName,
          detail: `${room.roomType} - floor ${room.floorNumber}`,
        }))}
      />

      <input
        required
        minLength={2}
        maxLength={80}
        placeholder="Device id"
        value={deviceId}
        onChange={(event) => onDeviceIdChange(event.target.value)}
        className={fieldClass}
      />

      <input
        required
        minLength={2}
        maxLength={100}
        placeholder="Device name"
        value={deviceName}
        onChange={(event) => onDeviceNameChange(event.target.value)}
        className={fieldClass}
      />

      <input
        required
        minLength={2}
        maxLength={60}
        placeholder="Device model"
        value={deviceModel}
        onChange={(event) => onDeviceModelChange(event.target.value)}
        className={fieldClass}
      />

      <input
        required
        minLength={2}
        maxLength={40}
        placeholder="Firmware version"
        value={firmwareVersion}
        onChange={(event) => onFirmwareVersionChange(event.target.value)}
        className={fieldClass}
      />

      {error && <div className="text-sm text-rose-300">{error}</div>}

      <button
        className={buttonClass}
      >
        Create
      </button>
    </form>
  );
}
