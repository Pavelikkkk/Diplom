import { request } from "../../shared/api/client";
import type {
  AnomalyStatistic,
  EnergyByRoom,
  PowerPoint,
  SeverityDistribution,
  TopConsumer,
} from "../../shared/api/types";

function buildingQuery(buildingId?: number | string) {
  return buildingId ? `&buildingId=${buildingId}` : "";
}

export async function getPowerHistory(hours = 24, buildingId?: number | string) {
  return request<PowerPoint[]>(`/power/history?hours=${hours}${buildingQuery(buildingId)}`);
}

export async function getTopConsumers(buildingId?: number | string) {
  return request<TopConsumer[]>(`/analytics/top-consumers?limit=10${buildingQuery(buildingId)}`);
}

export async function getAnomalyStatistics(buildingId?: number | string) {
  return request<AnomalyStatistic[]>(`/analytics/anomalies-by-type?scope=all${buildingQuery(buildingId)}`);
}

export async function getSeverityDistribution(buildingId?: number | string) {
  return request<SeverityDistribution[]>(`/analytics/severity-distribution?scope=all${buildingQuery(buildingId)}`);
}

export async function getEnergyByRoom(buildingId?: number | string) {
  return request<EnergyByRoom[]>(`/analytics/energy-by-room?scope=all${buildingQuery(buildingId)}`);
}
