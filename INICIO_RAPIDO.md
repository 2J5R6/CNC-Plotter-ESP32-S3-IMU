# 🚀 INICIO RÁPIDO - CNC Controller

## ⚡ En 5 Minutos

### 1️⃣ Instalar Software (2 min)
```
1. Descargar Arduino IDE 2.x
2. Instalar soporte ESP32
3. Instalar librería MPU6050
```
Ver detalles en: [INSTALACION.md](INSTALACION.md)

---

### 2️⃣ Conectar Hardware (2 min)

#### Conexiones Mínimas:
```
ESP32 S3 → Motor X
  GPIO 12 → IN1
  GPIO 13 → IN2
  GPIO 14 → IN3
  GPIO 15 → IN4

ESP32 S3 → Motor Y
  GPIO 16 → IN1
  GPIO 17 → IN2
  GPIO 18 → IN3
  GPIO 19 → IN4

ESP32 S3 → Motor Z
  GPIO 21 → IN1
  GPIO 47 → IN2
  GPIO 48 → IN3
  GPIO 45 → IN4

ESP32 S3 → MPU6050
  GPIO 8  → SDA
  GPIO 10 → SCL
  3.3V    → VCC
  GND     → GND

⚠️ No olvidar: 5V a los ULN2003
⚠️ No olvidar: GND común a todo
```

Ver detalles en: [CONEXIONES.txt](CONEXIONES.txt)

---

### 3️⃣ Subir Código (1 min)

```
1. Abrir CNC_Controller.ino
2. Seleccionar Board: ESP32S3 Dev Module
3. Seleccionar Puerto COM
4. Click en Upload ➡️
```

---

## 🎮 Primeros Comandos

Abrir Monitor Serial (115200 baud) y probar:

```
H  ← Ir a HOME (0,0)
X  ← Probar motor X
Y  ← Probar motor Y
Z  ← Probar motor Z (lápiz)
S  ← Dibujar CUADRADO
C  ← Dibujar CÍRCULO
T  ← Dibujar TRIÁNGULO
P  ← Ver posición actual
I  ← Ver datos IMU
```

---

## ✅ Checklist Rápida

Antes de empezar:

- [ ] Arduino IDE instalado
- [ ] ESP32 soporte instalado
- [ ] Librería MPU6050 instalada
- [ ] ESP32 S3 conectado por USB
- [ ] 3x ULN2003 conectados
- [ ] 3x Motores 28BYJ-48 conectados
- [ ] MPU6050 conectado (opcional)
- [ ] Alimentación 5V conectada
- [ ] GND común conectado

---

## 🎯 Tu Primera Prueba

### Paso a Paso:

1. **Subir el código**
   ```
   Arduino IDE → Upload
   ```

2. **Abrir Monitor Serial**
   ```
   Tools → Serial Monitor (115200 baud)
   ```

3. **Ver mensaje de inicio**
   ```
   ==========================================
     CNC Controller ESP32 S3 - Iniciando
   ==========================================
   ```

4. **Probar motor X**
   ```
   Escribir: X
   Presionar: Enter
   ```
   ✅ El motor X debe girar ida y vuelta

5. **Dibujar cuadrado**
   ```
   Escribir: H  (ir a home)
   Escribir: S  (dibujar cuadrado)
   ```
   ✅ La CNC debe dibujar un cuadrado

---

## ❌ Problemas Comunes

### Motor no se mueve
```
✓ Verificar conexión pines
✓ Verificar alimentación 5V
✓ Verificar GND común
```

### ESP32 no se detecta
```
✓ Probar otro cable USB
✓ Instalar drivers CH340/CP2102
✓ Mantener BOOT presionado al subir
```

### IMU no funciona
```
✓ Verificar voltaje: 3.3V (no 5V)
✓ Verificar pines SDA/SCL
✓ El sistema funciona sin IMU (advertencia)
```

---

## 📚 Documentación Completa

| Archivo | Descripción |
|---------|-------------|
| [README.md](README.md) | Documentación principal |
| [INSTALACION.md](INSTALACION.md) | Guía de instalación completa |
| [CONEXIONES.txt](CONEXIONES.txt) | Diagrama de conexiones |
| [DIAGRAMA_VISUAL.txt](DIAGRAMA_VISUAL.txt) | Diagramas visuales detallados |
| [PRUEBAS_SPRINT1.md](PRUEBAS_SPRINT1.md) | Guía de pruebas y calibración |
| [config.h](config.h) | Archivo de configuración |

---

## 🎨 Ejemplos de Uso

### Dibujar un Cuadrado
```
Comando: H
Comando: S
Resultado: ⬜ Cuadrado de 800 pasos
```

### Dibujar un Círculo
```
Comando: H
Comando: C
Resultado: ⭕ Círculo de radio 400 pasos
```

### Dibujar un Triángulo
```
Comando: H
Comando: T
Resultado: 🔺 Triángulo equilátero
```

---

## 🔧 Personalización Rápida

### Cambiar velocidad
```cpp
// En config.h
#define STEP_DELAY 1200  // ← Cambiar este valor
// Mayor = más lento, más torque
// Menor = más rápido
```

### Cambiar tamaño de figuras
```cpp
// En config.h
#define DEFAULT_SQUARE_SIZE 800    // ← Cambiar
#define DEFAULT_CIRCLE_RADIUS 400  // ← Cambiar
```

---

## 🎓 Roadmap del Proyecto

### ✅ Sprint 1 (Actual)
- Control básico de motores
- Límites de movimiento
- Figuras básicas
- Integración IMU

### 🔜 Sprint 2
- Parser G-code completo
- Comandos G0/G1/G2/G3
- Conversión mm → pasos

### 🔜 Sprint 3
- Interfaz gráfica web
- Dibujo libre
- WiFi/Bluetooth
- Preview en tiempo real

---

## 💡 Tips

1. **Siempre hacer HOME antes de dibujar**
   ```
   Comando: H
   ```

2. **Verificar posición con P**
   ```
   Comando: P
   ```

3. **Ajustar velocidad si pierde pasos**
   ```
   Aumentar STEP_DELAY en config.h
   ```

4. **Calibrar altura del lápiz**
   ```
   Ajustar PEN_UP_STEPS y PEN_DOWN_STEPS
   ```

---

## 📞 Soporte

**Problemas frecuentes:** Ver [PRUEBAS_SPRINT1.md](PRUEBAS_SPRINT1.md)

**Dudas técnicas:** Revisar comentarios en código

**Proyecto:** Universidad Militar - Comunicaciones
**Semestre:** VI - Noviembre 2025

---

## 🎉 ¡Listo!

Si llegaste aquí y todo funciona, ¡felicitaciones! 

Ahora puedes:
- ✅ Experimentar con diferentes figuras
- ✅ Ajustar parámetros en config.h
- ✅ Prepararte para el Sprint 2

**Próximo paso:** Parser G-code para comandos avanzados 🚀

---

¿Necesitas ayuda? Revisa la documentación completa en los archivos .md
