#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
from matplotlib.animation import FuncAnimation

def main():
    print("=== VISUALIZACIÓN DE PROPAGACIÓN DE ONDAS EN LÍNEA 1D ===")
    
    if not os.path.exists('results.csv'):
        print("Error: No se encuentra 'results.csv'")
        print("Ejecuta primero tu simulador para generar el archivo.")
        return
    
    # Cargar datos
    print("Cargando datos...")
    df = pd.read_csv('results.csv', index_col='Time_Step')
    num_nodes = len(df.columns)
    num_steps = len(df)
    print(f"Datos cargados: {num_steps} pasos de tiempo, {num_nodes} nodos")
    
    # --- GRAFICAR EVOLUCIÓN DE TODOS LOS NODOS ---
    plt.figure(figsize=(14, 7))
    for node in range(num_nodes):
        plt.plot(df.index, df[f'Node_{node}'], label=f'Nodo {node}', linewidth=1)
    plt.xlabel('Paso de Tiempo')
    plt.ylabel('Amplitud')
    plt.title('Evolución Temporal de Todos los Nodos (Red Lineal)')
    plt.grid(True, alpha=0.3)
    if num_nodes <= 15:
        plt.legend()
    plt.tight_layout()
    plt.savefig('onda_lineal_todos_nodos.png', dpi=150)
    print("Gráfico de todos los nodos guardado como 'onda_lineal_todos_nodos.png'")
    plt.show()

    # --- ANIMACIÓN DE LA PROPAGACIÓN ---
    print("Generando animación de la propagación de la onda en la red lineal...")

    fig, ax = plt.subplots(figsize=(12, 5))
    line, = ax.plot([], [], 'b-o', lw=2)
    ax.set_xlim(0, num_nodes - 1)
    min_amp = df.values.min()
    max_amp = df.values.max()
    ax.set_ylim(min_amp, max_amp)
    ax.set_xlabel('ID Nodo')
    ax.set_ylabel('Amplitud')
    ax.set_title('Propagación de Onda en Red Lineal')

    def init():
        line.set_data([], [])
        return line,

    def update(frame):
        y = df.iloc[frame].values
        x = np.arange(num_nodes)
        line.set_data(x, y)
        ax.set_title(f'Propagación de Onda - Paso {frame}')
        return line,

    anim = FuncAnimation(fig, update, frames=num_steps, init_func=init, blit=True, interval=50)
    anim.save('onda_lineal_animacion.gif', writer='pillow', fps=20)
    print("Animación guardada como 'onda_lineal_animacion.gif'")

    plt.close(fig)
    print("¡Visualización y animación completadas!")

if __name__ == "__main__":
    main()