#!/usr/bin/env python3
import os
import numpy as np
import matplotlib.pyplot as plt

SCHEDULE_NAMES = {0: "static", 1: "dynamic", 2: "guided"}

def load_bench(path):
    data = np.genfromtxt(path, comments="#")
    if data.ndim == 1:
        data = data.reshape(1, -1)
    return {
        "threads": data[:,0].astype(int),
        "schedule": data[:,1].astype(int),
        "chunk": data[:,2].astype(int),
        "time_mean": data[:,3],
        "time_std": data[:,4],
        "speedup": data[:,5],
        "efficiency": data[:,6],
        "sigma_Sp": data[:,7],
        "sigma_Ep": data[:,8],
    }

def amdahl_speedup(p, f):
    # S(p) = 1 / (f + (1-f)/p)
    p = np.asarray(p, dtype=float)
    return 1.0 / (f + (1.0 - f) / np.maximum(p, 1e-9))

def fit_amdahl(p_vals, Sp_vals):
    """
    Ajusta f en S = 1 / (f + (1-f)/p) por búsqueda en malla [0, 0.99].
    Devuelve f que minimiza el MSE.
    """
    p = np.array(p_vals, dtype=float)
    Sp = np.array(Sp_vals, dtype=float)
    mask = (p > 0) & (Sp > 0) & np.isfinite(Sp)
    p = p[mask]; Sp = Sp[mask]
    if len(p) < 2:
        return 0.0
    f_grid = np.linspace(0.0, 0.99, 2001)
    best_f = 0.0
    best_err = float('inf')
    for f in f_grid:
        pred = amdahl_speedup(p, f)
        err = np.mean((Sp - pred)**2)
        if err < best_err:
            best_err = err
            best_f = f
    return best_f

def main():
    bench_path = "benchmark results.dat"
    if not os.path.isfile(bench_path):
        print(f"[ERROR] No se encontró '{bench_path}'. Ejecuta primero el benchmark.")
        return
    b = load_bench(bench_path)

    fig, axes = plt.subplots(1, 3, figsize=(18,5))
    threads = b["threads"]; sched = b["schedule"]

    # 1) Speedup vs Threads (+ Amdahl)
    ax = axes[0]
    for s in sorted(np.unique(sched)):
        sel = (sched == s)
        t_all = threads[sel]
        Sp_all = b["speedup"][sel]
        e_all = b["sigma_Sp"][sel]

        # Para cada cantidad de threads, elegir el mejor speedup (p.ej., sobre todos los chunks)
        t_unique = np.unique(t_all)
        t_plot = []
        Sp_plot = []
        e_plot = []
        for p in t_unique:
            m = (t_all == p)
            if not np.any(m): continue
            idx_best = np.argmax(Sp_all[m])
            Sp_best = Sp_all[m][idx_best]
            e_best = e_all[m][idx_best]
            t_plot.append(p); Sp_plot.append(Sp_best); e_plot.append(e_best)

        t_plot = np.array(t_plot); Sp_plot = np.array(Sp_plot); e_plot = np.array(e_plot)
        order = np.argsort(t_plot)
        t_plot, Sp_plot, e_plot = t_plot[order], Sp_plot[order], e_plot[order]

        # Puntos experimentales con barras de error
        ax.errorbar(t_plot, Sp_plot, yerr=e_plot, marker='o', lw=2, capsize=3,
                    label=f"{SCHEDULE_NAMES.get(s,str(s))} (exp)")

        # Ajuste de Amdahl y curva teórica
        f_est = fit_amdahl(t_plot, Sp_plot)
        p_dense = np.linspace(1, max(t_plot) if len(t_plot) else 1, 200)
        Sp_pred = amdahl_speedup(p_dense, f_est)
        ax.plot(p_dense, Sp_pred, '--', lw=2,
                label=f"{SCHEDULE_NAMES.get(s,str(s))} Amdahl (f≈{f_est:.3f})")
        print(f"[AMDahl] schedule={SCHEDULE_NAMES.get(s,s)}: f ≈ {f_est:.4f}")

    ax.set_xlabel("Threads"); ax.set_ylabel("Speedup"); ax.set_title("Speedup vs Threads (con Amdahl)")
    ax.grid(True, ls='--', alpha=0.3); ax.legend()

    # 2) Eficiencia vs Threads
    ax = axes[1]
    for s in sorted(np.unique(sched)):
        sel = (sched==s)
        t = threads[sel]; y = b["efficiency"][sel]; e = b["sigma_Ep"][sel]
        idx = np.argsort(t); t=t[idx]; y=y[idx]; e=e[idx]
        ax.errorbar(t, y, yerr=e, marker='o', lw=2, capsize=3, label=SCHEDULE_NAMES.get(s,str(s)))
    ax.set_xlabel("Threads"); ax.set_ylabel("Eficiencia"); ax.set_title("Eficiencia vs Threads")
    ax.grid(True, ls='--', alpha=0.3); ax.legend()

    # 3) Tiempo vs Chunk (por schedule, promediando sobre threads iguales)
    ax = axes[2]
    chunks = b["chunk"]; Tm = b["time_mean"]; Ts = b["time_std"]
    for s in sorted(np.unique(sched)):
        sel = (sched==s)
        ch = chunks[sel]; tm = Tm[sel]; ts = Ts[sel]
        # Promediar por chunk para dar una curva representativa
        uniq = sorted(np.unique(ch))
        x=[]; y=[]; e=[]
        for c in uniq:
            mask = (ch==c)
            x.append(c)
            y.append(np.mean(tm[mask]))
            e.append(np.sqrt(np.mean(ts[mask]**2)))
        x = np.array(x); y = np.array(y); e = np.array(e)
        idx = np.argsort(x); x=x[idx]; y=y[idx]; e=e[idx]
        ax.errorbar(x, y, yerr=e, marker='o', lw=2, capsize=3, label=SCHEDULE_NAMES.get(s,str(s)))
    ax.set_xlabel("Chunk"); ax.set_ylabel("Tiempo [s]"); ax.set_title("Tiempo vs Chunk")
    ax.grid(True, ls='--', alpha=0.3); ax.legend()

    plt.tight_layout()
    plt.savefig("performance plots.png", dpi=150)
    print("OK: performance plots.png generado.")

if __name__ == "__main__":
    main()