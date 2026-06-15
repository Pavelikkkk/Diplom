const overviewCards = [
  {
    icon: "\u26A1",
    title: "Supported Monitoring",
    description: "Track room activity and energy consumption in real time.",
    items: [
      "Energy Consumption",
      "Motion Detection",
      "Lighting Activity",
      "Device Connectivity",
      "Room Activity",
      "Usage Trends",
    ],
  },
  {
    icon: "\u{1F50D}",
    title: "Detect Anomalies",
    description:
      "Identify unusual behavior before it becomes an expensive problem.",
    items: [
      "Lights Left On",
      "Power Without Motion",
      "Abnormal Consumption",
      "Machine Learning Detection",
      "Real-Time Alerts",
      "Severity Classification",
    ],
  },
  {
    icon: "\u{1F4E8}",
    title: "Instant Notifications",
    description: "Stay informed with alerts and analytics wherever you are.",
    items: [
      "Telegram Alerts",
      "Active Incident Tracking",
      "Device Status Updates",
      "Historical Records",
      "Analytics Dashboard",
      "Building Monitoring",
    ],
  },
];

function OverviewCard({ card }: { card: (typeof overviewCards)[number] }) {
  return (
    <div className="rounded-lg border border-cyan-700/40 bg-[#111827] p-6 transition-all duration-300 hover:-translate-y-1 hover:border-cyan-400 sm:p-8">
      <div className="mb-6 text-5xl">
        {card.icon}
      </div>

      <h3 className="mb-3 text-2xl font-bold sm:text-3xl">{card.title}</h3>
      <p className="mb-6 text-slate-400">{card.description}</p>
      <div className="mb-6 h-px bg-slate-700" />

      <div className="space-y-3 text-base text-slate-200">
        {card.items.map((item) => (
          <div key={item}>{item}</div>
        ))}
      </div>
    </div>
  );
}

export function HomeOverview() {
  return (
    <section className="py-12 md:py-20">
      <h2 className="mb-4 text-center text-3xl font-bold sm:text-4xl">
        Smart Energy Monitoring
      </h2>

      <p className="mx-auto mb-10 max-w-3xl text-center text-base text-slate-400 sm:text-lg">
        Monitor energy usage, detect abnormal behavior and receive instant
        alerts across your entire environment.
      </p>

      <div className="grid grid-cols-1 gap-5 lg:grid-cols-3">
        {overviewCards.map((card) => (
          <OverviewCard key={card.title} card={card} />
        ))}
      </div>
    </section>
  );
}
