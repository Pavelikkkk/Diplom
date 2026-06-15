const links = [
  {
    label: "GitHub",
    href: "https://github.com/Pavelikkkk/dorm-energy-monitoring",
  },
  {
    label: "Telegram",
    href: "https://t.me/atcocoon",
  },
];

export function HomeFooter() {
  return (
    <footer id="about" className="mt-16 border-t border-cyan-700/40 py-12 md:mt-20">
      <div className="grid grid-cols-1 gap-10 md:grid-cols-3">
        <div>
          <div className="mb-4 inline-flex h-10 w-10 items-center justify-center rounded-lg bg-cyan-500 text-xl text-slate-950">
            ⚡
          </div>
          <h3 className="mb-3 text-2xl font-bold">Monitoring Platform</h3>
          <p className="text-slate-300">
            Smart IoT platform for energy monitoring, anomaly detection and
            analytics.
          </p>
        </div>

        <div>
          <h4 className="mb-4 text-xl font-semibold">Links</h4>
          <div className="space-y-3 text-slate-300">
            {links.map((link) => (
              <a
                key={link.href}
                href={link.href}
                target="_blank"
                rel="noreferrer"
                className="block hover:text-cyan-400"
              >
                {link.label}
              </a>
            ))}
          </div>
        </div>
      </div>

      <div className="mt-10 border-t border-slate-800 pt-6 text-center text-slate-300">
        Monitoring Platform · Version 2.0
      </div>
    </footer>
  );
}
