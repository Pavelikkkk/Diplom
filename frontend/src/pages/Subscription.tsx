import { Link } from "react-router-dom";
import { useEffect, useState } from "react";

import { type Account, getAccount } from "../services/api";

const icons = {
  plan: "\u{1F4B3}",
  account: "\u{1F464}",
  status: "\u2705",
  buildings: "\u{1F3E2}",
  rooms: "\u{1F6AA}",
  anomalies: "\u26A0\uFE0F",
  notifications: "\u{1F4E8}",
  dashboard: "\u{1F4CA}",
};

export default function Subscription() {
  const [account, setAccount] = useState<Account | null>(null);
  const [error, setError] = useState("");

  useEffect(() => {
    getAccount()
      .then(setAccount)
      .catch((err) =>
        setError(
          err instanceof Error ? err.message : "Failed to load subscription",
        ),
      );
  }, []);

  if (error) {
    return <div className="text-red-400">{error}</div>;
  }

  if (!account) {
    return <div className="text-slate-300">Loading subscription...</div>;
  }

  const isBusiness = account.accountType === "BUSINESS";

  return (
    <div className="space-y-8 2xl:space-y-10">
      <h1 className="text-4xl font-bold md:text-5xl 2xl:text-6xl">
        Subscription
      </h1>

      <div className="rounded-lg border border-cyan-700/40 bg-[#182234] p-5 2xl:p-6">
        <div className="grid grid-cols-1 gap-5 md:grid-cols-3">
          <SummaryItem
            icon={icons.plan}
            label="Current Plan"
            value={account.subscription.plan}
            large
          />
          <SummaryItem
            icon={icons.account}
            label="Account Type"
            value={account.accountType}
          />
          <SummaryItem
            icon={icons.status}
            label="Status"
            value={account.subscription.status}
            valueClassName="text-green-400"
          />
        </div>
      </div>

      <div className="grid grid-cols-1 gap-5 md:grid-cols-3">
        {isBusiness && (
          <LimitCard
            icon={icons.buildings}
            label="Buildings"
            value={account.subscription.maxBuildings}
          />
        )}
        <LimitCard icon={icons.rooms} label="Rooms" value={account.subscription.maxRooms} />
      </div>

      <div className="rounded-lg border border-cyan-700/40 bg-[#182234] p-5 2xl:p-6">
        <h2 className="mb-6 text-2xl font-bold 2xl:text-3xl">
          Included Features
        </h2>

        <div className="grid grid-cols-1 gap-4 md:grid-cols-2">
          <Feature icon={icons.rooms} label="Rooms" />
          <Feature icon={icons.anomalies} label="Anomaly Detection" />
          <Feature icon={icons.notifications} label="Notifications" />
          {isBusiness && <Feature icon={icons.buildings} label="Buildings" />}
          {isBusiness && (
            <Feature icon={icons.dashboard} label="Organization Dashboard" />
          )}
        </div>

        <Link
          to="/upgrade"
          className="mt-8 inline-block rounded-lg bg-cyan-500 px-8 py-3 font-semibold text-slate-950 transition hover:bg-cyan-400"
        >
          View Available Plans
        </Link>
      </div>
    </div>
  );
}

function SummaryItem({
  icon,
  label,
  value,
  large = false,
  valueClassName = "",
}: {
  icon: string;
  label: string;
  value: string;
  large?: boolean;
  valueClassName?: string;
}) {
  return (
    <div>
      <div className="mb-2 text-slate-300">
        <span className="mr-2">{icon}</span>
        {label}
      </div>
      <div className={`${large ? "text-4xl" : "text-xl"} font-bold ${valueClassName}`}>
        {value}
      </div>
    </div>
  );
}

function LimitCard({
  icon,
  label,
  value,
}: {
  icon: string;
  label: string;
  value: number;
}) {
  return (
    <div className="rounded-lg border border-cyan-700/40 bg-[#182234] p-5">
      <div className="text-slate-300">
        <span className="mr-2">{icon}</span>
        {label}
      </div>
      <div className="mt-2 text-3xl font-bold">{value}</div>
    </div>
  );
}

function Feature({ icon, label }: { icon: string; label: string }) {
  return (
    <div>
      <span className="mr-2">{icon}</span>
      {label}
    </div>
  );
}
