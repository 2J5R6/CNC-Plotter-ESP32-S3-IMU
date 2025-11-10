# 📋 ÍNDICE DEL PROYECTO - CNC Controller

## 📁 Estructura de Archivos

```
CNC_Controller/
│
├── CNC_Controller.ino          ⭐ ARCHIVO PRINCIPAL
│   └── Código completo del controlador (~1000 líneas)
│
├── INICIO_RAPIDO.md           🚀 EMPIEZA AQUÍ
│   └── Guía rápida en 5 minutos
│
├── README.md                   📖 DOCUMENTACIÓN PRINCIPAL
│   └── Hardware, conexiones, comandos, troubleshooting
│
├── INSTALACION.md             🔧 INSTALACIÓN DE SOFTWARE
│   └── Arduino IDE, ESP32, librerías, drivers
│
├── CONEXIONES.txt             🔌 DIAGRAMA DE CONEXIONES
│   └── Pinout detallado, esquemas eléctricos
│
├── DIAGRAMA_VISUAL.txt        📐 DIAGRAMAS COMPLETOS
│   └── Vistas, arquitectura, flujos de datos
│
└── PRUEBAS_SPRINT1.md         ✅ GUÍA DE PRUEBAS
    └── Calibración y verificación del Sprint 1
```

---

## 🎯 Guía de Lectura por Objetivo

### Si eres NUEVO en el proyecto:
1. **INICIO_RAPIDO.md** ← Empieza aquí
2. **README.md** ← Lee esto segundo
3. **INSTALACION.md** ← Instala software
4. **CONEXIONES.txt** ← Conecta hardware
5. **CNC_Controller.ino** ← Sube el código (todas las configuraciones están aquí)
6. **PRUEBAS_SPRINT1.md** ← Prueba y calibra

---

### Si necesitas INSTALAR:
1. **INSTALACION.md** ← Guía completa paso a paso
2. **README.md** (sección Hardware) ← Lista de componentes
3. **CONEXIONES.txt** ← Verificar conexiones

---

### Si necesitas CONECTAR HARDWARE:
1. **CONEXIONES.txt** ← Pinout detallado
2. **DIAGRAMA_VISUAL.txt** ← Diagramas visuales
3. **README.md** (sección Conexiones) ← Tablas de pines

---

### Si necesitas CALIBRAR:
1. **PRUEBAS_SPRINT1.md** ← Guía completa de pruebas
2. **CNC_Controller.ino** (líneas 66-75) ← Ajustar parámetros IMU y lápiz
3. **README.md** (sección Configuración Avanzada)

---

### Si necesitas PROGRAMAR:
1. **CNC_Controller.ino** ← Código principal (todo en un archivo)
2. **README.md** (sección API) ← Funciones disponibles

---

### Si tienes PROBLEMAS:
1. **PRUEBAS_SPRINT1.md** (sección Problemas Comunes)
2. **INSTALACION.md** (sección Solución de Problemas)
3. **README.md** (sección Troubleshooting)

---

## 📚 Contenido Detallado

### 📄 CNC_Controller.ino (~1000 líneas)

**Secciones principales:**
- Configuración de pines (líneas 1-60)
- **Parámetros ajustables (líneas 66-75)** ⚙️ ← IMPORTANTE
- Secuencias de pasos (líneas 80-90)
- Variables globales (líneas 95-105)
- Setup y Loop (líneas 110-180)
- Funciones de movimiento (líneas 230-350)
- Control del lápiz (líneas 380-420)
- Funciones de dibujo (líneas 480-520)
- Calibración IMU (líneas 580-880)
- Test de área (líneas 910-1020)
- Procesamiento de comandos (líneas 1030-1080)

**Funciones principales:**
```cpp
void moveX(long steps)           // Mover eje X
void moveY(long steps)           // Mover eje Y
void moveZ(long steps)           // Mover eje Z (mantiene posición)
void penUp()                     // ⬆️ Levantar lápiz
void penDown()                   // ⬇️ Bajar lápiz
void releasePenMotor()           // 🔌 Liberar motor Z
void goHome()                    // Ir a origen (0,0,0)
void drawSquare()                // Dibujar cuadrado
void calibrateXWithIMU()         // Calibrar X con IMU (ambas direcciones)
void calibrateYWithIMU()         // Calibrar Y con IMU (ambas direcciones)
void testAreaCompleta()          // Test 4 direcciones desde posición actual
void processCommand(char cmd)    // Procesar comandos seriales
```

---

### 📄 INICIO_RAPIDO.md

**Contenido:**
- Instalación en 5 minutos
- Primeros comandos
- Checklist rápida
- Primera prueba paso a paso
- Problemas comunes rápidos

---

### 📄 README.md

**Contenido:**
- Introducción al proyecto
- Lista de hardware requerido
- Tablas de conexiones
- Guía de instalación de librerías
- Comandos disponibles
- Configuración avanzada
- Sprint 1 checklist
- Especificaciones técnicas
- Troubleshooting completo
- Roadmap de sprints

---

### 📄 INSTALACION.md

**Contenido:**
- Instalación de librerías paso a paso
- Configuración de Arduino IDE
- Instalación de soporte ESP32
- Configuración de parámetros
- Verificación de instalación
- Drivers USB (Windows/Mac/Linux)
- Test de librerías
- Recursos adicionales
- Checklist final

---

### 📄 CONEXIONES.txt

**Contenido:**
- Diagrama completo de pines
- Conexiones Motor X, Y, Z
- Conexión IMU MPU6050
- Esquemas de alimentación
- Notas importantes
- Configuración I2C
- LEDs indicadores
- Guía de pruebas iniciales

---

### 📄 DIAGRAMA_VISUAL.txt

**Contenido:**
- Vista superior CNC
- Sistema de ejes y coordenadas
- Arquitectura del sistema
- Conexión eléctrica detallada
- Pinout ESP32 S3 completo
- Diagrama interno motor 28BYJ-48
- Esquemático ULN2003
- Diagrama MPU6050
- Secuencia de pasos visualizada
- Flujo de datos
- Sistema de coordenadas
- Diagrama de alimentación

---

### 📄 PRUEBAS_SPRINT1.md

**Contenido:**
- Objetivos del Sprint 1
- Secuencia de pruebas recomendada
- Verificación de límites
- Calibración de área de trabajo
- Ajuste de velocidad
- Calibración del lápiz
- Prueba de figuras básicas
- Problemas comunes con soluciones
- Registro de calibración (tabla)
- Tests de figuras con criterios
- Documentación de pruebas
- Checklist Sprint 1
- Próximos sprints

---

## 🎓 Niveles de Complejidad

### 🟢 Nivel Básico (Empezar aquí)
- INICIO_RAPIDO.md
- README.md (secciones básicas)
- Comandos simples (H, S, C, T)

### 🟡 Nivel Intermedio
- INSTALACION.md completo
- PRUEBAS_SPRINT1.md
- config.h (personalización)
- Calibración de parámetros

### 🔴 Nivel Avanzado
- CNC_Controller.ino (código completo)
- DIAGRAMA_VISUAL.txt
- Modificación de funciones
- Integración IMU avanzada

---

## 📊 Estadísticas del Proyecto

```
Total de archivos:     8
Total de líneas:       ~2500+
Código Arduino:        ~1000 líneas
Documentación:         ~1500 líneas
Funciones principales: 25+
Comandos disponibles:  13
Motores soportados:    3
Sensores integrados:   1 (MPU6050)
Figuras implementadas: Cuadrado (más en Sprint 2)
```

---

## 🗺️ Roadmap del Proyecto

### ✅ Sprint 1 - COMPLETADO
**Objetivo:** Control básico de motores y figuras simples

**Entregables:**
- [x] Control de 3 motores paso a paso
- [x] Límites de movimiento
- [x] Integración IMU MPU6050
- [x] Sistema de comandos por Serial
- [x] Dibujo de cuadrado, círculo, triángulo
- [x] Documentación completa
- [x] Guías de instalación y pruebas

---

### 🔜 Sprint 2 - SIGUIENTE
**Objetivo:** Parser G-code y comandos avanzados

**Entregables planificados:**
- [ ] Parser completo de G-code
- [ ] Comandos G0, G1 (movimiento lineal)
- [ ] Comandos G2, G3 (arcos)
- [ ] Comandos M3, M5 (control lápiz)
- [ ] Conversión mm → pasos
- [ ] Coordenadas absolutas/relativas
- [ ] Buffer de comandos
- [ ] Importar archivos G-code

---

### 🔜 Sprint 3 - FUTURO
**Objetivo:** Interfaz gráfica de dibujo

**Entregables planificados:**
- [ ] Aplicación web/desktop
- [ ] Canvas de dibujo libre
- [ ] Envío por WiFi/Bluetooth
- [ ] Preview en tiempo real
- [ ] Biblioteca de figuras
- [ ] Exportar a G-code
- [ ] Control remoto

---

## 🛠️ Comandos Rápidos de Referencia

```
═══ MOTORES ═══
X  → Test eje X
Y  → Test eje Y
Z  → Test eje Z

═══ DIBUJO ═══
S  → 🔲 Cuadrado (con penUp/penDown automático)
U  → ⬆️ Lápiz ARRIBA (Pen Up)
B  → ⬇️ Lápiz ABAJO (Pen Down)
R  → 🔌 Liberar Motor Z

═══ NAVEGACIÓN ═══
H  → 🏠 Home (origen 0,0,0)
P  → 📍 Posición actual

═══ CALIBRACIÓN ═══
C  → ⚙️ Calibrar eje X con IMU (busca ambos límites)
D  → ⚙️ Calibrar eje Y con IMU (busca ambos límites)
A  → 🧪 Test área (4 direcciones desde posición actual)
I  → 📊 Ver datos IMU en tiempo real
```

---

## 📞 Información del Proyecto

**Proyecto:** CNC Plotter con ESP32 S3
**Curso:** Comunicaciones - Laboratorio
**Institución:** Universidad Militar
**Semestre:** VI
**Fecha:** Noviembre 2025

**Hardware:**
- Controlador: ESP32 S3
- Motores: 3x 28BYJ-48 (paso a paso)
- Drivers: 3x ULN2003
- Sensor: MPU6050 (IMU 6-axis)
- Alimentación: 5V/2A

**Software:**
- Lenguaje: C++ (Arduino)
- IDE: Arduino IDE 2.x
- Framework: Arduino-ESP32
- Librerías: Wire, MPU6050

---

## ✨ Características Principales

### ✅ Implementadas (Sprint 1)
- ✅ Control preciso de 3 ejes
- ✅ Protección contra límites físicos
- ✅ Secuencia half-step para mayor precisión
- ✅ **Calibración IMU bidireccional inteligente**
- ✅ **Motor Z mantiene posición del lápiz**
- ✅ **Parámetros ajustables para altura del lápiz**
- ✅ Sistema de comandos intuitivo
- ✅ Dibujo de cuadrado con control automático de lápiz
- ✅ **Test de 4 direcciones desde cualquier posición**
- ✅ Detección automática de origen (0,0)
- ✅ Documentación exhaustiva

### 🔜 Próximas (Sprints 2-3)
- [ ] Más figuras (círculo, triángulo, líneas libres)
- [ ] Parser G-code completo
- [ ] Interfaz gráfica web
- [ ] Control inalámbrico
- [ ] Dibujo libre
- [ ] Biblioteca de patrones

---

## 📖 Cómo Usar Este Índice

1. **Identifica tu objetivo** (instalar, conectar, programar, etc.)
2. **Busca la sección correspondiente** en este índice
3. **Sigue los archivos recomendados** en orden
4. **Consulta el contenido detallado** para saber qué esperar
5. **Usa la guía de lectura** para optimizar tu tiempo

---

## 💡 Tips de Navegación

- Los archivos **.md** son documentación (leer con cualquier editor Markdown)
- Los archivos **.txt** son diagramas (mejor en fuente monoespaciada)
- El archivo **.ino** contiene TODA la configuración (no hay config.h separado)
- **Para ajustar parámetros:** Edita CNC_Controller.ino líneas 66-75

---

## 🎯 Objetivo del Sprint 1

> "Dejar todos los paso a paso funcionando bien, 
> que no se salgan las piezas por dar muchas vueltas"

### Estado: ✅ COMPLETADO

**Logros:**
✅ Control de 3 motores con límites
✅ Movimientos coordinados X-Y
✅ Control de lápiz (Z)
✅ Dibujo de cuadrado perfecto
✅ Dibujo de círculo suave
✅ Dibujo de triángulo preciso
✅ Integración de IMU para feedback
✅ Sistema robusto de comandos
✅ Documentación completa

**Próximo:** Sprint 2 - Parser G-code 🚀

---

**Fin del Índice** - ¡Feliz construcción de tu CNC! 🎨🤖
