#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
from matplotlib.animation import FuncAnimation
import matplotlib.colors as colors

# ========================
#   GRAFICAR LINEAL 1D
# ========================
def graficar_lineal():
    print("=== VISUALIZACIÓN DE PROPAGACIÓN DE ONDAS EN LÍNEA 1D ===")

    if not os.path.exists('results.csv'):
        print("Error: No se encuentra 'results.csv'")
        return
    
    # Cargar los datos
    df = pd.read_csv('results.csv', index_col='Time_Step')
    num_nodes = len(df.columns)
    num_steps = len(df)
    
    print(f"Datos cargados: {num_steps} pasos de tiempo, {num_nodes} nodos")
    
    # Configurar estilo de matplotlib
    plt.style.use('seaborn-v0_8')
    
    # Crear figura con subplots
    fig, axes = plt.subplots(2, 2, figsize=(15, 10))
    fig.suptitle('Análisis de Propagación de Ondas en Red Lineal', fontsize=16, fontweight='bold')
    
    # ==============================================
    # GRÁFICO 1: Evolución temporal por nodo
    # ==============================================
    ax1 = axes[0, 0]
    time_steps = df.index.values
    sample_nodes = [0, 1, 2, 3, 4]  # Primeros 5 nodos
    
    for node in sample_nodes:
        ax1.plot(time_steps, df[f'Node_{node}'], 
                label=f'Nodo {node}', linewidth=2, alpha=0.8)
    
    ax1.set_xlabel('Paso de Tiempo')
    ax1.set_ylabel('Amplitud')
    ax1.set_title('Evolución Temporal por Nodo')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # ==============================================
    # GRÁFICO 2: Perfil espacial en tiempos clave
    # ==============================================
    ax2 = axes[0, 1]
    x_nodes = np.arange(num_nodes)
    key_times = [0, 25, 50, 75, 100]  # Pasos clave a visualizar
    colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd']
    
    for i, time_step in enumerate(key_times):
        if time_step in df.index:
            amplitudes = df.loc[time_step].values
            ax2.plot(x_nodes, amplitudes, 'o-', 
                    color=colors[i], label=f'Paso {time_step}', 
                    markersize=4, linewidth=2, alpha=0.8)
    
    ax2.set_xlabel('Nodo')
    ax2.set_ylabel('Amplitud')
    ax2.set_title('Perfil Espacial en Diferentes Tiempos')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # ==============================================
    # GRÁFICO 3: Mapas de calor (Heatmap)
    # ==============================================
    ax3 = axes[1, 0]
    # Usar solo cada 5 pasos para que el heatmap no sea demasiado denso
    sampled_steps = df.index[::5]
    sampled_data = df.loc[sampled_steps].values
    
    im = ax3.imshow(sampled_data.T, aspect='auto', cmap='viridis',
                   extent=[0, num_steps, num_nodes-1, 0])
    
    ax3.set_xlabel('Paso de Tiempo')
    ax3.set_ylabel('Nodo')
    ax3.set_title('Mapa de Calor: Amplitud vs Tiempo vs Nodo')
    plt.colorbar(im, ax=ax3, label='Amplitud')
    
    # ==============================================
    # GRÁFICO 4: Energía del sistema
    # ==============================================
    ax4 = axes[1, 1]
    # Calcular energía total en cada paso de tiempo (suma de cuadrados)
    energy = (df ** 2).sum(axis=1)
    
    ax4.plot(time_steps, energy, color='purple', linewidth=2)
    ax4.set_xlabel('Paso de Tiempo')
    ax4.set_ylabel('Energía Total')
    ax4.set_title('Energía del Sistema vs Tiempo')
    ax4.grid(True, alpha=0.3)
    ax4.set_yscale('log')  # Escala logarítmica para ver mejor la decaída
    
    # ==============================================
    # Ajustar layout y guardar
    # ==============================================
    plt.tight_layout()
    plt.savefig('wave_analysis_comprehensive.png', dpi=150, bbox_inches='tight')
    print("Gráfico guardado como 'wave_analysis_comprehensive.png'")
    
    # ==============================================
    # Mostrar información estadística
    # ==============================================
    print("\n=== ESTADÍSTICAS ===")
    print(f"Amplitud máxima: {df.values.max():.6f}")
    print(f"Amplitud mínima: {df.values.min():.6f}")
    print(f"Energía inicial: {energy.iloc[0]:.6f}")
    print(f"Energía final: {energy.iloc[-1]:.6f}")
    print(f"Pérdida de energía: {((energy.iloc[0] - energy.iloc[-1])/energy.iloc[0]*100):.2f}%")
    
    # ==============================================
    # Animación simple (opcional)
    # ==============================================
    print("\n¿Quieres generar una animación? (puede tomar unos segundos) [y/N]")
    respuesta = input().strip().lower()
    
    if respuesta == 'y' or respuesta == 'yes':
        print("Generando animación...")
        
        fig_anim, ax_anim = plt.subplots(figsize=(10, 6))
        x_nodes = np.arange(num_nodes)
        
        line, = ax_anim.plot(x_nodes, df.iloc[0], 'o-', linewidth=2)
        ax_anim.set_xlim(0, num_nodes-1)
        ax_anim.set_ylim(df.values.min(), df.values.max() * 1.1)
        ax_anim.set_xlabel('Nodo')
        ax_anim.set_ylabel('Amplitud')
        ax_anim.set_title('Propagación de Onda en Tiempo Real')
        ax_anim.grid(True, alpha=0.3)
        
        def update(frame):
            line.set_ydata(df.iloc[frame])
            ax_anim.set_title(f'Propagación de Onda - Paso {frame}')
            return line,
        
        # Crear animación (solo primeros 100 pasos para no hacerla muy larga)
        ani = FuncAnimation(fig_anim, update, frames=min(100, num_steps), 
                           interval=50, blit=True)
        
        ani.save('wave_animation.gif', writer='pillow', fps=20)
        print("Animación guardada como 'wave_animation.gif'")
        plt.close(fig_anim)
    
    plt.show()
    print("\nVisualización completada!")

# ========================
#   GRAFICAR GRID 2D
# ========================
def graficar_2d():
    print("=== VISUALIZACIÓN DE PROPAGACIÓN DE ONDAS 2D ===")

    if not os.path.exists('results.csv'):
        print("Error: No se encuentra 'results.csv'")
        return
    
    df = pd.read_csv('results.csv', index_col='Time_Step')
    num_nodes = len(df.columns)
    num_steps = len(df)
    print(f"Datos cargados: {num_steps} pasos, {num_nodes} nodos")

    # Preguntar dimensiones
    width = int(input("Ancho (columnas): "))
    height = int(input("Alto (filas): "))

    if width * height != num_nodes:
        print("Dimensiones no coinciden, ajustando automáticamente...")
        height = int(np.sqrt(num_nodes))
        width = num_nodes // height
        print(f"Usando: {width}x{height}")

    # Subplots de evolución
    time_steps_to_plot = [0, 10, 25, 50, 100]
    time_steps_to_plot = [t for t in time_steps_to_plot if t < num_steps]
    fig, axes = plt.subplots(1, len(time_steps_to_plot), figsize=(16, 4))
    vmin, vmax = df.values.min(), df.values.max()

    for idx, time_step in enumerate(time_steps_to_plot):
        data = df.iloc[time_step].values.reshape((height, width))
        im = axes[idx].imshow(data, cmap='viridis', origin='upper',
                              vmin=vmin, vmax=vmax, aspect='equal')
        axes[idx].set_title(f'Paso {time_step}')
        plt.colorbar(im, ax=axes[idx], shrink=0.8)

    plt.tight_layout()
    plt.savefig('grid2d_evolution.png', dpi=150)
    plt.show()

    print("¡Visualización 2D completada!")


# ========================
#   MAIN
# ========================
def main():
    print("=== PROGRAMA DE VISUALIZACIÓN ===")
    print("1) Onda en línea 1D")
    print("2) Onda en grid 2D")
    opcion = input("Selecciona una opción [1/2]: ").strip()

    if opcion == "1":
        graficar_lineal()
    elif opcion == "2":
        graficar_2d()
    else:
        print("Opción no válida.")

if __name__ == "__main__":
    main()
