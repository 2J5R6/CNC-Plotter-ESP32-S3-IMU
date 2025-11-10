# Guía de Instalación - Librerías Arduino

## 📦 Librerías Requeridas

### 1. MPU6050 by Electronic Cats

Esta librería es necesaria para comunicarse con el sensor IMU.

#### Instalación desde Library Manager:

1. Abrir Arduino IDE
2. Ir a `Sketch` → `Include Library` → `Manage Libraries...`
3. En el buscador, escribir: `MPU6050`
4. Buscar la librería **"MPU6050 by Electronic Cats"**
5. Click en `Install`
6. Esperar a que termine la instalación

#### Instalación Manual (alternativa):

Si no funciona desde Library Manager:

```bash
# Descargar desde GitHub
git clone https://github.com/ElectronicCats/mpu6050.git

# Copiar a carpeta de librerías
# Windows: C:\Users\[TU_USUARIO]\Documents\Arduino\libraries\
# Mac: ~/Documents/Arduino/libraries/
# Linux: ~/Arduino/libraries/
```

### 2. Wire Library

**Ya incluida** con el paquete ESP32. No requiere instalación adicional.

---

## 🔧 Instalación del Soporte ESP32

### Agregar URL del Board Manager:

1. Abrir Arduino IDE
2. Ir a `File` → `Preferences`
3. En "Additional Board Manager URLs", agregar:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Click en `OK`

### Instalar Board ESP32:

1. Ir a `Tools` → `Board` → `Boards Manager...`
2. Buscar: `ESP32`
3. Seleccionar **"esp32 by Espressif Systems"**
4. Click en `Install`
5. Esperar a que descargue e instale (puede tardar varios minutos)

### Seleccionar la Placa:

1. Ir a `Tools` → `Board` → `esp32`
2. Seleccionar: **`ESP32S3 Dev Module`**

### Configurar Parámetros:

```
Tools → Upload Speed: 115200
Tools → USB CDC On Boot: Enabled
Tools → CPU Frequency: 240MHz (WiFi/BT)
Tools → Flash Size: 4MB (32Mb)
Tools → Partition Scheme: Default 4MB with spiffs
Tools → PSRAM: Disabled
```

---

## 🔍 Verificación de Instalación

### Test de Compilación:

1. Abrir `CNC_Controller.ino`
2. Click en ✓ (Verify)
3. Debe compilar sin errores

Salida esperada:
```
Sketch uses XXXXX bytes (X%) of program storage space.
Global variables use XXXXX bytes (X%) of dynamic memory.
```

### Test de Carga:

1. Conectar ESP32 S3 por USB-C
2. Seleccionar puerto COM en `Tools` → `Port`
3. Click en → (Upload)
4. Abrir Monitor Serial: `Tools` → `Serial Monitor`
5. Configurar velocidad: **115200 baud**

Salida esperada:
```
==========================================
  CNC Controller ESP32 S3 - Iniciando
==========================================

Inicializando motores paso a paso...
✓ Motores inicializados

Inicializando IMU MPU6050...
✓ MPU6050 conectado correctamente
...
```

---

## ❌ Solución de Problemas

### Error: "MPU6050.h: No such file or directory"

**Solución:**
- Reinstalar librería MPU6050
- Verificar que está en la carpeta correcta de librerías
- Reiniciar Arduino IDE

### Error: "A fatal error occurred: Could not open port"

**Solución:**
1. Verificar que el cable USB funciona (probar con otro)
2. Instalar drivers CH340/CP2102 si es necesario
3. Mantener presionado el botón BOOT del ESP32 durante la carga
4. Probar con otro puerto USB

### Error: "Compilation error: espressif/arduino-esp32..."

**Solución:**
- Actualizar el core ESP32:
  - `Boards Manager` → Buscar "esp32" → Update
- Reinstalar el core si persiste

### Warning: "Board esp32:esp32:esp32s3 is unknown"

**Solución:**
- Reinstalar soporte ESP32 desde Board Manager
- Verificar que la URL del board manager es correcta

### Monitor Serial no muestra nada

**Solución:**
1. Verificar velocidad: debe ser **115200 baud**
2. Verificar que `USB CDC On Boot` está **Enabled**
3. Presionar botón RESET del ESP32
4. Cerrar y reabrir Monitor Serial

---

## 📱 Drivers USB (si son necesarios)

### Windows:

**Para CH340:**
- Descargar: https://sparks.gogo.co.nz/ch340.html
- Instalar y reiniciar PC

**Para CP2102:**
- Descargar: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
- Instalar y reiniciar PC

### Mac:

El ESP32 S3 generalmente funciona sin drivers adicionales en Mac.

Si hay problemas:
```bash
# Verificar dispositivo detectado
ls /dev/cu.*

# Debería aparecer algo como:
# /dev/cu.usbserial-XXXX
```

### Linux:

Agregar usuario al grupo dialout:
```bash
sudo usermod -a -G dialout $USER
# Cerrar sesión y volver a iniciar
```

---

## 🧪 Test de Librerías

### Crear sketch de prueba:

```cpp
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 10); // SDA=8, SCL=10
  
  mpu.initialize();
  
  if (mpu.testConnection()) {
    Serial.println("MPU6050 OK!");
  } else {
    Serial.println("MPU6050 FAIL!");
  }
}

void loop() {
  delay(1000);
}
```

---

## 📚 Recursos Adicionales

### Documentación ESP32:
- https://docs.espressif.com/projects/arduino-esp32/

### Datasheet 28BYJ-48:
- https://components101.com/motors/28byj-48-stepper-motor

### Datasheet MPU6050:
- https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/

### Foro Arduino ESP32:
- https://forum.arduino.cc/c/hardware/esp32/

---

## ✅ Checklist Final

Antes de continuar, verifica:

- [ ] Arduino IDE instalado (versión 1.8.19 o 2.x)
- [ ] Soporte ESP32 instalado
- [ ] Librería MPU6050 instalada
- [ ] Placa ESP32S3 Dev Module seleccionada
- [ ] Parámetros de carga configurados
- [ ] Sketch compila sin errores
- [ ] ESP32 detectado en puerto COM/tty
- [ ] Monitor Serial funciona a 115200 baud

Si todos los puntos están marcados, ¡estás listo para usar la CNC!
