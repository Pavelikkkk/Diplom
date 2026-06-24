import { Link } from "react-router-dom";
import { useEffect, useState } from "react";

import { type Account, getAccount } from "../services/api";

export default function Account() {
  const [account, setAccount] = useState<Account | null>(null);

  const [error, setError] = useState("");

  useEffect(() => {
    getAccount()
      .then(setAccount)
      .catch((err) =>
        setError(err instanceof Error ? err.message : "Failed to load account"),
      );
  }, []);

  if (error) {
    return <div className="text-red-400">{error}</div>;
  }

  if (!account) {
    return <div className="text-slate-300">Loading account...</div>;
  }

  const isBusiness = account.accountType === "BUSINESS";

  return (
    <div className="space-y-8">
      <h1 className="text-5xl
            font-bold">Account</h1>

      <div className="grid
            grid-cols-1
            lg:grid-cols-3
            gap-5">
        <div className="bg-[#111827]
              border
              border-cyan-700/40
              rounded-3xl
              p-5">
          <div className="w-32
                h-32
                rounded-full
                bg-cyan-500
                flex
                items-center
                justify-center
                text-5xl
                font-bold
                text-slate-900
                mx-auto
                mb-6">
            {account.username.charAt(0).toUpperCase()}
          </div>

          <h2 className="text-3xl
                font-bold
                text-center">{account.username}</h2>

          <p className="text-cyan-400
                text-center
                mt-2">
            {account.accountType}
          </p>
        </div>

        <div className="bg-[#111827]
              border
              border-cyan-700/40
              rounded-3xl
              p-5">
          <div className="flex
                justify-between
                items-center
                mb-6">
            <h3 className="text-3xl
                  font-bold">Subscription</h3>

            <span className="px-4
                  py-2
                  rounded-xl
                  bg-cyan-500
                  text-slate-900
                  font-semibold">
              {account.subscription.plan}
            </span>
          </div>

          <div className="space-y-4">
            <div>
              <div className="text-slate-300">Account Type</div>
              <div className="text-xl">{account.accountType}</div>
            </div>

            <div>
              <div className="text-slate-300">Status</div>
              <div className="text-green-400">
                {account.subscription.status}
              </div>
            </div>
          </div>

          <Link
            to="/upgrade"
            className="inline-block
                  mt-8
                  px-8
                  py-3
                  rounded-2xl
                  bg-cyan-500
                  text-slate-900
                  font-semibold
                  hover:bg-cyan-400
                  transition"
          >
            Upgrade Plan
          </Link>
        </div>

        <div className="bg-[#111827]
              border
              border-cyan-700/40
              rounded-3xl
              p-5">
          <h3 className="text-3xl
                font-bold
                mb-6">Usage & Limits</h3>

          <div className="space-y-4">
            {isBusiness && (
              <div>
                <div className="text-slate-300">Buildings</div>
                <div className="text-xl">
                  {account.subscription.maxBuildings}
                </div>
              </div>
            )}

            <div>
              <div className="text-slate-300">Rooms</div>
              <div className="text-xl">{account.subscription.maxRooms}</div>
            </div>
          </div>
        </div>

        <div className="lg:col-span-3
              bg-[#111827]
              border
              border-cyan-700/40
              rounded-3xl
              p-5">
          <h3 className="text-3xl
                font-bold
                mb-8">Profile Information</h3>

          <div className="space-y-6">
            <div>
              <div className="text-slate-300">Username</div>
              <div className="text-xl">{account.username}</div>
            </div>

            <div>
              <div className="text-slate-300">Email</div>
              <div className="text-xl">{account.email}</div>
            </div>

            <div>
              <div className="text-slate-300">Telegram Chat ID</div>
              <div className="text-xl">
                {account.telegramChatId || "Not configured"}
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
