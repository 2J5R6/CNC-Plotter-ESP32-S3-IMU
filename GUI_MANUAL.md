# 🎨 Manual de la Interfaz Gráfica CNC Plotter

## 📋 Contenido
1. [Instalación](#instalación)
2. [Uso Básico](#uso-básico)
3. [Características Avanzadas](#características-avanzadas)
4. [Protocolo de Comunicación](#protocolo-de-comunicación)
5. [Calibración](#calibración)
6. [Solución de Problemas](#solución-de-problemas)

---

## 🚀 Instalación

### Requisitos Previos
- Python 3.8 o superior
- ESP32 S3 con código CNC_Controller.ino cargado
- Cable USB para conectar el ESP32

### Paso 1: Instalar Dependencias
```bash
cd "Proyecto Final"
pip install -r requirements.txt
```

### Paso 2: Verificar Instalación
```bash
python cnc_plotter_gui.py
```

---

## 🎮 Uso Básico

### 1️⃣ Conectar al CNC

1. **Conecta tu ESP32** al puerto USB
2. Abre la aplicación: `python cnc_plotter_gui.py`
3. En el panel superior:
   - Selecciona el puerto COM (ej: COM3, COM5)
   - Clic en **"🔄"** para actualizar la lista de puertos
   - Clic en **"🔌 Conectar"**
4. Espera el mensaje **"✅ Conectado"** en el indicador de estado

### 2️⃣ Dibujar con el Mouse

1. **Haz clic y arrastra** en el canvas (área gris oscura)
2. El trazo aparecerá en **color verde**
3. Puedes dibujar **múltiples líneas**
4. El canvas muestra:
   - Grid cada 50 píxeles
   - Origen (0,0) en la esquina superior izquierda
   - Posición del mouse en tiempo real

### 3️⃣ Enviar Dibujo al CNC

1. Clic en **"🎨 DIBUJAR EN CNC"**
2. El proceso:
   - El CNC irá a **Home (0,0)** automáticamente
   - Subirá el lápiz
   - Se moverá al inicio de cada línea
   - Bajará el lápiz y dibujará
   - Subirá el lápiz al terminar cada línea
3. Observa:
   - **Barra de progreso**: muestra % completado
   - **Contador**: "líneas dibujadas / total líneas"
   - **Consola serial**: comandos enviados y respuestas

### 4️⃣ Controles Durante el Dibujo

- **⏸️ PAUSAR**: Detiene temporalmente el dibujo
- **▶️ REANUDAR**: Continúa desde donde se pausó
- **🗑️ Limpiar Canvas**: Borra todo y permite empezar de nuevo

---

## ⚙️ Características Avanzadas

### 💾 Guardar y Cargar Dibujos

#### Guardar
1. Clic en **"💾 Guardar"**
2. Elige ubicación y nombre del archivo
3. Se guarda como **archivo JSON** con todas las coordenadas

#### Cargar
1. Clic en **"📂 Cargar"**
2. Selecciona un archivo `.json` previamente guardado
3. El dibujo se reconstruirá en el canvas

### ✏️ Control Manual del Lápiz

- **⬆️ Subir**: Levanta el lápiz (comando `U`)
- **⬇️ Bajar**: Baja el lápiz para dibujar (comando `B`)

Útil para:
- Verificar movimiento del motor Z
- Ajustar altura del lápiz
- Pruebas manuales

### 🏠 Home - Ir a Origen

Clic en **"🏠 Home"** para:
- Regresar el CNC a la posición (0, 0)
- Reiniciar coordenadas
- Preparar para nuevo dibujo

---

## 🔧 Configuración Avanzada

### Ajustar Pasos por Milímetro

Por defecto: **51.2 pasos/mm** (calculado para motores 28BYJ-48)

**Para calibrar:**

1. En el panel derecho, encuentra **"⚙️ Configuración"**
2. Campo **"Pasos/mm"**: ingresa el nuevo valor
3. Clic en **"✓"** para aplicar

**Cómo calcular:**
```
Pasos/mm = Pasos totales en calibración / Distancia real medida

Ejemplo:
- Calibración X dio 4096 pasos
- Mediste 80 mm de recorrido
- Pasos/mm = 4096 / 80 = 51.2
```

### Ajustar Área de Trabajo

Edita en `cnc_plotter_gui.py` líneas ~25-30:
```python
self.canvas_width = 600           # Tamaño canvas en píxeles
self.canvas_height = 600
self.work_area_width = 150        # Área CNC en mm (ajustar)
self.work_area_height = 150       # Área CNC en mm (ajustar)
```

---

## 📡 Protocolo de Comunicación

### Comandos Enviados por la GUI

| Comando | Descripción | Ejemplo |
|---------|-------------|---------|
| `H` | Home - Ir a (0,0) | `H` |
| `U` | Pen Up - Subir lápiz | `U` |
| `B` | Pen Down - Bajar lápiz | `B` |
| `X<n>` | Mover X n pasos | `X100` (derecha 100), `X-50` (izquierda 50) |
| `Y<n>` | Mover Y n pasos | `Y200` (abajo 200), `Y-100` (arriba 100) |
| `Z<n>` | Mover Z n pasos | `Z50` (bajar 50), `Z-25` (subir 25) |
| `C` | Calibrar eje X | `C` |
| `D` | Calibrar eje Y | `D` |
| `A` | Test 4 direcciones | `A` |
| `P` | Posición actual | `P` |

### Respuestas del ESP32

El ESP32 envía confirmaciones y datos que aparecen en la **Consola Serial** (panel inferior):

```
← Posición actual: X=1024, Y=512, Z=0
← Moviendo X: 100 pasos
← ✓ Movimiento completado
```

---

## 🎯 Calibración

### Asistente de Calibración

Clic en **"⚙️ Calibrar"** en el panel superior.

#### 1. Calibrar Eje X
- Clic en **"📐 Calibrar Eje X"**
- El CNC buscará el límite izquierdo (IMU detecta resistencia)
- Luego buscará el límite derecho
- Establecerá el origen en el extremo izquierdo
- **Mide la distancia recorrida** con una regla

#### 2. Calibrar Eje Y
- Clic en **"📐 Calibrar Eje Y"**
- Busca límite inferior y superior
- Establece origen en el extremo inferior
- **Mide la distancia recorrida**

#### 3. Test 4 Direcciones
- Clic en **"🧪 Test 4 Direcciones"**
- Verifica que se mueva en todas las direcciones
- Debe regresar a la posición inicial

#### 4. Ver Datos IMU
- Clic en **"📊 Ver Datos IMU"**
- Muestra lecturas en tiempo real del acelerómetro

#### 5. Calcular y Actualizar

```
Pasos/mm = Pasos totales / Distancia medida (mm)

Ejemplo eje X:
- Total de pasos en calibración: 4096
- Distancia medida: 80 mm
- Pasos/mm = 4096 / 80 = 51.2

Ejemplo eje Y:
- Total de pasos: 3584
- Distancia medida: 70 mm
- Pasos/mm = 3584 / 70 = 51.2
```

Ingresa el valor en **"Pasos/mm"** y clic en **"✓"**

---

## 🐛 Solución de Problemas

### ❌ No se detectan puertos COM

**Causa**: Driver USB no instalado o cable defectuoso

**Solución**:
1. Verifica que el ESP32 esté conectado
2. Instala driver CP210x o CH340 según tu ESP32
3. En Windows: Verifica en "Administrador de dispositivos"
4. Prueba con otro cable USB
5. Clic en **"🔄"** para actualizar lista

---

### ❌ "Error de Conexión: Access Denied"

**Causa**: Puerto COM en uso por otra aplicación

**Solución**:
1. Cierra Arduino IDE si está abierto
2. Cierra cualquier monitor serial
3. Reinicia la GUI
4. Si persiste, reinicia el ESP32

---

### ❌ El CNC no se mueve

**Causa**: Comandos no llegan o motores sin alimentación

**Solución**:
1. Verifica en la consola serial que aparezcan comandos (`→ Enviado: X100`)
2. Verifica que aparezcan respuestas del ESP32 (`← Moviendo X: 100 pasos`)
3. Revisa conexión de alimentación de los motores (5V)
4. Prueba comandos manuales:
   - Envía `H` desde la GUI
   - Envía `U` y `B` para probar motor Z
   - Envía `X100` para probar motor X

---

### ❌ El dibujo sale desproporcionado

**Causa**: Pasos/mm incorrectos o área de trabajo mal configurada

**Solución**:
1. **Re-calibra** usando el asistente de calibración
2. **Mide físicamente** el área de trabajo real
3. Actualiza `work_area_width` y `work_area_height` en el código
4. Recalcula **Pasos/mm** con mediciones reales
5. Dibuja un cuadrado simple para verificar proporciones

---

### ❌ El lápiz no sube/baja correctamente

**Causa**: Motor Z mal calibrado o PEN_UP_STEPS/PEN_DOWN_STEPS incorrectos

**Solución**:
1. En el código `.ino`, ajusta:
   ```cpp
   #define PEN_UP_STEPS 200      // Aumenta si necesita subir más
   #define PEN_DOWN_STEPS 200    // Aumenta si necesita bajar más
   ```
2. Prueba manual con botones **⬆️ Subir** y **⬇️ Bajar**
3. Usa comando `R` para liberar el motor si se sobrecalienta

---

### ❌ La GUI se congela durante el dibujo

**Causa**: No debería pasar (usa threading)

**Solución**:
1. Clic en **⏸️ PAUSAR**
2. Espera 5 segundos
3. Si no responde, cierra la ventana
4. Reinicia la aplicación
5. Verifica que el archivo `.ino` esté actualizado con la última versión

---

### ❌ Errores de "Serial write failed"

**Causa**: Conexión USB intermitente

**Solución**:
1. Desconecta y reconecta el ESP32
2. Usa un cable USB de datos (no solo carga)
3. Conecta a un puerto USB 2.0 (más estable que 3.0)
4. Evita HUBs USB sin alimentación

---

## 📊 Especificaciones Técnicas

### Hardware Soportado
- **Microcontrolador**: ESP32 S3
- **Motores**: 28BYJ-48 (unipolar 5V, 4096 pasos/rev con reductor)
- **Drivers**: ULN2003
- **IMU**: MPU6050 (I2C)

### Software
- **Lenguaje GUI**: Python 3.8+
- **Lenguaje CNC**: C++ (Arduino)
- **Framework GUI**: Tkinter
- **Comunicación**: Serial 115200 baud

### Resolución
- **Canvas**: 600x600 píxeles
- **Área física**: Configurable (default 150x150 mm)
- **Resolución motor**: ~0.088° por paso (half-step)
- **Precisión lineal**: ~0.02 mm por paso (con pasos/mm = 51.2)

---

## 🎓 Tips de Uso

### ✅ Mejores Prácticas

1. **Siempre calibra** antes de dibujar por primera vez
2. **Guarda tus dibujos** frecuentemente
3. **Empieza con formas simples** (círculos, cuadrados)
4. **Verifica la altura del lápiz** con trazos de prueba
5. **Limpia el área de trabajo** para evitar obstáculos

### 🎨 Tips de Dibujo

1. **Dibujos simples funcionan mejor** al inicio
2. **No dibujes muy rápido** con el mouse (el CNC no tendrá tiempo de seguir cada píxel)
3. **Usa trazos continuos** (menos levantamientos de lápiz = más rápido)
4. **Prueba con papel borrador** primero
5. **Ajusta la presión del lápiz** moviendo el motor Z

### ⚡ Optimización

- Para dibujos grandes: aumenta `time.sleep()` entre comandos
- Para dibujos detallados: reduce `time.sleep()` para más velocidad
- Línea 455 en `cnc_plotter_gui.py`: `time.sleep(abs(delta_x) * 0.002)`

---

## 📝 Registro de Cambios

### Versión 1.0 (Noviembre 2024)
- ✅ Interfaz gráfica completa con tema oscuro
- ✅ Dibujo con mouse (click y drag)
- ✅ Comunicación serial con ESP32
- ✅ Conversión automática píxeles → mm → pasos
- ✅ Control de lápiz (pen up/down)
- ✅ Progreso en tiempo real
- ✅ Guardar/Cargar dibujos (JSON)
- ✅ Asistente de calibración integrado
- ✅ Consola serial con mensajes color

### Mejoras Realizadas al Código .ino
- ✅ Soporte para comandos con parámetros (`X100`, `Y-50`)
- ✅ Buffer de comandos completos
- ✅ Parsing de números positivos y negativos
- ✅ Retrocompatibilidad con comandos simples

---

## 🆘 Soporte

### Recursos
- **README.md**: Documentación del hardware
- **INSTALACION.md**: Guía de instalación detallada
- **CONEXIONES.txt**: Diagrama de pines
- **GitHub**: [CNC-Plotter-ESP32-S3-IMU](https://github.com/2J5R6/CNC-Plotter-ESP32-S3-IMU)

### Contacto
- **Proyecto**: Universidad Militar - Comunicaciones
- **Repositorio**: 2J5R6/CNC-Plotter-ESP32-S3-IMU

---

## 🎉 ¡Feliz Dibujo con tu CNC!

Recuerda: la práctica hace al maestro. Experimenta con diferentes configuraciones y dibujos. 🚀
