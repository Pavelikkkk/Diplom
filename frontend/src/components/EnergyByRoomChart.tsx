import { useEffect, useState } from "react";

import {
  ResponsiveContainer,
  BarChart,
  Bar,
  XAxis,
  YAxis,
  Tooltip,
  Legend,
} from "recharts";

import { getEnergyByRoom } from "../services/api";
import type { EnergyByRoom } from "../services/api";

export default function EnergyByRoomChart({ buildingId }: { buildingId?: number | string }) {
  const [data, setData] = useState<EnergyByRoom[]>([]);

  useEffect(() => {
    getEnergyByRoom(buildingId).then(setData).catch(console.error);
  }, [buildingId]);

  return (
    <div className="bg-[#111827]
          border
          border-cyan-700/40
          rounded-2xl
          p-5">
      <h2 className="text-2xl
            font-bold
            mb-6">Energy By Room</h2>

      <div style={{ height: 350 }}>
        <ResponsiveContainer>
          <BarChart data={data} layout="vertical" margin={{ left: 24, right: 16 }}>
            <XAxis
              type="number"
              tick={{ fill: "#cbd5e1", fontSize: 14, fontWeight: 600 }}
              tickFormatter={(value) => Number(value).toFixed(1)}
            />

            <YAxis
              type="category"
              dataKey="roomName"
              width={120}
              tick={{ fill: "#cbd5e1", fontSize: 14, fontWeight: 600 }}
            />

            <Tooltip
              cursor={{ fill: "transparent" }}
              formatter={(value) => [`${Number(value).toFixed(2)} kW`, "Power"]}
              contentStyle={{
                backgroundColor: "#111827",
                border: "1px solid #164e63",
                borderRadius: "12px",
                color: "#e2e8f0",
                fontSize: "16px",
              }}
              labelStyle={{
                color: "#ffffff",
                fontWeight: 700,
              }}
            />

            <Legend
              formatter={() => "Power, kW"}
              wrapperStyle={{ color: "#e2e8f0", fontSize: 15, fontWeight: 700 }}
            />

            <Bar dataKey="power" fill="#60a5fa" activeBar={{ fill: "#60a5fa" }} />
          </BarChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}
