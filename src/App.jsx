import React from "react";

export default function App() {
  return (
    <main className="min-h-screen bg-gradient-to-br from-indigo-50 via-white to-teal-50 flex items-center justify-center p-8">
      <section className="max-w-4xl bg-white/80 backdrop-blur-sm rounded-3xl shadow-2xl p-10">
        <header className="flex items-center gap-4">
          <div className="w-16 h-16 rounded-xl bg-gradient-to-br from-indigo-500 to-teal-400 flex items-center justify-center text-white text-2xl font-bold">CZ</div>
          <div>
            <h1 className="text-2xl font-extrabold text-slate-800">Codespaces Beautiful Starter</h1>
            <p className="text-slate-600">Fast devcontainer, Vite + React, Tailwind, CI, linting &amp; tests.</p>
          </div>
        </header>

        <article className="mt-6 prose prose-slate">
          <h2>Look great instantly</h2>
          <p>This site is a polished starter: responsive, accessible, and ready for Codespaces or local development.</p>
          <ul>
            <li>Hot reloading with Vite</li>
            <li>Tailwind CSS for rapid styling</li>
            <li>Linting, formatting, unit &amp; E2E testing</li>
            <li>Devcontainer for Codespaces or VS Code Remote Containers</li>
          </ul>
        </article>

        <footer className="mt-8 text-sm text-slate-500">
          <span>Open the repository in Codespaces or run <code>npm install</code> and <code>npm run dev</code>.</span>
        </footer>
      </section>
    </main>
  );
}
