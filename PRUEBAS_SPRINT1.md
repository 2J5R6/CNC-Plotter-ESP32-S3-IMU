# Guía de Pruebas y Calibración

## 🎯 Sprint 1 - Pruebas Básicas

### Objetivo
Asegurar que todos los motores funcionan correctamente y no se salgan las piezas por dar muchas vueltas.

---

## 📝 Secuencia de Pruebas Recomendada

### 1️⃣ Prueba Inicial (Sin Conexión Mecánica)

Antes de conectar los motores a la estructura mecánica:

1. **Conectar solo el Motor X**
   - Enviar comando: `X`
   - Observar: El motor debe girar suavemente
   - Verificar: LEDs del ULN2003 se encienden en secuencia

2. **Conectar solo el Motor Y**
   - Enviar comando: `Y`
   - Verificar movimiento suave

3. **Conectar solo el Motor Z**
   - Enviar comando: `Z`
   - Verificar movimiento de subida/bajada

✅ **Resultado esperado:** Cada motor gira sin vibraciones excesivas

---

### 2️⃣ Verificación de Límites

Una vez que los motores están conectados a la estructura:

```
Comando: P
```

Salida esperada:
```
╔══════════════════════════════╗
║  POSICIÓN ACTUAL             ║
╚══════════════════════════════╝
X: 0 / 4096
Y: 0 / 4096
Z: 0 / 200
Lápiz: ARRIBA ↑
```

#### Prueba de Límite X:

```
1. Enviar: X  (prueba básica +500/-500)
2. Observar que regresa a posición inicial
3. ✅ Si regresa = Motor X OK
```

#### Prueba de Límite Y:

```
1. Enviar: Y  (prueba básica +500/-500)
2. Observar que regresa a posición inicial
3. ✅ Si regresa = Motor Y OK
```

---

### 3️⃣ Calibración de Área de Trabajo

#### Encontrar los límites reales de tu CNC:

1. **Medir distancia física:**
   ```
   - Medir área útil X: ___ mm
   - Medir área útil Y: ___ mm
   ```

2. **Calcular pasos necesarios:**
   ```
   Motor 28BYJ-48: 2048 pasos = 1 revolución
   
   Si 1 revolución = 40mm de desplazamiento:
   Pasos por mm = 2048 / 40 = 51.2 pasos/mm
   
   Para área de 80mm x 80mm:
   MAX_X_STEPS = 80 * 51.2 = 4096 pasos
   MAX_Y_STEPS = 80 * 51.2 = 4096 pasos
   ```

3. **Ajustar en config.h:**
   ```cpp
   #define MAX_X_STEPS 4096  // Tu valor calculado
   #define MAX_Y_STEPS 4096  // Tu valor calculado
   ```

4. **Probar límites:**
   ```
   Enviar comandos de prueba en Monitor Serial
   ```

---

### 4️⃣ Ajuste de Velocidad

Si los motores pierden pasos o vibran demasiado:

1. **Abrir config.h**
2. **Modificar STEP_DELAY:**
   ```cpp
   // Valor actual (rápido)
   #define STEP_DELAY 1200
   
   // Si pierde pasos, aumentar a:
   #define STEP_DELAY 1500  // Más lento, más torque
   
   // Si es muy lento:
   #define STEP_DELAY 1000  // Más rápido
   ```

3. **Recompilar y probar**

---

### 5️⃣ Calibración del Lápiz (Eje Z)

#### Encontrar altura óptima:

1. **Posicionar papel en área de trabajo**

2. **Bajar lápiz:**
   ```
   Comando: D  (Pen Down)
   ```

3. **Verificar contacto:**
   - ¿El lápiz toca el papel?
   - ¿La presión es correcta?

4. **Ajustar altura en config.h:**
   ```cpp
   #define PEN_UP_STEPS 200    // Lápiz arriba (no toca)
   #define PEN_DOWN_STEPS 0    // Lápiz abajo (dibuja)
   
   // Si el lápiz no toca cuando está "abajo":
   #define PEN_DOWN_STEPS 50   // Aumentar para más contacto
   
   // Si presiona demasiado:
   #define PEN_DOWN_STEPS -20  // Reducir presión
   ```

---

### 6️⃣ Prueba de Figuras Básicas

#### Secuencia de prueba:

1. **HOME**
   ```
   Comando: H
   Resultado: CNC va a posición (0,0)
   ```

2. **Cuadrado pequeño**
   ```
   Comando: S
   Resultado: Dibuja un cuadrado
   ```
   
   ✅ Verificar:
   - Las líneas son rectas
   - Las esquinas se conectan
   - El cuadrado es cerrado

3. **Triángulo**
   ```
   Comando: T
   Resultado: Dibuja un triángulo
   ```
   
   ✅ Verificar:
   - Los lados son rectos
   - Las esquinas se conectan

4. **Círculo**
   ```
   Comando: C
   Resultado: Dibuja un círculo
   ```
   
   ✅ Verificar:
   - El círculo es redondo (no ovalado)
   - El inicio y fin se conectan
   - El movimiento es suave

---

## 🔧 Problemas Comunes y Soluciones

### Problema: Las líneas no son rectas

**Causa:** Juego mecánico o pérdida de pasos

**Solución:**
1. Aumentar STEP_DELAY a 1500-2000
2. Verificar tornillos de la estructura
3. Reducir fricción en ejes

---

### Problema: El círculo sale ovalado

**Causa:** Diferentes velocidades en X e Y

**Solución:**
1. Verificar que ambos motores son idénticos
2. Ajustar CIRCLE_SEGMENTS en config.h:
   ```cpp
   #define CIRCLE_SEGMENTS 48  // Más segmentos = más redondo
   ```

---

### Problema: El cuadrado no cierra bien

**Causa:** Acumulación de error de posición

**Solución:**
1. Hacer HOME antes de cada dibujo
2. Verificar que la mecánica no tiene juego
3. Usar IMU para corrección (Sprint 1 avanzado)

---

### Problema: El motor vibra pero no gira

**Causa:** Secuencia de pasos incorrecta o velocidad muy alta

**Solución:**
1. Verificar conexión de pines (IN1-IN4)
2. Aumentar STEP_DELAY
3. Verificar alimentación 5V adecuada

---

### Problema: El lápiz no levanta bien

**Causa:** PEN_UP_STEPS insuficiente

**Solución:**
```cpp
#define PEN_UP_STEPS 250  // Aumentar valor
```

---

## 📊 Registro de Calibración

Completa esta tabla con tus valores:

| Parámetro | Valor Inicial | Valor Ajustado | Notas |
|-----------|---------------|----------------|-------|
| MAX_X_STEPS | 4096 | _____ | |
| MAX_Y_STEPS | 4096 | _____ | |
| MAX_Z_STEPS | 512 | _____ | |
| STEP_DELAY | 1200 | _____ | |
| PEN_UP_STEPS | 200 | _____ | |
| PEN_DOWN_STEPS | 0 | _____ | |

---

## 🎨 Tests de Figuras

### Test 1: Cuadrado Simple
```
1. Comando: H (Home)
2. Comando: S (Square)
3. Resultado: ⬜ Cuadrado completo
```

**Criterios de éxito:**
- [ ] Las 4 líneas son rectas
- [ ] Las esquinas forman 90°
- [ ] El cuadrado cierra completamente
- [ ] No hay desplazamiento después del HOME

---

### Test 2: Círculo
```
1. Comando: H (Home)
2. Comando: C (Circle)
3. Resultado: ⭕ Círculo completo
```

**Criterios de éxito:**
- [ ] El círculo es redondo (no ovalado)
- [ ] El movimiento es fluido
- [ ] El inicio y fin conectan
- [ ] No hay escalones visibles

---

### Test 3: Triángulo
```
1. Comando: H (Home)
2. Comando: T (Triangle)
3. Resultado: 🔺 Triángulo completo
```

**Criterios de éxito:**
- [ ] Las 3 líneas son rectas
- [ ] Las esquinas se conectan
- [ ] El triángulo cierra completamente

---

### Test 4: Precisión de Repetición
```
1. Comando: S (dibujar cuadrado)
2. Comando: H (volver a home)
3. Comando: S (dibujar otro cuadrado)
4. Comparar: ¿Ambos cuadrados están en la misma posición?
```

**Criterio de éxito:**
- [ ] Ambos cuadrados se superponen perfectamente
- [ ] No hay desviación acumulada

---

## 📸 Documentación de Pruebas

Toma fotos de:

1. ✅ Primera figura exitosa (cuadrado)
2. ✅ Círculo completo
3. ✅ Prueba de repetibilidad (2 cuadrados superpuestos)
4. ✅ Vista del montaje de motores
5. ✅ Posición del MPU6050

---

## ✅ Checklist Sprint 1 Completado

- [ ] Motor X funciona correctamente
- [ ] Motor Y funciona correctamente
- [ ] Motor Z (lápiz) sube y baja correctamente
- [ ] Límites de movimiento configurados y probados
- [ ] No se salen las piezas con movimientos completos
- [ ] Cuadrado se dibuja correctamente
- [ ] Círculo se dibuja correctamente
- [ ] Triángulo se dibuja correctamente
- [ ] IMU MPU6050 detectado y funcionando
- [ ] Precisión de repetición aceptable (<2mm error)

---

## 🎯 Próximo Sprint

Una vez completado el Sprint 1, estarás listo para:

**Sprint 2:** Parser G-code y comandos avanzados
- Comandos G0/G1 (movimiento lineal)
- Comandos G2/G3 (arcos)
- Comandos M3/M5 (control lápiz)
- Coordenadas absolutas/relativas

**Sprint 3:** Interfaz gráfica
- Dibujo libre en aplicación
- Envío por WiFi/Bluetooth
- Preview en tiempo real

---

¡Felicitaciones por completar el Sprint 1! 🎉
