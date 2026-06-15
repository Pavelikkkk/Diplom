const audiences = [
  {
    icon: "\u{1F3E0}",
    title: "Personal Users",
    description:
      "Monitor your rooms, connected devices and receive instant anomaly notifications.",
  },
  {
    icon: "\u{1F3E2}",
    title: "Organizations",
    description:
      "Manage buildings, users, energy analytics and AI anomaly detection.",
  },
];

export function HomeAudience() {
  return (
    <section className="py-12 md:py-20">
      <h2 className="mb-10 text-center text-3xl font-bold sm:text-4xl">
        Who Is It For?
      </h2>

      <div className="grid grid-cols-1 gap-5 md:grid-cols-2">
        {audiences.map((audience) => (
          <div
            key={audience.title}
            className="rounded-lg border border-cyan-700/40 bg-[#111827] p-6 sm:p-8"
          >
            <div className="mb-6 text-5xl">
              {audience.icon}
            </div>
            <h3 className="mb-3 text-2xl font-bold">{audience.title}</h3>
            <p className="text-slate-300">{audience.description}</p>
          </div>
        ))}
      </div>
    </section>
  );
}
