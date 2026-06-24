import { useEffect, useState } from "react";

import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  Tooltip,
  ResponsiveContainer,
  CartesianGrid,
} from "recharts";

import { getPowerHistory } from "../services/api";

type ChartPowerPoint = {
  time: string;
  power: number;
};

const ranges = [
  { label: "Day", hours: 24, subtitle: "Average power consumption over the last day" },
  { label: "7 Days", hours: 168, subtitle: "Average power consumption over the last 7 days" },
  { label: "Month", hours: 720, subtitle: "Average power consumption over the last month" },
];

function formatPower(value: number) {
  return `${value.toFixed(2)} kW`;
}

export default function PowerChart({ buildingId }: { buildingId?: number | string }) {
  const [data, setData] = useState<ChartPowerPoint[]>([]);
  const [rangeHours, setRangeHours] = useState(24);

  useEffect(() => {
    getPowerHistory(rangeHours, buildingId)
      .then((history) => {
        const formatted = history.map((item) => ({
          time: new Date(item.time).toLocaleString([], rangeHours <= 24
            ? { hour: "2-digit", minute: "2-digit" }
            : { month: "short", day: "numeric" }),

          power: Number(item.power),
        }));

        setData(formatted);
      })
      .catch(console.error);
  }, [buildingId, rangeHours]);

  const current = data.length > 0 ? data[data.length - 1].power : 0;

  const peak = data.length > 0 ? Math.max(...data.map((x) => x.power)) : 0;

  const average =
    data.length > 0
      ? data.reduce((sum, item) => sum + item.power, 0) / data.length
      : 0;
  const activeRange = ranges.find((range) => range.hours === rangeHours) ?? ranges[0];

  return (
    <div className="w-full">
      {/* Header */}

      <div className="flex
            flex-col
            lg:flex-row
            lg:justify-between
            lg:items-center
            mb-6
            gap-4">
        <div>
          <h3 className="text-xl
                font-semibold
                text-white">
            Energy Usage Analytics
          </h3>

          <p className="text-slate-300
                text-sm">
            {activeRange.subtitle}
          </p>
        </div>

        <div className="flex flex-wrap gap-2">
          {ranges.map((range) => (
            <button
              key={range.hours}
              onClick={() => setRangeHours(range.hours)}
              className={`rounded-lg px-4 py-2 text-sm font-semibold transition ${
                rangeHours === range.hours
                  ? "bg-cyan-500 text-slate-950"
                  : "bg-slate-950 text-slate-200 hover:bg-slate-800"
              }`}
            >
              {range.label}
            </button>
          ))}
        </div>
      </div>

      {/* Metrics */}

      <div className="grid
            grid-cols-3
            gap-4
            mb-6">
        <div className="bg-[#020617]
              rounded-lg
              p-4">
          <p className="text-slate-300
                text-sm">Current Power</p>

          <h3 className="text-2xl
                font-bold
                text-cyan-400">
            {formatPower(current)}
          </h3>
        </div>

        <div className="bg-[#020617]
              rounded-lg
              p-4">
          <p className="text-slate-300
                text-sm">Average Power</p>

          <h3 className="text-2xl
                font-bold
                text-emerald-400">{formatPower(average)}</h3>
        </div>

        <div className="bg-[#020617]
              rounded-lg
              p-4">
          <p className="text-slate-300
                text-sm">Peak Power</p>

          <h3 className="text-2xl
                font-bold
                text-orange-400">
            {formatPower(peak)}
          </h3>
        </div>
      </div>

      {/* Chart */}

      <div style={{ height: 350 }}>
        <ResponsiveContainer width="100%" height="100%">
          <LineChart data={data}>
            <CartesianGrid stroke="#334155" strokeDasharray="3 3" />

            <XAxis dataKey="time" stroke="#cbd5e1" tick={{ fill: "#cbd5e1", fontSize: 13, fontWeight: 600 }} />

            <YAxis
              stroke="#cbd5e1"
              tick={{ fill: "#cbd5e1", fontSize: 13, fontWeight: 600 }}
              tickFormatter={(value) => Number(value).toFixed(1)}
              label={{
                value: "Power, kW",
                angle: -90,
                position: "insideLeft",
                fill: "#e2e8f0",
                fontSize: 14,
                fontWeight: 700,
              }}
            />

            <Tooltip
              formatter={(value) => [formatPower(Number(value)), "Power"]}
              contentStyle={{
                backgroundColor: "#0f172a",
                border: "1px solid #334155",
                borderRadius: "10px",
                color: "#e2e8f0",
                fontSize: "16px",
              }}
              labelStyle={{
                color: "#ffffff",
                fontWeight: 700,
              }}
            />

            <Line
              type="monotone"
              dataKey="power"
              stroke="#06b6d4"
              strokeWidth={4}
              dot={false}
              activeDot={{
                r: 8,
                fill: "#06b6d4",
              }}
            />
          </LineChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}
