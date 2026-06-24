import { useMemo, useState } from "react";

import type { AdminOverview } from "../../services/api";

export function AdminUsersTable({ overview }: { overview: AdminOverview }) {
  const [search, setSearch] = useState("");
  const [role, setRole] = useState("all");
  const [accountType, setAccountType] = useState("all");
  const [showAll, setShowAll] = useState(false);

  const users = useMemo(() => {
    const query = search.trim().toLowerCase();

    return overview.users.filter((user) => {
      const matchesSearch =
        !query ||
        user.username.toLowerCase().includes(query) ||
        user.email.toLowerCase().includes(query) ||
        user.organizationName.toLowerCase().includes(query);
      const matchesRole = role === "all" || user.role === role;
      const matchesType = accountType === "all" || user.accountType === accountType;

      return matchesSearch && matchesRole && matchesType;
    });
  }, [accountType, overview.users, role, search]);

  const visibleUsers = showAll ? users : users.slice(0, 6);

  return (
    <div
      className="bg-[#111827]
      border
      border-cyan-700/40
      rounded-3xl
      p-5
      overflow-x-auto"
    >
      <h2
        className="text-3xl
        font-bold
        mb-5"
      >
        Users & Workspaces
      </h2>

      <div className="mb-5 grid grid-cols-1 gap-3 md:grid-cols-[1fr_12rem_12rem]">
        <input
          value={search}
          onChange={(event) => setSearch(event.target.value)}
          placeholder="Search users..."
          className="rounded-lg border border-slate-700 bg-slate-950 px-4 py-3 outline-none focus:border-cyan-400"
        />
        <select
          value={role}
          onChange={(event) => setRole(event.target.value)}
          className="rounded-lg border border-slate-700 bg-slate-950 px-4 py-3 outline-none focus:border-cyan-400"
        >
          <option value="all">All roles</option>
          <option value="ADMIN">Admin</option>
          <option value="USER">User</option>
        </select>
        <select
          value={accountType}
          onChange={(event) => setAccountType(event.target.value)}
          className="rounded-lg border border-slate-700 bg-slate-950 px-4 py-3 outline-none focus:border-cyan-400"
        >
          <option value="all">All accounts</option>
          <option value="BUSINESS">Business</option>
          <option value="PERSONAL">Personal</option>
        </select>
      </div>

      <table
        className="w-full
        text-left"
      >
        <thead className="text-slate-300">
          <tr>
            <th className="py-3">User</th>
            <th>Email</th>
            <th>Role</th>
            <th>Plan</th>
            <th>Workspace</th>
            <th>Data</th>
          </tr>
        </thead>

        <tbody>
          {visibleUsers.map((user) => (
            <tr
              key={user.id}
              className="border-t
              border-slate-800"
            >
              <td className="py-3">{user.username}</td>
              <td>{user.email}</td>
              <td>{user.role}</td>
              <td>{user.plan}</td>
              <td>{user.organizationName}</td>
              <td>
                {user.buildingsCount} buildings, {user.roomsCount} rooms,{" "}
                {user.devicesCount} devices
              </td>
            </tr>
          ))}
        </tbody>
      </table>

      {users.length === 0 && (
        <div className="py-6 text-center text-slate-400">No users match these filters.</div>
      )}

      {users.length > 6 && (
        <div className="mt-5 flex justify-center">
          <button
            type="button"
            onClick={() => setShowAll((current) => !current)}
            className="rounded-lg border border-cyan-700/40 px-5 py-2 font-semibold text-cyan-300 transition hover:border-cyan-400 hover:text-cyan-200"
          >
            {showAll ? "Show first 6" : "Show more"}
          </button>
        </div>
      )}
    </div>
  );
}
