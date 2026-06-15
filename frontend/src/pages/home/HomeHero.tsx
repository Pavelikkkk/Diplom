import { Link } from "react-router-dom";

export function HomeHero() {
  return (
    <section className="flex min-h-[calc(100vh-5rem)] flex-col items-center justify-center text-center">
      <div className="mb-8 inline-flex h-16 w-16 items-center justify-center text-6xl text-orange-400 md:h-20 md:w-20 md:text-7xl">
        {"\u26A1"}
      </div>

      <h1 className="mb-6 max-w-6xl text-4xl font-bold leading-tight sm:text-6xl lg:text-7xl 2xl:text-8xl">
        The Future of Smart Monitoring
      </h1>

      <p className="mb-10 max-w-4xl text-base text-slate-300 sm:text-xl 2xl:text-2xl">
        Real-time monitoring, machine learning anomaly detection and analytics
        platform for dormitories and smart buildings.
      </p>

      <div className="flex w-full flex-col justify-center gap-3 sm:w-auto sm:flex-row">
        <Link
          to="/register"
          className="rounded-lg bg-cyan-500 px-7 py-3 text-base font-semibold text-slate-950 transition hover:bg-cyan-400 2xl:px-9 2xl:py-4 2xl:text-lg"
        >
          Start now
        </Link>

        <button
          disabled
          className="rounded-lg border border-slate-600 px-7 py-3 text-base text-slate-300 2xl:px-9 2xl:py-4 2xl:text-lg"
        >
          API Reference (Coming soon)
        </button>
      </div>
    </section>
  );
}
