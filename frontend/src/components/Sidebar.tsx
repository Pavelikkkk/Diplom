import { Link } from "react-router-dom";
import { useEffect, useState } from "react";

import { type Account, getAccount } from "../services/api";

const icons = {
  account: "\u{1F464}",
  buildings: "\u{1F3E2}",
  analytics: "\u{1F4C8}",
  rooms: "\u{1F6AA}",
  devices: "\u{1F4E1}",
  anomalies: "\u26A0\uFE0F",
  settings: "\u2699\uFE0F",
  upgrade: "\u{1F680}",
};

export default function Sidebar() {
  const [open, setOpen] = useState(false);
  const [account, setAccount] = useState<Account | null>(null);

  useEffect(() => {
    getAccount()
      .then(setAccount)
      .catch(() => setAccount(null));
  }, []);

  const isBusiness = account?.accountType === "BUSINESS";
  const isAdmin = account?.role === "ADMIN";

  return (
    <>
      <button
        onClick={() => setOpen(true)}
        className="h-10 w-10 rounded-lg border border-cyan-700/40 bg-[#111827] text-slate-200 transition hover:border-orange-400 hover:text-orange-300 md:h-12 md:w-12"
        aria-label="Open menu"
      >
        {"\u2630"}
      </button>

      {open && (
        <div className="fixed inset-0 z-50 bg-black/60 backdrop-blur-sm">
          <aside className="flex h-screen w-[min(20rem,calc(100vw-2rem))] flex-col border-r border-slate-800 bg-slate-950 p-5">
            <div className="mb-10 flex items-center justify-between">
              <Link
                to="/account"
                onClick={() => setOpen(false)}
                className="flex items-center gap-3"
              >
                <span className="inline-flex h-10 w-10 items-center justify-center text-3xl text-orange-400">
                  {"\u26A1"}
                </span>
                <span className="text-2xl font-bold">SMATI</span>
              </Link>

              <button
                onClick={() => setOpen(false)}
                className="text-2xl transition hover:text-orange-300"
                aria-label="Close menu"
              >
                {"\u00D7"}
              </button>
            </div>

            <div className="space-y-4 text-base md:text-lg">
              <Link to="/account" onClick={() => setOpen(false)} className="block transition hover:text-orange-300">
                <span className="mr-2">{icons.account}</span>
                Account
              </Link>

              {isBusiness && (
                <>
                  <Link to="/buildings" onClick={() => setOpen(false)} className="block transition hover:text-orange-300">
                    <span className="mr-2">{icons.buildings}</span>
                    Buildings
                  </Link>
                  <Link to="/analytics" onClick={() => setOpen(false)} className="block transition hover:text-orange-300">
                    <span className="mr-2">{icons.analytics}</span>
                    Analytics
                  </Link>
                </>
              )}

              <Link to="/rooms" onClick={() => setOpen(false)} className="block transition hover:text-orange-300">
                <span className="mr-2">{icons.rooms}</span>
                Rooms
              </Link>
              <Link to="/devices" onClick={() => setOpen(false)} className="block transition hover:text-orange-300">
                <span className="mr-2">{icons.devices}</span>
                Devices
              </Link>
              <Link to="/anomalies" onClick={() => setOpen(false)} className="block transition hover:text-orange-300">
                <span className="mr-2">{icons.anomalies}</span>
                Anomalies
              </Link>
              <Link to="/settings" onClick={() => setOpen(false)} className="block transition hover:text-orange-300">
                <span className="mr-2">{icons.settings}</span>
                Settings
              </Link>

              {isAdmin && (
                <Link
                  to="/admin"
                  onClick={() => setOpen(false)}
                  className="block text-cyan-400 transition hover:text-orange-300"
                >
                  Admin
                </Link>
              )}

              <Link
                to="/upgrade"
                onClick={() => setOpen(false)}
                className="block pt-4 font-semibold text-cyan-400 transition hover:text-orange-300"
              >
                <span className="mr-2">{icons.upgrade}</span>
                Upgrade
              </Link>
            </div>

            <div className="mt-auto border-t border-slate-800 pt-6">
              <div className="mb-2 text-sm text-slate-300">Logged in as</div>
              <div className="text-lg font-semibold">{account?.username ?? "Guest"}</div>
              <div className="mt-2 text-sm text-cyan-400">
                {account?.accountType ?? "PERSONAL"}
              </div>
              <div className="text-sm text-slate-300">
                {account?.subscription.plan ?? "STANDARD"}
              </div>
            </div>
          </aside>
        </div>
      )}
    </>
  );
}
