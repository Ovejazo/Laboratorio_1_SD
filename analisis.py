#!/usr/bin/env python3
import os
import argparse
import numpy as np
import matplotlib.pyplot as plt

SCHEDULE_NAMES = {0: "static", 1: "dynamic", 2: "guided"}

def load_bench(path):
    data = np.genfromtxt(path, comments="#")
    if data.ndim == 1:
        data = data.reshape(1, -1)
    # Formato esperado: threads schedule chunk time_mean time_std speedup efficiency sigma_Sp sigma_Ep
    return {
        "threads":   data[:,0].astype(int),
        "schedule":  data[:,1].astype(int),
        "chunk":     data[:,2].astype(int),
        "time_mean": data[:,3],
        "time_std":  data[:,4],
        "speedup":   data[:,5],
        "efficiency":data[:,6],
        "sigma_Sp":  data[:,7],
        "sigma_Ep":  data[:,8],
    }

def amdahl_speedup(p, f):
    p = np.asarray(p, dtype=float)
    return 1.0 / (f + (1.0 - f) / np.maximum(p, 1e-9))

def fit_amdahl(p_vals, Sp_vals):
    p = np.array(p_vals, dtype=float)
    Sp = np.array(Sp_vals, dtype=float)
    mask = (p > 0) & (Sp > 0) & np.isfinite(Sp)
    p = p[mask]; Sp = Sp[mask]
    if len(p) < 2:
        return 0.0
    f_grid = np.linspace(0.0, 0.99, 2001)
    err = np.empty_like(f_grid)
    for i, f in enumerate(f_grid):
        pred = amdahl_speedup(p, f)
        err[i] = np.mean((Sp - pred)**2)
    return f_grid[np.argmin(err)]

def parse_args():
    p = argparse.ArgumentParser(description="Analiza benchmark y genera gráficos de performance")
    p.add_argument("--infile", default=os.path.join("datos", "benchmark results.dat"),
                   help="Ruta al archivo de benchmark (.dat). Por defecto: datos/benchmark results.dat")
    p.add_argument("--outdir", default="datos",
                   help="Directorio de salida para gráficos. Por defecto: datos")
    p.add_argument("--outfile", default="performance plots.png",
                   help="Nombre del PNG de salida. Por defecto: 'performance plots.png'")
    return p.parse_args()

def main():
    args = parse_args()

    bench_path = args.infile
    if not os.path.isfile(bench_path):
        print(f"[ERROR] No se encontró '{bench_path}'. Ejecuta primero el benchmark.")
        return

    b = load_bench(bench_path)

    fig, axes = plt.subplots(1, 3, figsize=(18,5))
    threads = b["threads"]; sched = b["schedule"]

    # 1) Speedup vs Threads (mejor por threads) + Amdahl
    ax = axes[0]
    for s in sorted(np.unique(sched)):
        sel = (sched == s)
        t_all = threads[sel]
        Sp_all = b["speedup"][sel]
        e_all  = b["sigma_Sp"][sel]

        # Mejor por cantidad de threads
        t_plot, Sp_plot, e_plot = [], [], []
        for p in sorted(np.unique(t_all)):
            m = (t_all == p)
            if not np.any(m): continue
            idx_best = np.argmax(Sp_all[m])
            t_plot.append(p)
            Sp_plot.append(Sp_all[m][idx_best])
            e_plot.append(e_all[m][idx_best])

        t_plot = np.array(t_plot); Sp_plot = np.array(Sp_plot); e_plot = np.array(e_plot)

        ax.errorbar(t_plot, Sp_plot, yerr=e_plot, marker='o', lw=2, capsize=3,
                    label=f"{SCHEDULE_NAMES.get(s,str(s))} (exp)")

        f_est = fit_amdahl(t_plot, Sp_plot)
        p_dense = np.linspace(1, max(t_plot) if len(t_plot) else 1, 200)
        ax.plot(p_dense, amdahl_speedup(p_dense, f_est), '--', lw=2,
                label=f"{SCHEDULE_NAMES.get(s,str(s))} Amdahl (f≈{f_est:.3f})")

    ax.set_xlabel("Threads"); ax.set_ylabel("Speedup"); ax.set_title("Speedup vs Threads (Amdahl)")
    ax.grid(True, ls='--', alpha=0.3); ax.legend()

    # 2) Eficiencia vs Threads (mejor por threads)
    ax = axes[1]
    for s in sorted(np.unique(sched)):
        sel = (sched == s)
        t_all = threads[sel]
        Ef_all = b["efficiency"][sel]
        e_all  = b["sigma_Ep"][sel]

        t_plot, Ef_plot, e_plot = [], [], []
        for p in sorted(np.unique(t_all)):
            m = (t_all == p)
            if not np.any(m): continue
            idx_best = np.argmax(Ef_all[m])
            t_plot.append(p)
            Ef_plot.append(Ef_all[m][idx_best])
            e_plot.append(e_all[m][idx_best])

        ax.errorbar(t_plot, Ef_plot, yerr=e_plot, marker='o', lw=2, capsize=3,
                    label=f"{SCHEDULE_NAMES.get(s,str(s))}")

    ax.set_xlabel("Threads"); ax.set_ylabel("Eficiencia"); ax.set_title("Eficiencia vs Threads")
    ax.grid(True, ls='--', alpha=0.3); ax.legend()

    # 3) Tiempo vs Chunk (promedio por chunk, por schedule)
    ax = axes[2]
    chunks = b["chunk"]; Tm = b["time_mean"]; Ts = b["time_std"]
    for s in sorted(np.unique(sched)):
        sel = (sched==s)
        ch = chunks[sel]; tm = Tm[sel]; ts = Ts[sel]
        uniq = sorted(np.unique(ch))
        x,y,e = [],[],[]
        for c in uniq:
            m = (ch==c)
            x.append(c)
            y.append(np.mean(tm[m]))
            e.append(np.sqrt(np.mean(ts[m]**2)))
        ax.errorbar(x, y, yerr=e, marker='o', lw=2, capsize=3, label=SCHEDULE_NAMES.get(s,str(s)))

    ax.set_xlabel("Chunk"); ax.set_ylabel("Tiempo [s]"); ax.set_title("Tiempo vs Chunk")
    ax.grid(True, ls='--', alpha=0.3); ax.legend()

    plt.tight_layout()

    # Asegurar carpeta de salida y guardar
    os.makedirs(args.outdir, exist_ok=True)
    out_path = os.path.join(args.outdir, args.outfile)
    plt.savefig(out_path, dpi=150)
    print(f"OK: gráfico guardado en '{out_path}'.")

if __name__ == "__main__":
    main()