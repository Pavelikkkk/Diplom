const stats = [
  ["1200+", "Connected Devices"],
  ["25+", "Buildings"],
  ["300+", "Active Users"],
  ["99.9%", "Uptime"],
];

function StatTile({ value, label }: { value: string; label: string }) {
  return (
    <div className="rounded-lg border border-cyan-700/40 bg-[#111827] p-5 sm:p-6 2xl:p-7">
      <div className="text-3xl font-bold text-cyan-400 sm:text-4xl 2xl:text-5xl">
        {value}
      </div>
      <div className="mt-2 text-sm text-slate-300 sm:text-base 2xl:text-lg">
        {label}
      </div>
    </div>
  );
}

export function HomeStats() {
  return (
    <section id="stats" className="py-10 md:py-16">
      <div className="grid grid-cols-1 gap-4 sm:grid-cols-2 lg:grid-cols-4 2xl:gap-6">
        {stats.map(([value, label]) => (
          <StatTile key={label} value={value} label={label} />
        ))}
      </div>
    </section>
  );
}
