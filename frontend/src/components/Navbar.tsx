import { Link } from "react-router-dom";
import { useEffect, useRef, useState } from "react";

import Sidebar from "./Sidebar";
import { clearToken, type Account, getAccount } from "../services/api";

const menuIcons = {
  account: "\u{1F464}",
  subscription: "\u{1F4B3}",
  settings: "\u2699\uFE0F",
  signOut: "\u{1F6AA}",
};

export default function Header() {
  const [profileOpen, setProfileOpen] = useState(false);
  const [account, setAccount] = useState<Account | null>(null);
  const profileRef = useRef<HTMLDivElement | null>(null);

  useEffect(() => {
    getAccount()
      .then(setAccount)
      .catch(() => setAccount(null));
  }, []);

  useEffect(() => {
    if (!profileOpen) {
      return;
    }

    function handlePointerDown(event: MouseEvent) {
      if (!profileRef.current?.contains(event.target as Node)) {
        setProfileOpen(false);
      }
    }

    document.addEventListener("mousedown", handlePointerDown);

    return () => document.removeEventListener("mousedown", handlePointerDown);
  }, [profileOpen]);

  const username = account?.username ?? "Guest";

  return (
    <header className="sticky top-0 z-40 border-b border-slate-800 bg-[#020617]">
      <div className="flex h-16 items-center px-4 sm:px-6 md:h-20 lg:px-8">
        <div className="flex items-center gap-4 sm:gap-5">
          <Sidebar />

          <Link to="/account" className="flex items-center gap-3" aria-label="Dashboard">
            <span className="inline-flex h-9 w-9 items-center justify-center text-3xl text-orange-400">
              {"\u26A1"}
            </span>
          </Link>
        </div>

        <div ref={profileRef} className="relative ml-auto flex items-center gap-3 sm:gap-4">
          <span className="hidden text-sm text-slate-300 sm:inline md:text-base">
            {username}
          </span>

          <button
            onClick={() => setProfileOpen(!profileOpen)}
            className="flex h-10 w-10 items-center justify-center rounded-full bg-cyan-500 text-base font-bold text-slate-950 transition hover:scale-105 md:h-11 md:w-11"
          >
            {username.charAt(0).toUpperCase()}
          </button>

          {profileOpen && (
            <div className="absolute right-0 top-12 w-[min(18rem,calc(100vw-2rem))] overflow-hidden rounded-lg border border-slate-800 bg-[#111827] shadow-2xl md:top-14">
              <div className="border-b border-slate-800 p-5">
                <div className="text-base font-semibold md:text-lg">{username}</div>
                <div className="truncate text-sm text-slate-300">{account?.email ?? ""}</div>
                <div className="mt-2 text-sm text-cyan-400">
                  {account?.subscription.plan ?? "STANDARD"} Plan
                </div>
              </div>

              <div className="p-2">
                <Link onClick={() => setProfileOpen(false)} to="/account" className="block rounded-lg px-4 py-3 hover:bg-slate-800">
                  <span className="mr-2">{menuIcons.account}</span>
                  Account
                </Link>
                <Link
                  to="/subscription"
                  onClick={() => setProfileOpen(false)}
                  className="block rounded-lg px-4 py-3 hover:bg-slate-800"
                >
                  <span className="mr-2">{menuIcons.subscription}</span>
                  Subscription
                </Link>
                <Link onClick={() => setProfileOpen(false)} to="/settings" className="block rounded-lg px-4 py-3 hover:bg-slate-800">
                  <span className="mr-2">{menuIcons.settings}</span>
                  Settings
                </Link>
              </div>

              <div className="border-t border-slate-800 p-2">
                <button
                  onClick={() => {
                    clearToken();
                    window.location.hash = "#/login";
                  }}
                  className="w-full rounded-lg px-4 py-3 text-left text-red-400 hover:bg-red-500/10"
                >
                  <span className="mr-2">{menuIcons.signOut}</span>
                  Sign Out
                </button>
              </div>
            </div>
          )}
        </div>
      </div>
    </header>
  );
}
