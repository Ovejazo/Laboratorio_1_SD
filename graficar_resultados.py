#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

def main():
    print("=== VISUALIZACIÓN RED ALEATORIA (ESCALA LOG) ===")
    
    if not os.path.exists('results.csv'):
        print("Error: No se encuentra 'results.csv'")
        return
    
    # Cargar datos
    df = pd.read_csv('results.csv')
    num_nodes = len(df.columns) - 1
    df.set_index('Time_Step', inplace=True)
    
    print(f"Nodos: {num_nodes}, Pasos: {len(df)}")
    
    # Reemplazar ceros con un valor muy pequeño para log scale
    df_log = df.replace(0, 1e-10)
    
    # ==============================================
    # HEATMAP CON ESCALA LOGARÍTMICA
    # ==============================================
    plt.figure(figsize=(15, 8))
    
    # Usar escala logarítmica
    from matplotlib.colors import LogNorm
    im = plt.imshow(df_log.T, aspect='auto', cmap='viridis', 
                   norm=LogNorm(vmin=1e-10, vmax=1.0),
                   extent=[0, len(df), num_nodes-1, 0])
    
    plt.xlabel('Paso de Tiempo')
    plt.ylabel('Nodo')
    plt.title('Propagación en Red Aleatoria (Escala Logarítmica)')
    plt.colorbar(im, label='Amplitud (log)')
    plt.savefig('random_network_log_heatmap.png', dpi=150, bbox_inches='tight')
    
    # ==============================================
    # GRÁFICO DE LÍNEAS CON ESCALA LOG
    # ==============================================
    plt.figure(figsize=(12, 6))
    
    # Seleccionar nodos interesantes (no cero)
    non_zero_nodes = []
    for col in df.columns:
        if df[col].max() > 1e-5:  # Nodos con amplitud significativa
            non_zero_nodes.append(col)
    
    # Tomar máximo 10 nodos para no saturar
    sample_nodes = non_zero_nodes[:10]
    
    for node in sample_nodes:
        plt.plot(df.index, df[node], label=node, linewidth=2, alpha=0.8)
    
    plt.yscale('log')  # ESCALA LOGARÍTMICA EN Y
    plt.xlabel('Paso de Tiempo')
    plt.ylabel('Amplitud (log)')
    plt.title('Evolución Temporal - Escala Logarítmica')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.savefig('random_network_log_evolution.png', dpi=150, bbox_inches='tight')
    
    # ==============================================
    # GRÁFICO DE DISTRIBUCIÓN DE AMPLITUDES
    # ==============================================
    plt.figure(figsize=(10, 6))
    
    # Amplitudes finales (último paso)
    final_amplitudes = df.iloc[-1].values
    final_amplitudes = final_amplitudes[final_amplitudes > 0]  # Filtrar ceros
    
    plt.hist(np.log10(final_amplitudes), bins=30, alpha=0.7, edgecolor='black')
    plt.xlabel('log10(Amplitud)')
    plt.ylabel('Frecuencia')
    plt.title('Distribución de Amplitudes Finales (log scale)')
    plt.grid(True, alpha=0.3)
    plt.savefig('random_network_amplitude_dist.png', dpi=150, bbox_inches='tight')
    
    # ==============================================
    # ESTADÍSTICAS DETALLADAS
    # ==============================================
    print("\n=== ESTADÍSTICAS DETALLADAS ===")
    print(f"Amplitud máxima: {df.values.max():.6f}")
    print(f"Amplitud mínima (no cero): {df[df > 0].min().min():.6e}")
    
    # Porcentaje de nodos no cero en el último paso
    non_zero_count = np.sum(df.iloc[-1] > 0)
    print(f"Nodos activos (amplitude > 0): {non_zero_count}/{num_nodes} ({non_zero_count/num_nodes*100:.1f}%)")
    
    # Energía
    energy = (df ** 2).sum(axis=1)
    print(f"Energía inicial: {energy.iloc[0]:.6e}")
    print(f"Energía final: {energy.iloc[-1]:.6e}")
    
    plt.show()
    print("Visualización completada!")

if __name__ == "__main__":
    main()