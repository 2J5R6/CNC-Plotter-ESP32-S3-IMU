# 🤖 Auto-Detección Automática del Origen CNC

## 🎯 ¿Qué Problema Resuelve?

**Antes**: Tenías que adivinar dónde estaba el origen (0,0) del CNC  
**Ahora**: La GUI **APRENDE AUTOMÁTICAMENTE** dónde está el origen después de la calibración

---

## 🔍 ¿Cómo Funciona?

### Paso 1: Calibración Automática
```
1. Usuario hace clic en "🔍 AUTO-DETECTAR ORIGEN"
2. GUI envía comando 'C' (Calibrar eje X)
   - El CNC busca los límites en X
   - Se detiene cuando el IMU detecta resistencia
   - Vuelve al origen X=0
   
3. GUI envía comando 'D' (Calibrar eje Y)
   - El CNC busca los límites en Y
   - Se detiene cuando el IMU detecta resistencia
   - Vuelve al origen Y=0
   
4. GUI envía comando 'A' (Test 4 direcciones)
   - Verifica que se puede mover en todas las direcciones
```

### Paso 2: Pregunta al Usuario
```
Después de la calibración, la GUI pregunta:

┌─────────────────────────────────────┐
│   🔍 ¿Dónde está el CNC ahora?      │
│                                     │
│  ⬉ Superior Izq    Superior Der ⬈  │
│  ⬋ Inferior Izq    Inferior Der ⬊  │
└─────────────────────────────────────┘

El usuario mira físicamente el CNC y selecciona la esquina.
```

### Paso 3: Configuración Automática
```
La GUI ajusta automáticamente:
- ✅ Posición del origen en el canvas
- ✅ Dirección de los ejes X+ y Y+
- ✅ Fórmulas de conversión píxeles → mm
- ✅ Indicadores visuales (flechas de ejes)
```

---

## 🎨 Conversión Dinámica de Coordenadas

### Origen en SUPERIOR IZQUIERDA:
```python
x_mm = px / scale_factor           # X+ → derecha
y_mm = py / scale_factor           # Y+ → abajo
```

### Origen en SUPERIOR DERECHA:
```python
x_mm = (canvas_width - px) / scale_factor   # X+ → izquierda
y_mm = py / scale_factor                    # Y+ → abajo
```

### Origen en INFERIOR IZQUIERDA:
```python
x_mm = px / scale_factor                    # X+ → derecha
y_mm = (canvas_height - py) / scale_factor  # Y+ → arriba
```

### Origen en INFERIOR DERECHA:
```python
x_mm = (canvas_width - px) / scale_factor   # X+ → izquierda
y_mm = (canvas_height - py) / scale_factor  # Y+ → arriba
```

---

## 🚀 Cómo Usar

### 1️⃣ Conectar al CNC
```
1. Abrir la GUI: python cnc_plotter_gui.py
2. Seleccionar puerto COM
3. Clic en "🔌 Conectar"
```

### 2️⃣ Auto-Detectar Origen
```
1. Clic en "⚙️ Calibrar" (botón del panel superior)
2. En la ventana de calibración:
   - Clic en "🔍 AUTO-DETECTAR ORIGEN" (botón verde)
3. Confirmar: "Sí" para iniciar
4. Esperar ~40 segundos mientras calibra
```

### 3️⃣ Seleccionar Esquina
```
Cuando pregunte "¿Dónde está el CNC ahora?":
1. Mira físicamente tu CNC
2. Identifica en qué esquina quedó después de calibración
3. Haz clic en el botón correspondiente
```

### 4️⃣ ¡Listo para Dibujar!
```
✅ El canvas muestra el origen correcto
✅ Las flechas X+ y Y+ apuntan en la dirección correcta
✅ Puedes empezar a dibujar con el mouse
```

---

## 📊 Ejemplo Visual

### Escenario: Origen en INFERIOR DERECHA

#### Canvas (Pantalla):
```
    (0,0) -------------------- (600,0)
      |                            |
      |         PANTALLA           |
      |          (300,300)         |
      |                            |
      |                            ⬊ (600,600) ← ORIGEN CNC
    (0,600) -------------------- (600,600)
                                    ↑
                                  (0,0) CNC
```

#### CNC (Físico):
```
    (150,150) ------------ (0,150)
      |                        |
      | Y+ ↑                   |
      |         CNC            |
      |        (75,75)         |
      |                        |
      |                     ←──┤ X+
    (150,0) -------------- (0,0) ← ORIGEN
```

#### Conversión:
```
Dibujas en:              CNC va a:
─────────────────────────────────────
Canvas (600, 600)    →   CNC (0, 0)     ✅ Origen
Canvas (0, 0)        →   CNC (150, 150) ✅ Opuesto
Canvas (300, 300)    →   CNC (75, 75)   ✅ Centro
```

---

## 🔧 Ventajas del Sistema

### ✅ No más adivinanzas
- Ya no necesitas saber de antemano dónde está el origen
- El sistema lo detecta automáticamente

### ✅ Funciona con CUALQUIER configuración
- Origen en cualquier esquina
- Ejes en cualquier dirección
- Se adapta automáticamente

### ✅ Visual e intuitivo
- Canvas muestra exactamente dónde está el origen
- Flechas muestran dirección de movimiento
- Todo sincronizado con el hardware

### ✅ Una sola vez
- Calibras una vez al inicio
- La configuración se mantiene durante toda la sesión
- Si cambias algo físico, vuelves a calibrar

---

## 🧪 Flujo de Prueba

### Test Completo:

```bash
1. Conectar al CNC
   ✓ "✅ Conectado"

2. Clic en "⚙️ Calibrar"
   ✓ Ventana de calibración abierta

3. Clic en "🔍 AUTO-DETECTAR ORIGEN"
   ✓ Confirmación: "Sí"
   ✓ Console log: "🔍 INICIANDO AUTO-DETECCIÓN"
   ✓ "[1/4] Calibrando eje X..."
   ✓ "[2/4] Calibrando eje Y..."
   ✓ "[3/4] Test 4 direcciones..."
   ✓ "[4/4] Analizando..."

4. Aparece pregunta: "¿Dónde está el CNC?"
   ✓ Miras el CNC físicamente
   ✓ Seleccionas esquina correcta

5. Canvas actualizado
   ✓ Origen rojo en esquina correcta
   ✓ Flechas X+ y Y+ apuntando bien
   ✓ Mensaje: "✅ Origen detectado"

6. Dibujar algo simple
   ✓ Dibujas línea en canvas
   ✓ Clic "🎨 DIBUJAR EN CNC"
   ✓ CNC reproduce línea correctamente

7. Verificar
   ✓ Dibujo sale dentro del área
   ✓ Dibujo no se sale de límites
   ✓ Dibujo no está invertido
```

---

## 🐛 Solución de Problemas

### ❓ "El dibujo sale invertido"
**Causa**: Seleccionaste la esquina incorrecta  
**Solución**: Vuelve a hacer AUTO-DETECTAR y selecciona la esquina opuesta

### ❓ "El dibujo se sale del área"
**Causa**: Calibración incompleta o pasos/mm incorrectos  
**Solución**: 
1. Verifica que la calibración completó (mensaje "✅ AUTO-DETECCIÓN COMPLETADA")
2. Ajusta "Pasos/mm" según tu medición real

### ❓ "No aparece la pregunta de esquina"
**Causa**: Calibración no completó o error de comunicación  
**Solución**: 
1. Revisa consola serial - debe mostrar mensajes de calibración
2. Verifica que el CNC respondió correctamente
3. Intenta calibración manual (botones individuales)

### ❓ "Canvas muestra 'Calibra primero'"
**Causa**: No has ejecutado AUTO-DETECTAR aún  
**Solución**: Clic en "⚙️ Calibrar" → "🔍 AUTO-DETECTAR ORIGEN"

---

## 📝 Variables Internas

```python
# Estado de detección
self.origin_detected = False              # True después de calibración
self.origin_corner = "unknown"            # "top-left", "top-right", etc.
self.max_x_steps = 0                      # Pasos máximos en X (detectado)
self.max_y_steps = 0                      # Pasos máximos en Y (detectado)
self.calibration_in_progress = False      # True durante calibración
```

---

## 🎓 Conceptos Clave

### 1. Sistema de Referencia Relativo
- El canvas SIEMPRE tiene origen en (0,0) superior izquierda
- El CNC puede tener origen en CUALQUIER esquina
- La conversión se ajusta dinámicamente

### 2. Calibración Bidireccional
- El código Arduino busca límites en AMBAS direcciones
- Encuentra el punto más lejano y vuelve al origen
- Esto define automáticamente dónde está (0,0)

### 3. Transformación de Coordenadas
- Cada esquina tiene su propia fórmula de conversión
- X e Y pueden ser directos o invertidos
- Todo se calcula en `pixel_to_mm()`

---

## ✅ Resultado Final

**¡YA NO NECESITAS ADIVINAR!**

1. ✅ Calibras una vez
2. ✅ Seleccionas la esquina
3. ✅ La GUI se adapta automáticamente
4. ✅ Dibujas sin preocuparte de coordenadas
5. ✅ Todo funciona correctamente

---

**Universidad Militar - Proyecto CNC Plotter**  
**Sistema Inteligente de Detección de Origen**  
**Noviembre 2024** 🚀
