type Plan = {
  name: string;
  price: string;
  badge?: "Current" | "Popular" | "Best Choice";
  highlight?: boolean;
  currentWhen?: string;
  features: string[];
};

const personalPlans: Plan[] = [
  {
    name: "Free",
    price: "Free",
    badge: "Current",
    highlight: true,
    features: ["\u2705 2 Rooms", "\u2705 5 Devices", "\u2705 Anomalies", "\u274C Analytics"],
  },
  {
    name: "Basic",
    price: "$4.99 / month",
    features: ["\u2705 10 Rooms", "\u2705 30 Devices", "\u2705 Notifications", "\u2705 Full History"],
  },
  {
    name: "Pro",
    price: "$9.99 / month",
    badge: "Popular",
    highlight: true,
    features: ["\u2705 50 Rooms", "\u2705 100 Devices", "\u2705 AI Analytics", "\u2705 Priority Support"],
  },
];

const businessPlans: Plan[] = [
  {
    name: "Free",
    price: "Free",
    currentWhen: "FREE",
    features: ["\u2705 1 Building", "\u2705 5 Rooms", "\u2705 10 Devices", "\u274C Advanced Analytics"],
  },
  {
    name: "Standard",
    price: "$29.99 / month",
    badge: "Best Choice",
    currentWhen: "STANDARD",
    features: ["\u2705 5 Buildings", "\u2705 100 Rooms", "\u2705 500 Devices", "\u2705 Analytics"],
  },
  {
    name: "Enterprise",
    price: "Contact Sales",
    currentWhen: "ENTERPRISE",
    features: [
      "\u2705 Unlimited Buildings",
      "\u2705 Unlimited Rooms",
      "\u2705 Unlimited Devices",
      "\u2705 Team Management",
      "\u2705 Reports",
      "\u2705 Dedicated Support",
    ],
  },
];

function Badge({ children }: { children: React.ReactNode }) {
  return (
    <div className="mb-4 inline-block rounded-lg bg-cyan-500 px-3 py-2 text-sm font-semibold text-slate-950">
      {children}
    </div>
  );
}

function PlanCard({ plan, currentPlan }: { plan: Plan; currentPlan: string }) {
  const isCurrent = plan.currentWhen
    ? currentPlan === plan.currentWhen
    : plan.badge === "Current";
  const borderClass =
    isCurrent || plan.highlight ? "border-cyan-500" : "border-cyan-700/40";

  return (
    <div className={`rounded-lg border bg-[#111827] p-5 ${borderClass}`}>
      <div className="mb-2 flex flex-wrap items-center gap-2">
        {plan.badge && <Badge>{plan.badge}</Badge>}
        {isCurrent && plan.badge !== "Current" && <Badge>Current</Badge>}
      </div>

      <h3 className="text-2xl font-bold sm:text-3xl">{plan.name}</h3>
      <div className="mt-2 text-cyan-400">{plan.price}</div>

      <div className="mt-6 space-y-3">
        {plan.features.map((feature) => (
          <div key={feature}>{feature}</div>
        ))}
      </div>
    </div>
  );
}

function PlanSection({
  title,
  plans,
  currentPlan,
}: {
  title: string;
  plans: Plan[];
  currentPlan: string;
}) {
  return (
    <div>
      <h2 className="mb-6 text-3xl font-bold sm:text-4xl">{title}</h2>
      <div className="grid grid-cols-1 gap-5 lg:grid-cols-3">
        {plans.map((plan) => (
          <PlanCard key={plan.name} plan={plan} currentPlan={currentPlan} />
        ))}
      </div>
    </div>
  );
}

export function PersonalPlans({ currentPlan }: { currentPlan: string }) {
  return (
    <PlanSection
      title={`${"\u{1F464}"} Personal Plans`}
      plans={personalPlans}
      currentPlan={currentPlan}
    />
  );
}

export function BusinessPlans({ currentPlan }: { currentPlan: string }) {
  return (
    <PlanSection
      title={`${"\u{1F3E2}"} Business Plans`}
      plans={businessPlans}
      currentPlan={currentPlan}
    />
  );
}
