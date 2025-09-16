#!/usr/bin/env python3

print("Verificando paquetes instalados...")
print("=" * 40)

try:
    import pandas
    print("✓ pandas está instalado")
except ImportError:
    print("✗ pandas NO está instalado")
    print("  Instalar con: sudo pacman -S python-pandas")

try:
    import matplotlib
    print("✓ matplotlib está instalado")
except ImportError:
    print("✗ matplotlib NO está instalado")
    print("  Instalar con: sudo pacman -S python-matplotlib")

try:
    import numpy
    print("✓ numpy está instalado")
except ImportError:
    print("✗ numpy NO está instalado")
    print("  Instalar con: sudo pacman -S python-numpy")

print("\nRecomendación: Usa la Opción 1:")
print("sudo pacman -S python-pandas python-matplotlib")