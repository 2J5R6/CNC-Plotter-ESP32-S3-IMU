# CNC Controller - ESP32 S3

Sistema de control para CNC con 3 motores paso a paso 28BYJ-48 y sensor IMU MPU6050.

## 🔧 Hardware Requerido

### Componentes Principales
- **ESP32 S3** (controlador principal)
- **3x Motor 28BYJ-48** (motores paso a paso unipolares)
- **3x Driver ULN2003** (controladores de motor)
- **MPU6050** (IMU 6-axis para corrección de posición)
- **Fuente de alimentación 5V** (para motores)

### Conexiones

#### Motor X (Eje X)
| ULN2003 | ESP32 S3 |
|---------|----------|
| IN1     | GPIO 12  |
| IN2     | GPIO 13  |
| IN3     | GPIO 14  |
| IN4     | GPIO 15  |

#### Motor Y (Eje Y)
| ULN2003 | ESP32 S3 |
|---------|----------|
| IN1     | GPIO 16  |
| IN2     | GPIO 17  |
| IN3     | GPIO 18  |
| IN4     | GPIO 19  |

#### Motor Z (Lápiz)
| ULN2003 | ESP32 S3 |
|---------|----------|
| IN1     | GPIO 21  |
| IN2     | GPIO 47  |
| IN3     | GPIO 48  |
| IN4     | GPIO 45  |

#### IMU MPU6050
| MPU6050 | ESP32 S3 |
|---------|----------|
| VCC     | 3.3V     |
| GND     | GND      |
| SDA     | GPIO 8   |
| SCL     | GPIO 10  |

## 📚 Librerías Necesarias

Instala estas librerías desde el Library Manager de Arduino IDE:

1. **MPU6050** by Electronic Cats
   - Menú: Sketch → Include Library → Manage Libraries
   - Buscar: "MPU6050"
   - Instalar la versión de Electronic Cats

2. **Wire** (incluida con ESP32)
   - Ya viene preinstalada

## 🚀 Instalación

1. **Instalar ESP32 en Arduino IDE**
   ```
   File → Preferences → Additional Board Manager URLs
   Agregar: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   
   Tools → Board → Boards Manager
   Buscar: "ESP32"
   Instalar: "esp32 by Espressif Systems"
   ```

2. **Seleccionar la placa**
   ```
   Tools → Board → esp32 → ESP32S3 Dev Module
   ```

3. **Configurar parámetros de carga**
   ```
   Upload Speed: 115200
   USB CDC On Boot: Enabled
   USB Mode: Hardware CDC and JTAG
   ```

4. **Abrir el proyecto**
   - Abrir la carpeta `CNC_Controller`
   - Abrir `CNC_Controller.ino`

5. **Compilar y subir**
   - Conectar ESP32 S3 por USB
   - Presionar el botón de carga
   - Verificar en Monitor Serial (115200 baud)

## 📋 Comandos Disponibles

Envía estos comandos por Monitor Serial (115200 baud):

| Comando | Descripción |
|---------|-------------|
| `H` | **Home** - Regresar a posición origen (0, 0) |
| `U` | **Pen Up** - Levantar lápiz |
| `D` | **Pen Down** - Bajar lápiz |
| `S` | **Square** - Dibujar un cuadrado |
| `C` | **Circle** - Dibujar un círculo |
| `T` | **Triangle** - Dibujar un triángulo |
| `X` | **Test X** - Probar movimiento en eje X |
| `Y` | **Test Y** - Probar movimiento en eje Y |
| `Z` | **Test Z** - Probar movimiento en eje Z (lápiz) |
| `P` | **Position** - Mostrar posición actual |
| `I` | **IMU** - Mostrar datos del sensor IMU |

## ⚙️ Configuración Avanzada

### Ajustar Velocidad de Motores
Edita en el archivo `.ino`:
```cpp
#define STEP_DELAY 1200  // Microsegundos entre pasos
```
- Valores mayores = más lento pero más preciso
- Valores menores = más rápido pero menos torque

### Ajustar Límites de Movimiento
```cpp
#define MAX_X_STEPS 4096   // Máximo en X
#define MAX_Y_STEPS 4096   // Máximo en Y
#define MAX_Z_STEPS 512    // Máximo en Z
```

### Ajustar Altura del Lápiz
```cpp
#define PEN_UP_STEPS 200   // Pasos para levantar
#define PEN_DOWN_STEPS 0   // Pasos para bajar
```

## 🎯 Sprint 1 - Checklist

- [x] Control básico de 3 motores paso a paso
- [x] Límites de movimiento para evitar colisiones
- [x] Integración de IMU MPU6050
- [x] Control de lápiz (subir/bajar)
- [x] Funciones de prueba para cada eje
- [x] Dibujo de figuras básicas (cuadrado, círculo, triángulo)
- [x] Sistema de comandos por Serial

## 📊 Especificaciones Técnicas

### Motor 28BYJ-48
- Tipo: Paso a paso unipolar
- Pasos por revolución: 2048 (con reducción 64:1)
- Voltaje: 5V DC
- Consumo: ~240mA por motor
- Torque: ~300 gf·cm

### Secuencia de Pasos
- Modo: Half-Step (8 pasos por ciclo)
- Mayor precisión que full-step
- Movimiento más suave

## 🐛 Solución de Problemas

### Motor no se mueve
- Verificar conexiones de pines
- Verificar alimentación 5V a los drivers ULN2003
- Verificar que los LEDs del driver se encienden

### Motor pierde pasos
- Reducir velocidad (aumentar STEP_DELAY)
- Verificar que la carga mecánica no es excesiva
- Asegurar buena alimentación de corriente

### IMU no detectado
- Verificar conexiones I2C (SDA, SCL)
- Verificar alimentación 3.3V
- El sistema puede funcionar sin IMU (aparecerá advertencia)

### Movimientos imprecisos
- Calibrar límites MAX_X_STEPS, MAX_Y_STEPS
- Usar comando `H` (Home) para recalibrar
- Verificar que la mecánica no tenga juego

## 📝 Próximos Sprints

### Sprint 2: Parser G-code
- Implementar comandos G0, G1, G2, G3
- Soporte para coordenadas absolutas y relativas
- Conversión de mm a pasos

### Sprint 3: Interfaz Gráfica
- Aplicación web/desktop para dibujar
- Envío de trayectorias por WiFi
- Preview en tiempo real

## 👥 Equipo
Proyecto Final - Comunicaciones
Universidad Militar - Semestre VI
Noviembre 2025

## 📄 Licencia
Proyecto educativo - Uso libre para fines académicos
