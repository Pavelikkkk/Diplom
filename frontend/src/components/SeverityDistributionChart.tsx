import { useEffect, useState } from "react";

import { ResponsiveContainer, PieChart, Pie, Tooltip, Cell, Legend } from "recharts";

import { getSeverityDistribution } from "../services/api";
import type { SeverityDistribution } from "../services/api";

const COLORS = ["#22c55e", "#f59e0b", "#ef4444"];

export default function SeverityDistributionChart({ buildingId }: { buildingId?: number | string }) {
  const [data, setData] = useState<SeverityDistribution[]>([]);

  useEffect(() => {
    getSeverityDistribution(buildingId).then(setData).catch(console.error);
  }, [buildingId]);

  return (
    <div className="bg-[#111827]
          border
          border-cyan-700/40
          rounded-2xl
          p-5">
      <h2 className="text-2xl
            font-bold
            mb-6">Severity Distribution</h2>

      <div style={{ height: 350 }}>
        <ResponsiveContainer>
          <PieChart>
            <Pie
              data={data}
              dataKey="count"
              nameKey="severity"
              outerRadius={110}
              label={{ fill: "#e2e8f0", fontSize: 14, fontWeight: 700 }}
            >
              {data.map((_, index) => (
                <Cell key={index} fill={COLORS[index % COLORS.length]} />
              ))}
            </Pie>

            <Tooltip
              contentStyle={{
                backgroundColor: "#111827",
                border: "1px solid #164e63",
                borderRadius: "12px",
                color: "#e2e8f0",
                fontSize: "16px",
              }}
              labelStyle={{ color: "#ffffff", fontWeight: 700 }}
            />
            <Legend wrapperStyle={{ color: "#e2e8f0", fontSize: 15, fontWeight: 700 }} />
          </PieChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}
