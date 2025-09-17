#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
from matplotlib.animation import FuncAnimation
import matplotlib.colors as colors

def main():
    print("=== VISUALIZACIÓN DE PROPAGACIÓN DE ONDAS 2D ===")
    
    # Verificar que el archivo existe
    if not os.path.exists('results.csv'):
        print("Error: No se encuentra 'results.csv'")
        print("Ejecuta primero: ./wave_serial")
        return
    
    # Cargar datos
    print("Cargando datos...")
    df = pd.read_csv('results.csv', index_col='Time_Step')
    num_nodes = len(df.columns)
    num_steps = len(df)
    
    print(f"Datos cargados: {num_steps} pasos de tiempo, {num_nodes} nodos")
    
    # Preguntar dimensiones de la grid 2D
    print("\n¿Cuáles son las dimensiones de tu grid 2D?")
    width = int(input("Ancho (columnas): "))
    height = int(input("Alto (filas): "))
    
    if width * height != num_nodes:
        print(f"Error: {width}x{height} = {width*height} nodos, pero el CSV tiene {num_nodes} nodos")
        print("Ajustando automáticamente...")
        # Intentar determinar dimensiones automáticamente
        height = int(np.sqrt(num_nodes))
        width = num_nodes // height
        print(f"Usando: {width}x{height} = {width*height} nodos")
    
    # Configurar estilo
    plt.style.use('default')
    plt.rcParams['figure.figsize'] = [12, 8]
    
    # ==============================================
    # VISUALIZACIÓN 2D: EVOLUCIÓN TEMPORAL
    # ==============================================
    print("\nGenerando visualización 2D...")
    
    # Seleccionar pasos de tiempo clave para visualizar
    time_steps_to_plot = [0, 10, 25, 50, 75, 100, 150, 200]
    time_steps_to_plot = [t for t in time_steps_to_plot if t < num_steps]
    
    # Crear subplots para cada paso de tiempo
    n_cols = 4
    n_rows = (len(time_steps_to_plot) + n_cols - 1) // n_cols
    fig, axes = plt.subplots(n_rows, n_cols, figsize=(16, 4 * n_rows))
    fig.suptitle(f'Propagación de Onda en Grid 2D {width}x{height}', fontsize=16, fontweight='bold')
    
    # Normalizar colores para todos los plots
    vmin = df.values.min()
    vmax = df.values.max()
    
    for idx, time_step in enumerate(time_steps_to_plot):
        if time_step < num_steps:
            # Obtener datos y reorganizar en grid 2D
            data = df.iloc[time_step].values.reshape((height, width))
            
            # Seleccionar subplot
            if n_rows == 1:
                ax = axes[idx % n_cols]
            else:
                ax = axes[idx // n_cols, idx % n_cols]
            
            # Crear heatmap 2D
            im = ax.imshow(data, cmap='viridis', origin='upper', 
                          vmin=vmin, vmax=vmax, aspect='equal')
            
            ax.set_title(f'Paso {time_step}')
            ax.set_xlabel('Columnas')
            ax.set_ylabel('Filas')
            
            # Agregar barra de color
            plt.colorbar(im, ax=ax, shrink=0.8)
    
    # Ocultar subplots vacíos
    for idx in range(len(time_steps_to_plot), n_rows * n_cols):
        if n_rows == 1:
            axes[idx].set_visible(False)
        else:
            axes[idx // n_cols, idx % n_cols].set_visible(False)
    
    plt.tight_layout()
    plt.savefig('grid2d_evolution.png', dpi=150, bbox_inches='tight')
    print("Gráfico 2D guardado como 'grid2d_evolution.png'")
    
    # ==============================================
    # ANIMACIÓN 2D
    # ==============================================
    print("\n¿Generar animación 2D? (puede tomar tiempo) [y/N]")
    respuesta = input().strip().lower()
    
    if respuesta in ['y', 'yes']:
        print("Generando animación 2D...")
        
        fig_anim, ax_anim = plt.subplots(figsize=(8, 8))
        
        # Primer frame
        data = df.iloc[0].values.reshape((height, width))
        im = ax_anim.imshow(data, cmap='viridis', origin='upper', 
                           vmin=vmin, vmax=vmax, aspect='equal')
        
        plt.colorbar(im, ax=ax_anim, label='Amplitud')
        ax_anim.set_title('Propagación de Onda 2D - Paso 0')
        ax_anim.set_xlabel('Columnas')
        ax_anim.set_ylabel('Filas')
        
        def update_2d(frame):
            data = df.iloc[frame].values.reshape((height, width))
            im.set_array(data)
            ax_anim.set_title(f'Propagación de Onda 2D - Paso {frame}')
            return [im]
        
        # Crear animación
        ani = FuncAnimation(fig_anim, update_2d, frames=min(200, num_steps), 
                           interval=50, blit=True)
        
        ani.save('grid2d_animation.gif', writer='pillow', fps=15)
        print("Animación 2D guardada como 'grid2d_animation.gif'")
        plt.close(fig_anim)
    
    # ==============================================
    # GRÁFICOS ADICIONALES (1D para comparación)
    # ==============================================
    fig2, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))
    
    # Gráfico 1: Evolución temporal del nodo central
    center_x = width // 2
    center_y = height // 2
    center_id = center_y * width + center_x
    center_amplitude = df[f'Node_{center_id}']
    
    ax1.plot(df.index, center_amplitude, 'r-', linewidth=2, label='Nodo Central')
    ax1.set_xlabel('Paso de Tiempo')
    ax1.set_ylabel('Amplitud')
    ax1.set_title(f'Evolución del Nodo Central ({center_x},{center_y})')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # Gráfico 2: Energía del sistema
    energy = (df ** 2).sum(axis=1)
    ax2.plot(df.index, energy, 'b-', linewidth=2)
    ax2.set_xlabel('Paso de Tiempo')
    ax2.set_ylabel('Energía Total')
    ax2.set_title('Energía del Sistema vs Tiempo')
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('grid2d_additional_plots.png', dpi=150, bbox_inches='tight')
    print("Gráficos adicionales guardados como 'grid2d_additional_plots.png'")
    
    # ==============================================
    # ESTADÍSTICAS
    # ==============================================
    print("\n=== ESTADÍSTICAS 2D ===")
    print(f"Dimensiones: {width}x{height}")
    print(f"Amplitud máxima: {df.values.max():.6f}")
    print(f"Amplitud mínima: {df.values.min():.6f}")
    print(f"Energía inicial: {energy.iloc[0]:.6f}")
    print(f"Energía final: {energy.iloc[-1]:.6f}")
    
    if energy.iloc[0] > 0:
        energy_loss = ((energy.iloc[0] - energy.iloc[-1]) / energy.iloc[0]) * 100
        print(f"Pérdida de energía: {energy_loss:.2f}%")
    
    # Mostrar información del nodo central
    print(f"\nNodo central ({center_x},{center_y}):")
    print(f"  Amplitud inicial: {center_amplitude.iloc[0]:.6f}")
    print(f"  Amplitud final: {center_amplitude.iloc[-1]:.6f}")
    
    plt.show()
    print("\nVisualización 2D completada!")

if __name__ == "__main__":
    main()