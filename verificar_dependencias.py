#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script de verificación de dependencias para CNC Plotter GUI
"""

import sys

def check_dependencies():
    print("=" * 50)
    print("  VERIFICACION DE DEPENDENCIAS - CNC Plotter")
    print("=" * 50)
    print()
    
    all_ok = True
    
    # Python version
    print(f"[1/5] Python version: {sys.version}")
    if sys.version_info < (3, 8):
        print("    ERROR: Se requiere Python 3.8 o superior")
        all_ok = False
    else:
        print("    OK: Version compatible")
    print()
    
    # tkinter
    try:
        import tkinter as tk
        print(f"[2/5] tkinter: OK (version {tk.TkVersion})")
    except ImportError as e:
        print(f"[2/5] tkinter: ERROR - {e}")
        print("    Solucion: tkinter viene con Python, reinstala Python")
        all_ok = False
    print()
    
    # pyserial
    try:
        import serial
        print(f"[3/5] pyserial: OK (version {serial.VERSION})")
    except ImportError as e:
        print(f"[3/5] pyserial: ERROR - {e}")
        print("    Solucion: pip install pyserial")
        all_ok = False
    print()
    
    # pillow
    try:
        from PIL import Image
        import PIL
        print(f"[4/5] pillow: OK (version {PIL.__version__})")
    except ImportError as e:
        print(f"[4/5] pillow: ERROR - {e}")
        print("    Solucion: pip install pillow")
        all_ok = False
    print()
    
    # numpy
    try:
        import numpy as np
        print(f"[5/5] numpy: OK (version {np.__version__})")
    except ImportError as e:
        print(f"[5/5] numpy: ERROR - {e}")
        print("    Solucion: pip install numpy")
        all_ok = False
    print()
    
    print("=" * 50)
    if all_ok:
        print("  TODAS LAS DEPENDENCIAS ESTAN INSTALADAS!")
        print("  Puedes ejecutar: python cnc_plotter_gui.py")
    else:
        print("  ALGUNAS DEPENDENCIAS FALTAN")
        print("  Ejecuta: pip install -r requirements.txt")
    print("=" * 50)
    
    return all_ok

if __name__ == "__main__":
    success = check_dependencies()
    sys.exit(0 if success else 1)
