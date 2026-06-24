export type AccountType = "PERSONAL" | "BUSINESS";

export type Subscription = {
  plan: string;
  status: string;
  maxBuildings: number;
  maxRooms: number;
};

export type Account = {
  id: number;
  username: string;
  email: string;
  role: string;
  organizationId: number;
  accountType: AccountType;
  telegramChatId: string;
  subscription: Subscription;
};

export type Stats = {
  buildings: number;
  rooms: number;
  devices: number;
  anomalies: number;
};

export type Building = {
  id: number;
  name: string;
  address: string;
  description: string;
};

export type Room = {
  id: number;
  roomId: number;
  buildingId: number;
  name: string;
  roomName: string;
  roomType?: string;
  floorNumber?: number;
  minNormalPowerKw?: number;
  maxNormalPowerKw?: number;
  allowUnattendedPower?: boolean;
  buildingName: string;
  deviceCount?: number;
  motion: boolean;
  power: number;
  light: number;
};

export type AnomalySeverity = "CRITICAL" | "WARNING" | "INFO";
export type AnomalyStatus = "ACTIVE" | "ACKNOWLEDGED" | "RESOLVED";

export type Device = {
  deviceId: string;
  deviceName: string;
  deviceModel: string;
  firmwareVersion: string;
  roomId: number;
  buildingId: number;
  organizationId: number;
  roomName: string;
  buildingName?: string;
  mqttTopic?: string;
  isOnline: boolean;
  lastSeenAt: string;
};

export type Anomaly = {
  room: string;
  type: string;
  severity: AnomalySeverity;
  score: number;
  status?: AnomalyStatus;
  detectedAt: string;
};

export type PowerPoint = {
  time: string;
  power: number;
};

export type TopConsumer = {
  roomName: string;
  power: number;
};

export type AnomalyStatistic = {
  type: string;
  count: number;
};

export type EnergyByRoom = {
  roomName: string;
  power: number;
};

export type SeverityDistribution = {
  severity: string;
  count: number;
};

export type AdminUser = {
  id: number;
  username: string;
  email: string;
  role: string;
  accountType: AccountType;
  organizationId: number;
  organizationName: string;
  plan: string;
  buildingsCount: number;
  roomsCount: number;
  devicesCount: number;
};

export type AdminBuilding = {
  id: number;
  organizationId: number;
  name: string;
  address: string;
  description: string;
};

export type AdminRoom = {
  id: number;
  buildingId: number;
  roomName: string;
  roomType: string;
  floorNumber: number;
  minNormalPowerKw?: number;
  maxNormalPowerKw?: number;
  allowUnattendedPower?: boolean;
};

export type AdminDevice = {
  deviceId: string;
  deviceName: string;
  deviceModel: string;
  firmwareVersion: string;
  roomId: number;
  roomName: string;
  buildingId: number;
  organizationId: number;
  mqttTopic: string;
  isOnline: boolean;
  lastSeenAt: string;
};

export type AdminOverview = {
  users: AdminUser[];
  buildings: AdminBuilding[];
  rooms: AdminRoom[];
  devices: AdminDevice[];
};
