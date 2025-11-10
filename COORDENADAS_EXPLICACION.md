# 🔄 Sistema de Coordenadas - Canvas vs CNC

## 📐 Problema Resuelto

### ANTES (INCORRECTO):
```
Canvas (GUI):              CNC (Físico):
(0,0) ─────► X+           (0,0) ─────► X+
  │                         │
  │ Y+                      │ Y+
  ▼                         ▼

Dibujas arriba → CNC dibuja arriba ✅
Dibujas abajo  → CNC dibuja abajo  ✅
PERO: El área de trabajo se invierte!
```

### AHORA (CORRECTO):
```
Canvas (GUI):              CNC (Físico):
(0,0) ─────► X+           
  │                        ▲ Y+
  │ Y+                     │
  ▼                       (0,0) ─────► X+


Canvas Y=0 (arriba)    → CNC Y=150mm (arriba físico)
Canvas Y=600px (abajo) → CNC Y=0mm (origen físico)
```

---

## 🎯 Conversión Implementada

### Fórmula de Conversión:

```python
def pixel_to_mm(px, py):
    # X: directa (izquierda a derecha en ambos)
    x_mm = px / scale_factor
    
    # Y: INVERTIDA (arriba-abajo en canvas ≠ arriba-abajo en CNC)
    y_mm = (canvas_height - py) / scale_factor
    
    return x_mm, y_mm
```

### Ejemplos:

| Posición Mouse | Canvas (px) | Conversión | CNC (mm) | CNC (pasos) |
|----------------|-------------|------------|----------|-------------|
| **Arriba Izq** | (0, 0) | Y invertido | (0, 150) | (0, 7680) |
| **Arriba Der** | (600, 0) | Y invertido | (150, 150) | (7680, 7680) |
| **Abajo Izq** | (0, 600) | Y invertido | (0, 0) | (0, 0) |
| **Abajo Der** | (600, 600) | Y invertido | (150, 0) | (7680, 0) |
| **Centro** | (300, 300) | Y invertido | (75, 75) | (3840, 3840) |

*Asumiendo: canvas 600x600px, área CNC 150x150mm, pasos/mm=51.2*

---

## 🖼️ Representación Visual

### Canvas (Pantalla):
```
    0           300         600 (px)
    ├─────────────┼─────────────┤
0 ──┤ (0,0)                     │  ← Y=0 canvas (ARRIBA)
    │                           │
    │         📱 PANTALLA       │
300 │            ●              │  ← Centro (300,300)
    │                           │
    │                           │
600 ┤                    (600,600)  ← Y=600 canvas (ABAJO)
    └───────────────────────────┘
        X=0              X=600

Al dibujar aquí arriba (Y=0)...
```

### CNC (Físico):
```
    0           75          150 (mm)
    ├─────────────┼─────────────┤
150 ┤                           │  ← Y=150mm CNC (ARRIBA físico)
    │                           │
    │         🤖 CNC            │
75  │            ●              │  ← Centro (75,75)
    │                           │
    │                           │
0 ──┤ (0,0)                     │  ← Y=0mm CNC (ORIGEN)
    └───────────────────────────┘
        X=0              X=150

...el CNC dibuja aquí arriba (Y=150mm)
```

---

## 🔍 Verificación Paso a Paso

### Test 1: Línea Horizontal en la Parte Superior
```
1. Dibujas en canvas: (100, 50) → (200, 50)
   
2. Conversión:
   Punto 1: (100px, 50px)
            x_mm = 100/4 = 25mm
            y_mm = (600-50)/4 = 137.5mm
            → (25mm, 137.5mm)
   
   Punto 2: (200px, 50px)
            x_mm = 200/4 = 50mm
            y_mm = (600-50)/4 = 137.5mm
            → (50mm, 137.5mm)
   
3. CNC dibuja:
   Línea horizontal en Y=137.5mm (CERCA DE ARRIBA) ✅
```

### Test 2: Línea Vertical en el Centro
```
1. Dibujas en canvas: (300, 100) → (300, 500)
   
2. Conversión:
   Punto 1: (300px, 100px) → (75mm, 125mm)
   Punto 2: (300px, 500px) → (75mm, 25mm)
   
3. CNC dibuja:
   Línea vertical desde Y=125mm hasta Y=25mm
   (de arriba hacia abajo) ✅
```

### Test 3: Cuadrado
```
1. Dibujas cuadrado en canvas:
   (200,200) → (400,200) → (400,400) → (200,400) → (200,200)
   
2. Conversión:
   (200,200) → (50mm, 100mm)
   (400,200) → (100mm, 100mm)
   (400,400) → (100mm, 50mm)
   (200,400) → (50mm, 50mm)
   
3. CNC dibuja:
   Cuadrado centrado en (75mm, 75mm) ✅
   Tamaño: 50mm x 50mm
```

---

## 🎨 Indicadores Visuales en el Canvas

El canvas ahora muestra:

1. **Origen CNC** (punto rojo): Esquina INFERIOR izquierda
   - Texto: "(0,0) CNC"

2. **Eje X** (flecha verde): Apunta a la DERECHA
   - Texto: "X+"

3. **Eje Y** (flecha azul): Apunta HACIA ARRIBA
   - Texto: "Y+"

4. **Grid**: Cuadrícula cada 50 píxeles (12.5mm)

---

## ✅ Validación

### Para confirmar que funciona correctamente:

1. **Dibuja una línea en la parte SUPERIOR del canvas**
   - El CNC debe dibujar CERCA DE ARRIBA (Y alto)

2. **Dibuja una línea en la parte INFERIOR del canvas**
   - El CNC debe dibujar CERCA DEL ORIGEN (Y bajo)

3. **Dibuja una línea en el LADO IZQUIERDO del canvas**
   - El CNC debe dibujar CERCA DEL ORIGEN X (X=0)

4. **Dibuja una línea en el LADO DERECHO del canvas**
   - El CNC debe dibujar LEJOS DEL ORIGEN X (X alto)

---

## 🔧 Configuración Importante

```python
# En cnc_plotter_gui.py (líneas ~20-30)
self.canvas_width = 600          # Píxeles
self.canvas_height = 600         # Píxeles
self.work_area_width = 150       # mm (ajustar según tu CNC)
self.work_area_height = 150      # mm (ajustar según tu CNC)
self.scale_factor = 4.0          # píxeles por mm (600/150)
```

---

## 📊 Tabla de Conversión Rápida

| Descripción | Canvas (px) | CNC (mm) | CNC (pasos) |
|-------------|-------------|----------|-------------|
| Origen CNC | (0, 600) | (0, 0) | (0, 0) |
| Esquina superior izq | (0, 0) | (0, 150) | (0, 7680) |
| Esquina superior der | (600, 0) | (150, 150) | (7680, 7680) |
| Esquina inferior der | (600, 600) | (150, 0) | (7680, 0) |
| Centro | (300, 300) | (75, 75) | (3840, 3840) |

*Con pasos/mm = 51.2*

---

## 🎯 Resumen

**✅ PROBLEMA RESUELTO**

- Antes: Dibujos se salían del área de trabajo
- Causa: Eje Y no estaba invertido
- Solución: `y_mm = (canvas_height - py) / scale_factor`
- Resultado: Canvas y CNC ahora están sincronizados correctamente

**El origen (0,0) del CNC ahora corresponde a la esquina INFERIOR izquierda del canvas, tal como debe ser** 🎉

---

**Universidad Militar - Proyecto CNC Plotter**
**Noviembre 2024**
