import Navbar from "../components/Navbar";

type Props = {
  children: React.ReactNode;
};

export default function DashboardLayout({ children }: Props) {
  return (
    <div className="min-h-screen bg-[#020617] text-white">
      <Navbar />
      <main className="mx-auto w-full max-w-[1760px] px-4 pb-10 sm:px-6 lg:px-10 2xl:px-16">
        {children}
      </main>
    </div>
  );
}
