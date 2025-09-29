#!/usr/bin/env python3
import os
import argparse
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

"""
ejemplo para ejecutar el python
- python3 graficar_resultados.py --mode 1d --input "wave evolution.dat" --output wave_1d.gif
- python3 graficar_resultados.py --mode 2d --width 100 --height 100 --input "wave evolution.dat" --output wave_2d.gif
"""

def load_wave_evolution(path):
    """
    Lee 'wave evolution.dat' con formato:
      # Time_Step Node_0 Node_1 ...
      0 v0 v1 ...
      1 v0 v1 ...
    Devuelve:
      time: (T,) array
      amps: (T, N) matriz de amplitudes
    """
    if not os.path.isfile(path):
        raise FileNotFoundError(f"No se encontró el archivo: {path}")

    # Cargar datos ignorando líneas de comentario '#'
    data = np.loadtxt(path, comments="#")
    if data.ndim == 1:
        data = data.reshape(1, -1)
    time = data[:, 0]
    amps = data[:, 1:]
    return time, amps

def make_gif_1d(time, amps, out_path, fps=20, every=1, dpi=100):
    """
    Genera un GIF 1D (perfil espacial por paso de tiempo).
      time: (T,)
      amps: (T, N)
    """
    T, N = amps.shape
    # Submuestreo temporal si se desea
    idx = np.arange(0, T, max(1, every))
    time_s = time[idx]
    amps_s = amps[idx, :]

    x = np.arange(N)
    y_min = np.min(amps_s)
    y_max = np.max(amps_s)
    # Ampliar un poco los límites para que no “tope” con bordes
    pad = 0.05 * max(abs(y_min), abs(y_max), 1e-9)
    y_min -= pad
    y_max += pad

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
    """
    Genera un GIF 2D (heatmap por paso de tiempo).
      time: (T,)
      amps: (T, N) con N = width*height
    """
    T, N = amps.shape
    if width * height != N:
        raise ValueError(f"width*height ({width*height}) != N nodos ({N}).")

    idx = np.arange(0, T, max(1, every))
    time_s = time[idx]
    amps_s = amps[idx, :]

    # Límites de color fijos en todo el video
    vmin = np.min(amps_s)
    vmax = np.max(amps_s)
    if vmin == vmax:
        # Evita rango nulo
        vmin -= 1e-6
        vmax += 1e-6

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
    p = argparse.ArgumentParser(description="Genera GIFs de evolución temporal a partir de 'wave evolution.dat'")
    p.add_argument("--input", default="wave evolution.dat", help="Ruta al archivo de entrada (por defecto: 'wave evolution.dat')")
    p.add_argument("--mode", choices=["1d", "2d"], required=True, help="Tipo de red a graficar (1d o 2d)")
    p.add_argument("--width", type=int, default=None, help="Ancho (columnas) para 2D")
    p.add_argument("--height", type=int, default=None, help="Alto (filas) para 2D")
    p.add_argument("--output", default=None, help="Nombre del GIF de salida (si no se especifica, se infiere)")
    p.add_argument("--fps", type=int, default=20, help="Cuadros por segundo del GIF")
    p.add_argument("--every", type=int, default=1, help="Usar 1 de cada 'every' frames (submuestreo temporal)")
    p.add_argument("--dpi", type=int, default=100, help="DPI del lienzo para el GIF")
    return p.parse_args()

def main():
    args = parse_args()
    time, amps = load_wave_evolution(args.input)

    if args.mode == "1d":
        out = args.output if args.output else "wave_1d.gif"
        make_gif_1d(time, amps, out_path=out, fps=args.fps, every=args.every, dpi=args.dpi)
    else:
        if args.width is None or args.height is None:
            raise ValueError("Para modo 2d debes especificar --width y --height")
        out = args.output if args.output else "wave_2d.gif"
        make_gif_2d(time, amps, width=args.width, height=args.height,
                    out_path=out, fps=args.fps, every=args.every, dpi=args.dpi)

if __name__ == "__main__":
    main()