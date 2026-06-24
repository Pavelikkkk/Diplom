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

import { getTopConsumers } from "../services/api";
import type { TopConsumer } from "../services/api";

export default function TopConsumersChart() {
  const [data, setData] = useState<TopConsumer[]>([]);

  useEffect(() => {
    getTopConsumers().then(setData).catch(console.error);
  }, []);

  return (
    <div className="bg-[#111827]
          border
          border-cyan-700/40
          rounded-2xl
          p-5">
      <h2 className="text-2xl
            font-bold
            mb-6">Top Power Consumers</h2>

      <div style={{ height: 300 }}>
        <ResponsiveContainer>
          <BarChart data={data}>
            <XAxis dataKey="roomName" stroke="#cbd5e1" tick={{ fill: "#cbd5e1", fontSize: 13, fontWeight: 600 }} />

            <YAxis stroke="#cbd5e1" tick={{ fill: "#cbd5e1", fontSize: 13, fontWeight: 600 }} tickFormatter={(value) => Number(value).toFixed(1)} />

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
              labelStyle={{ color: "#ffffff", fontWeight: 700 }}
            />

            <Legend formatter={() => "Power, kW"} wrapperStyle={{ color: "#e2e8f0", fontSize: 15, fontWeight: 700 }} />

            <Bar dataKey="power" fill="#22d3ee" activeBar={{ fill: "#22d3ee" }} />
          </BarChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}
