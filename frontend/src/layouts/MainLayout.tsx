import Header from "../components/Header";

type Props = {
  children: React.ReactNode;
};

export default function MainLayout({ children }: Props) {
  return (
    <div className="min-h-screen bg-[#020617] text-white">
      <Header />
      <main className="mx-auto w-full max-w-[1760px] px-4 py-6 sm:px-6 lg:px-10 lg:py-8 2xl:px-16">
        {children}
      </main>
    </div>
  );
}
