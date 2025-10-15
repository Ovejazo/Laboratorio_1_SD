#!/usr/bin/env python3
import os
import argparse
import glob
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

"""
Ejemplos:
- python3 graficar_resultados.py --mode 1d --input "datos/wave evolution.dat" --outdir datos
- python3 graficar_resultados.py --mode 2d --width 100 --height 100 --input "datos/wave evolution.dat" --outdir datos
"""

def load_wave_evolution(path):
    if not os.path.isfile(path):
        # Mostrar candidatos para ayudar
        candidates = []
        for pat in ["datos/*.dat", "*.dat"]:
            candidates.extend(glob.glob(pat))
        msg = f"No se encontró el archivo: {path}"
        if candidates:
            msg += "\nArchivos .dat disponibles en el directorio actual:\n  " + "\n  ".join(sorted(candidates))
        raise FileNotFoundError(msg)

    data = np.loadtxt(path, comments="#")
    if data.ndim == 1:
        data = data.reshape(1, -1)
    time = data[:, 0]
    amps = data[:, 1:]
    return time, amps

def make_gif_1d(time, amps, out_path, fps=20, every=1, dpi=100):
    T, N = amps.shape
    idx = np.arange(0, T, max(1, every))
    time_s = time[idx]
    amps_s = amps[idx, :]

    x = np.arange(N)
    y_min = np.min(amps_s); y_max = np.max(amps_s)
    pad = 0.05 * max(abs(y_min), abs(y_max), 1e-9)
    y_min -= pad; y_max += pad

    plt.style.use("default")
    fig, ax = plt.subplots(figsize=(10, 5), dpi=dpi)
    line, = ax.plot(x, amps_s[0], "o-", lw=2)
    ax.set_xlim(0, N - 1)
    ax.set_ylim(y_min, y_max)
    ax.set_xlabel("Nodo")
    ax.set_ylabel("Amplitud")
    ax.set_title(f"Evolución 1D — t = {time_s[0]:.0f}")
    ax.grid(True, alpha=0.3)

    def update(frame):
        y = amps_s[frame]
        line.set_ydata(y)
        ax.set_title(f"Evolución 1D — t = {time_s[frame]:.0f}")
        return (line,)

    ani = FuncAnimation(fig, update, frames=len(time_s), interval=1000.0 / fps, blit=True)
    ani.save(out_path, writer="pillow", fps=fps)
    plt.close(fig)
    print(f"[OK] GIF 1D guardado en: {out_path}")

def make_gif_2d(time, amps, width, height, out_path, fps=20, every=1, dpi=100, cmap="viridis"):
    T, N = amps.shape
    if width * height != N:
        raise ValueError(f"width*height ({width*height}) != N nodos ({N}).")

    idx = np.arange(0, T, max(1, every))
    time_s = time[idx]
    amps_s = amps[idx, :]

    vmin = np.min(amps_s); vmax = np.max(amps_s)
    if vmin == vmax:
        vmin -= 1e-6; vmax += 1e-6

    plt.style.use("default")
    fig, ax = plt.subplots(figsize=(6, 6), dpi=dpi)
    frame0 = amps_s[0].reshape(height, width)
    im = ax.imshow(frame0, cmap=cmap, origin="upper", vmin=vmin, vmax=vmax, aspect="equal")
    ax.set_title(f"Evolución 2D — t = {time_s[0]:.0f}")
    ax.set_xlabel("x (columna)")
    ax.set_ylabel("y (fila)")
    cbar = fig.colorbar(im, ax=ax, shrink=0.85)
    cbar.set_label("Amplitud")

    def update(frame):
        im.set_data(amps_s[frame].reshape(height, width))
        ax.set_title(f"Evolución 2D — t = {time_s[frame]:.0f}")
        return (im,)

    ani = FuncAnimation(fig, update, frames=len(time_s), interval=1000.0 / fps, blit=True)
    ani.save(out_path, writer="pillow", fps=fps)
    plt.close(fig)
    print(f"[OK] GIF 2D guardado en: {out_path}")

def parse_args():
    p = argparse.ArgumentParser(description="Genera GIFs de evolución temporal a partir de datos .dat")
    p.add_argument("--input", default=os.path.join("datos", "wave evolution.dat"),
                   help="Ruta al archivo de entrada (.dat). Por defecto: datos/wave evolution.dat")
    p.add_argument("--mode", choices=["1d", "2d"], required=True, help="Tipo de red a graficar (1d o 2d)")
    p.add_argument("--width", type=int, default=None, help="Ancho (columnas) para 2D")
    p.add_argument("--height", type=int, default=None, help="Alto (filas) para 2D")
    p.add_argument("--outdir", default="datos", help="Directorio donde guardar el GIF. Por defecto: datos")
    p.add_argument("--output", default=None, help="Nombre del GIF de salida (solo nombre, sin ruta)")
    p.add_argument("--fps", type=int, default=20, help="Cuadros por segundo del GIF")
    p.add_argument("--every", type=int, default=1, help="Usar 1 de cada 'every' frames")
    p.add_argument("--dpi", type=int, default=100, help="DPI del lienzo")
    return p.parse_args()

def main():
    args = parse_args()
    time, amps = load_wave_evolution(args.input)

    # Asegurar directorio de salida
    os.makedirs(args.outdir, exist_ok=True)

    if args.mode == "1d":
        base = args.output if args.output else "wave_1d.gif"
        out = os.path.join(args.outdir, os.path.basename(base))
        make_gif_1d(time, amps, out_path=out, fps=args.fps, every=args.every, dpi=args.dpi)
    else:
        if args.width is None or args.height is None:
            raise ValueError("Para modo 2d debes especificar --width y --height")
        base = args.output if args.output else "wave_2d.gif"
        out = os.path.join(args.outdir, os.path.basename(base))
        make_gif_2d(time, amps, width=args.width, height=args.height,
                    out_path=out, fps=args.fps, every=args.every, dpi=args.dpi)

if __name__ == "__main__":
    main()