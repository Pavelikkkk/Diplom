import { useEffect, useMemo, useRef, useState } from "react";

export type AdminPickerOption = {
  value: number;
  label: string;
  detail?: string;
};

type AdminPickerProps = {
  options: AdminPickerOption[];
  value: number;
  placeholder: string;
  onChange: (value: number) => void;
  allowEmpty?: boolean;
  emptyLabel?: string;
};

export function AdminPicker({
  options,
  value,
  placeholder,
  onChange,
  allowEmpty = false,
  emptyLabel = "None",
}: AdminPickerProps) {
  const [open, setOpen] = useState(false);
  const [query, setQuery] = useState("");
  const pickerRef = useRef<HTMLDivElement | null>(null);
  const selected = options.find((option) => option.value === value);

  useEffect(() => {
    if (!open) {
      return;
    }

    function handlePointerDown(event: MouseEvent) {
      if (!pickerRef.current?.contains(event.target as Node)) {
        setOpen(false);
        setQuery("");
      }
    }

    document.addEventListener("mousedown", handlePointerDown);

    return () => document.removeEventListener("mousedown", handlePointerDown);
  }, [open]);

  const filteredOptions = useMemo(() => {
    const normalizedQuery = query.trim().toLowerCase();

    return options.filter((option) => {
      if (!normalizedQuery) {
        return true;
      }

      return `${option.label} ${option.detail ?? ""}`
        .toLowerCase()
        .includes(normalizedQuery);
    });
  }, [options, query]);

  function selectValue(nextValue: number) {
    onChange(nextValue);
    setOpen(false);
    setQuery("");
  }

  return (
    <div ref={pickerRef} className="relative">
      <button
        type="button"
        onClick={() => setOpen((current) => !current)}
        className="flex w-full items-center justify-between gap-3 rounded-xl border border-slate-700 bg-[#020617] px-4 py-3 text-left outline-none transition hover:border-cyan-500 focus:border-cyan-400"
      >
        <span className={selected || (allowEmpty && value === 0) ? "truncate text-white" : "truncate text-slate-400"}>
          {selected?.label ?? (allowEmpty && value === 0 ? emptyLabel : placeholder)}
        </span>
        <span className="text-cyan-300">{open ? "^" : "v"}</span>
      </button>

      {open && (
        <div className="absolute z-30 mt-2 w-full overflow-hidden rounded-xl border border-cyan-700/40 bg-slate-950 shadow-2xl shadow-black/50">
          <div className="border-b border-slate-800 p-2">
            <input
              value={query}
              onChange={(event) => setQuery(event.target.value)}
              placeholder="Search..."
              className="w-full rounded-lg border border-slate-700 bg-[#020617] px-3 py-2 outline-none focus:border-cyan-400"
            />
          </div>

          <div className="max-h-64 overflow-y-auto p-2">
            {allowEmpty && (
              <button
                type="button"
                onClick={() => selectValue(0)}
                className={`mb-1 w-full rounded-lg px-3 py-2 text-left transition hover:bg-slate-800 ${
                  value === 0 ? "bg-cyan-500/20 text-cyan-200" : "text-slate-200"
                }`}
              >
                {emptyLabel}
              </button>
            )}

            {filteredOptions.map((option) => (
              <button
                type="button"
                key={option.value}
                onClick={() => selectValue(option.value)}
                className={`mb-1 w-full rounded-lg px-3 py-2 text-left transition hover:bg-slate-800 ${
                  option.value === value ? "bg-cyan-500/20 text-cyan-200" : "text-slate-200"
                }`}
              >
                <span className="block truncate font-semibold">{option.label}</span>
                {option.detail && (
                  <span className="block truncate text-sm text-slate-400">{option.detail}</span>
                )}
              </button>
            ))}

            {filteredOptions.length === 0 && (
              <div className="px-3 py-6 text-center text-sm text-slate-400">
                No matches
              </div>
            )}
          </div>
        </div>
      )}
    </div>
  );
}
