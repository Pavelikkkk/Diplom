import type { Account } from "../../services/api";

export function UpgradeHero({
  account,
  isBusiness,
  currentPlan,
}: {
  account: Account | null;
  isBusiness: boolean;
  currentPlan: string;
}) {
  return (
    <div className="rounded-lg border border-cyan-700/40 bg-[#111827] p-6 sm:p-8 lg:p-10">
      <div className="mb-6 text-6xl">
        {isBusiness ? "\u{1F3E2}" : "\u{1F3E0}"}
      </div>

      <h1 className="mb-4 text-3xl font-bold sm:text-4xl md:text-5xl">
        {isBusiness ? "Business Subscription" : "Personal Subscription"}
      </h1>

      <p className="text-base text-slate-300 sm:text-lg">
        Account Type:{" "}
        <span className="font-semibold text-cyan-400">
          {account?.accountType ?? "PERSONAL"}
        </span>
      </p>

      <p className="mt-2 text-base text-slate-300 sm:text-lg">
        Current Plan: {currentPlan}
      </p>
    </div>
  );
}
