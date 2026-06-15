import { Link } from "react-router-dom";

export default function Header() {
  const scrollToSection = (id: string) => {
    document.getElementById(id)?.scrollIntoView({
      behavior: "smooth",
      block: "start",
    });
  };

  return (
    <header className="sticky top-0 z-30 border-b border-slate-800 bg-[#020617]/85 backdrop-blur-md">
      <div className="mx-auto flex h-16 w-full max-w-[1760px] items-center justify-between px-4 sm:px-6 md:h-20 lg:px-10 2xl:px-16">
        <div className="flex items-center gap-6 lg:gap-10">
          <Link
            to="/"
            onClick={() => window.scrollTo({ top: 0, behavior: "smooth" })}
            className="group flex items-center gap-3 font-bold"
            aria-label="Home"
          >
            <span className="inline-flex h-10 w-10 items-center justify-center text-3xl text-orange-400 transition group-hover:scale-105">
              {"\u26A1"}
            </span>
          </Link>

          <nav className="hidden items-center gap-6 text-sm md:flex lg:gap-8 lg:text-base">
            <button
              onClick={() => scrollToSection("features")}
              className="text-slate-300 transition hover:text-orange-300"
            >
              Features
            </button>
            <button
              onClick={() => scrollToSection("stats")}
              className="text-slate-300 transition hover:text-orange-300"
            >
              Statistics
            </button>
            <button
              onClick={() => scrollToSection("about")}
              className="text-slate-300 transition hover:text-orange-300"
            >
              About
            </button>
          </nav>
        </div>

        <div className="flex items-center gap-2 sm:gap-3">
          <Link
            to="/login"
            className="rounded-lg px-3 py-2 text-sm text-slate-300 transition hover:bg-orange-400/10 hover:text-orange-300 sm:px-5 sm:text-base"
          >
            Sign in
          </Link>
          <Link
            to="/register"
            className="rounded-lg bg-cyan-500 px-4 py-2 text-sm font-semibold text-slate-950 shadow-lg shadow-cyan-500/20 transition hover:bg-cyan-400 sm:px-6 sm:text-base"
          >
            Sign up
          </Link>
        </div>
      </div>
    </header>
  );
}
