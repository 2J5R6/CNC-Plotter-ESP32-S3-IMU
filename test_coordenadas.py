#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Test de Coordenadas - Verificar que la inversion del eje Y funciona correctamente
"""

def test_coordinate_conversion():
    """Prueba la conversion de coordenadas canvas -> CNC"""
    
    # Configuracion (igual que en la GUI)
    canvas_width = 600
    canvas_height = 600
    work_area_width = 150  # mm
    work_area_height = 150  # mm
    scale_factor = canvas_width / work_area_width  # 4.0
    steps_per_mm = 51.2
    
    def pixel_to_mm(px, py):
        """Convertir pixeles a mm con origen en SUPERIOR DERECHA"""
        # X invertido: izquierda canvas = derecha CNC (origen)
        x_mm = (canvas_width - px) / scale_factor
        # Y directo: arriba canvas = arriba CNC
        y_mm = py / scale_factor
        return x_mm, y_mm
    
    def mm_to_steps(mm):
        """Convertir mm a pasos"""
        return int(mm * steps_per_mm)
    
    print("=" * 70)
    print("  TEST DE CONVERSION DE COORDENADAS")
    print("=" * 70)
    print()
    print(f"Canvas: {canvas_width}x{canvas_height} px")
    print(f"Area CNC: {work_area_width}x{work_area_height} mm")
    print(f"Escala: {scale_factor} px/mm")
    print(f"Resolucion: {steps_per_mm} pasos/mm")
    print()
    print("=" * 70)
    print()
    
    # Tests de las 4 esquinas
    test_points = [
        ("Esquina SUPERIOR IZQUIERDA (canvas)", 0, 0),
        ("Esquina SUPERIOR DERECHA (canvas) = ORIGEN CNC", 600, 0),
        ("Esquina INFERIOR IZQUIERDA (canvas)", 0, 600),
        ("Esquina INFERIOR DERECHA (canvas)", 600, 600),
        ("CENTRO", 300, 300),
        ("Punto arriba (Y=100)", 300, 100),
        ("Punto abajo (Y=500)", 300, 500),
    ]
    
    print("{:<40} | {:<15} | {:<15} | {:<20}".format(
        "Posicion", "Canvas (px)", "CNC (mm)", "CNC (pasos)"))
    print("-" * 70)
    
    for description, px, py in test_points:
        x_mm, y_mm = pixel_to_mm(px, py)
        x_steps = mm_to_steps(x_mm)
        y_steps = mm_to_steps(y_mm)
        
        print("{:<40} | ({:>3}, {:>3})    | ({:>6.1f}, {:>6.1f}) | ({:>5}, {:>5})".format(
            description, px, py, x_mm, y_mm, x_steps, y_steps))
    
    print()
    print("=" * 70)
    print("  VERIFICACION VISUAL")
    print("=" * 70)
    print()
    
    # Verificacion visual
    print("CANVAS (Pantalla):")
    print()
    print("    (0,0) -------- X+ -------- (600,0) <- ORIGEN CNC AQUI")
    print("      |                            |")
    print("      | Y+                         |")
    print("      |                            |")
    print("      |        PANTALLA            |")
    print("      |          (300,300)         |")
    print("      |                            |")
    print("      |                            |")
    print("    (0,600) ------------------ (600,600)")
    print()
    print("         ORIGEN CNC: (600,0) canvas = SUPERIOR DERECHA")
    print()
    print()
    print("CNC (Fisico):")
    print()
    print("    (150,0) -------------- (0,0)    <- ORIGEN CNC (Superior Derecha)")
    print("      |                        |")
    print("      |                     X+ |")
    print("      |                        |")
    print("      |         CNC            |")
    print("      |        (75,75)         |")
    print("      |                        |")
    print("      | Y+                     |")
    print("    (150,150) ------------ (0,150)")
    print()
    print("         ORIGEN CNC: (0,0) = SUPERIOR DERECHA fisica")
    print()
    print("=" * 70)
    print()
    
    # Tests de validacion
    print("TESTS DE VALIDACION:")
    print()
    
    # Test 1: Esquina superior derecha = origen CNC
    px, py = 600, 0  # Superior derecha en canvas
    x_mm, y_mm = pixel_to_mm(px, py)
    print(f"1. Esquina SUPERIOR DERECHA del canvas (origen CNC)")
    print(f"   Canvas: ({px}, {py})")
    print(f"   CNC: ({x_mm:.1f}, {y_mm:.1f}) mm")
    print(f"   Esperado: (0, 0) mm - ORIGEN CNC")
    print(f"   Resultado: {'OK' if x_mm == 0 and y_mm == 0 else 'ERROR'}")
    print()
    
    # Test 2: Izquierda del canvas = derecha del CNC
    px, py = 0, 100  # Izquierda en canvas
    x_mm, y_mm = pixel_to_mm(px, py)
    print(f"2. Dibujar IZQUIERDA en canvas")
    print(f"   Canvas: ({px}, {py})")
    print(f"   CNC: ({x_mm:.1f}, {y_mm:.1f}) mm")
    print(f"   Esperado: X cercano a {work_area_width}mm (lejos del origen)")
    print(f"   Resultado: {'OK' if x_mm > 100 else 'ERROR'}")
    print()
    
    # Test 3: Centro debe estar en el centro
    px, py = 300, 300  # Centro
    x_mm, y_mm = pixel_to_mm(px, py)
    print(f"3. Dibujar CENTRO en canvas (300,300)")
    print(f"   Canvas: ({px}, {py})")
    print(f"   CNC: ({x_mm:.1f}, {y_mm:.1f}) mm")
    print(f"   Esperado: Cerca de ({work_area_width/2}, {work_area_height/2}) mm")
    print(f"   Resultado: {'OK' if 70 < x_mm < 80 and 70 < y_mm < 80 else 'ERROR'}")
    print()
    
    print("=" * 70)
    print("  TEST COMPLETADO")
    print("=" * 70)
    print()
    print("Si todos los tests dicen 'OK', la conversion funciona correctamente!")
    print()

if __name__ == "__main__":
    test_coordinate_conversion()
