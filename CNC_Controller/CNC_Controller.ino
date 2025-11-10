/*
 * CNC Controller ESP32 S3 - VERSION OPTIMIZADA
 * Control de 3 motores paso a paso con calibración IMU
 * Pines finales: Motor X (12-15), Motor Y (1,3,4,5), Motor Z (21,47,48,45)
 * IMU: MPU6050 en GPIO 8 (SDA), GPIO 10 (SCL)
 */

#include <Wire.h>
#include <MPU6050.h>

// === DECLARACIÓN DE FUNCIONES ===
void stepX(bool cw);
void stepY(bool cw);
void stepZ(bool cw);
void moveX(long steps);
void moveY(long steps);
void moveZ(long steps);
void penUp();
void penDown();
void releasePenMotor();
void testMotorX();
void testMotorY();
void testMotorZ();
void drawSquare();
void goHome();
void printPosition();
void calibrateXWithIMU();
void calibrateYWithIMU();
void testAreaCompleta();
void processCommand(char cmd);
bool isAtLimit(int16_t current, int16_t previous, int threshold);

// ============================================
// CONFIGURACIÓN DE PINES - ESP32 S3
// ============================================

// Motor X - Eje X (ULN2003)
#define MOTOR_X_IN1  12
#define MOTOR_X_IN2  13
#define MOTOR_X_IN3  14
#define MOTOR_X_IN4  15

// Motor Y - Eje Y (ULN2003) - GPIO OUTPUT SEGUROS
#define MOTOR_Y_IN1  1
#define MOTOR_Y_IN2  3
#define MOTOR_Y_IN3  4
#define MOTOR_Y_IN4  5

// Motor Z - Lápiz (ULN2003)
#define MOTOR_Z_IN1  21
#define MOTOR_Z_IN2  47
#define MOTOR_Z_IN3  48
#define MOTOR_Z_IN4  45

// IMU MPU6050 - Pines I2C
#define SDA_PIN  8
#define SCL_PIN  10

// ============================================
// CONSTANTES DEL SISTEMA
// ============================================

#define STEP_DELAY 2000  // Aumentado para más torque
#define MAX_X_STEPS 8192  // Se ajustará automáticamente con calibración IMU
#define MAX_Y_STEPS 8192  // Se ajustará automáticamente con calibración IMU
#define MAX_Z_STEPS 512

// Constantes para detección de límite con IMU
#define IMU_THRESHOLD 500     // AUMENTADO: Diferencia mínima para considerar movimiento (ajústalo si necesitas)
#define STABLE_COUNT 15       // AUMENTADO: Lecturas consecutivas sin cambio para confirmar límite
#define CALIBRATION_DELAY 0   // Sin delay entre pasos = MÁXIMA VELOCIDAD
#define IMU_CHECK_INTERVAL 30 // AUMENTADO: Verificar IMU cada 30 pasos (menos frecuente = más movimiento)
#define MIN_STEPS_BEFORE_CHECK 300  // AUMENTADO: Muchos pasos antes de empezar a verificar límite

// ============================================
// CONTROL DEL LÁPIZ (MOTOR Z) - ¡AJUSTA ESTOS VALORES!
// ============================================
#define PEN_UP_STEPS 200      // ⬆️ AJUSTA: Pasos para SUBIR el lápiz (aumenta si necesita subir más)
#define PEN_DOWN_STEPS 200    // ⬇️ AJUSTA: Pasos para BAJAR el lápiz (aumenta si necesita bajar más)
#define PEN_HOLD_ENABLED true // ✅ Mantener motor Z energizado para sostener posición

// ============================================
// SECUENCIAS DE PASOS - 28BYJ-48
// ============================================

const int halfStep[8][4] = {
  {1,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,1,1,0},
  {0,0,1,0}, {0,0,1,1}, {0,0,0,1}, {1,0,0,1}
};

// ============================================
// VARIABLES GLOBALES
// ============================================

MPU6050 mpu;

long currentX = 0;
long currentY = 0;
long currentZ = 0;
int stepIndexX = 0;
int stepIndexY = 0;
int stepIndexZ = 0;
bool penIsDown = false;

// ============================================
// SETUP
// ============================================

void setup() {
  delay(1000);
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\n=== CNC ESP32 S3 ===\n");
  Serial.flush();
  
  // Motor X
  Serial.print("Motor X...");
  Serial.flush();
  pinMode(MOTOR_X_IN1, OUTPUT);
  pinMode(MOTOR_X_IN2, OUTPUT);
  pinMode(MOTOR_X_IN3, OUTPUT);
  pinMode(MOTOR_X_IN4, OUTPUT);
  digitalWrite(MOTOR_X_IN1, LOW);
  digitalWrite(MOTOR_X_IN2, LOW);
  digitalWrite(MOTOR_X_IN3, LOW);
  digitalWrite(MOTOR_X_IN4, LOW);
  Serial.println(" OK");
  Serial.flush();
  delay(300);
  
  // Motor Y
  Serial.print("Motor Y...");
  Serial.flush();
  pinMode(MOTOR_Y_IN1, OUTPUT);
  pinMode(MOTOR_Y_IN2, OUTPUT);
  pinMode(MOTOR_Y_IN3, OUTPUT);
  pinMode(MOTOR_Y_IN4, OUTPUT);
  digitalWrite(MOTOR_Y_IN1, LOW);
  digitalWrite(MOTOR_Y_IN2, LOW);
  digitalWrite(MOTOR_Y_IN3, LOW);
  digitalWrite(MOTOR_Y_IN4, LOW);
  Serial.println(" OK");
  Serial.flush();
  delay(300);
  
  // Motor Z
  Serial.print("Motor Z...");
  Serial.flush();
  pinMode(MOTOR_Z_IN1, OUTPUT);
  pinMode(MOTOR_Z_IN2, OUTPUT);
  pinMode(MOTOR_Z_IN3, OUTPUT);
  pinMode(MOTOR_Z_IN4, OUTPUT);
  digitalWrite(MOTOR_Z_IN1, LOW);
  digitalWrite(MOTOR_Z_IN2, LOW);
  digitalWrite(MOTOR_Z_IN3, LOW);
  digitalWrite(MOTOR_Z_IN4, LOW);
  Serial.println(" OK");
  Serial.flush();
  delay(300);
  
  // Inicializar MPU6050
  Serial.print("IMU MPU6050...");
  Serial.flush();
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);
  
  mpu.initialize();
  delay(100);
  
  if (mpu.testConnection()) {
    Serial.println(" OK");
    Serial.println("  ✓ IMU conectado correctamente");
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);  // ±2g
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);   // ±250°/s
  } else {
    Serial.println(" FALLO");
    Serial.println("  ⚠ IMU no detectado - calibración manual");
  }
  Serial.flush();
  delay(300);
  
  Serial.println("\n=== LISTO ===");
  Serial.println("\n📋 COMANDOS DISPONIBLES:");
  Serial.println("  [TESTS]");
  Serial.println("    X = Test Motor X (4000 pasos)");
  Serial.println("    Y = Test Motor Y (4000 pasos)");
  Serial.println("    Z = Test Motor Z (50 pasos)");
  Serial.println();
  Serial.println("  [CALIBRACIÓN CON IMU]");
  Serial.println("    C = Calibrar límite X (detección automática)");
  Serial.println("    D = Calibrar límite Y (detección automática)");
  Serial.println("    A = Test área completa (4 esquinas)");
  Serial.println("    I = Ver datos IMU en tiempo real");
  Serial.println();
  Serial.println("  [DIBUJO]");
  Serial.println("    S = Cuadrado (3500 pasos)");
  Serial.println();
  Serial.println("  [NAVEGACIÓN]");
  Serial.println("    H = Home (volver a 0,0,0)");
  Serial.println("    P = Ver posición actual");
  Serial.println();
  Serial.println("💡 Escribe un comando y presiona Enter");
  Serial.println();
  Serial.flush();
}

// ============================================
// LOOP
// ============================================

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd != '\n' && cmd != '\r') {
      processCommand(cmd);
    }
  }
  delay(10);
}

// ============================================
// FUNCIONES MOTOR
// ============================================

void stepX(bool cw) {
  if (cw) {
    stepIndexX++;
    if (stepIndexX >= 8) stepIndexX = 0;
  } else {
    stepIndexX--;
    if (stepIndexX < 0) stepIndexX = 7;
  }
  digitalWrite(MOTOR_X_IN1, halfStep[stepIndexX][0]);
  digitalWrite(MOTOR_X_IN2, halfStep[stepIndexX][1]);
  digitalWrite(MOTOR_X_IN3, halfStep[stepIndexX][2]);
  digitalWrite(MOTOR_X_IN4, halfStep[stepIndexX][3]);
  delayMicroseconds(STEP_DELAY);
}

void stepY(bool cw) {
  if (cw) {
    stepIndexY++;
    if (stepIndexY >= 8) stepIndexY = 0;
  } else {
    stepIndexY--;
    if (stepIndexY < 0) stepIndexY = 7;
  }
  digitalWrite(MOTOR_Y_IN1, halfStep[stepIndexY][0]);
  digitalWrite(MOTOR_Y_IN2, halfStep[stepIndexY][1]);
  digitalWrite(MOTOR_Y_IN3, halfStep[stepIndexY][2]);
  digitalWrite(MOTOR_Y_IN4, halfStep[stepIndexY][3]);
  delayMicroseconds(STEP_DELAY);
}

void stepZ(bool cw) {
  if (cw) {
    stepIndexZ++;
    if (stepIndexZ >= 8) stepIndexZ = 0;
  } else {
    stepIndexZ--;
    if (stepIndexZ < 0) stepIndexZ = 7;
  }
  digitalWrite(MOTOR_Z_IN1, halfStep[stepIndexZ][0]);
  digitalWrite(MOTOR_Z_IN2, halfStep[stepIndexZ][1]);
  digitalWrite(MOTOR_Z_IN3, halfStep[stepIndexZ][2]);
  digitalWrite(MOTOR_Z_IN4, halfStep[stepIndexZ][3]);
  delayMicroseconds(STEP_DELAY);
}

// ============================================
// FUNCIONES DE MOVIMIENTO
// ============================================

void moveX(long steps) {
  if (steps == 0) return;
  bool cw = (steps > 0);
  steps = abs(steps);
  
  Serial.print("X ");
  Serial.print(cw ? "+" : "-");
  Serial.println(steps);
  
  for (long i = 0; i < steps; i++) {
    stepX(cw);
  }
  currentX += (cw ? steps : -steps);
  
  digitalWrite(MOTOR_X_IN1, LOW);
  digitalWrite(MOTOR_X_IN2, LOW);
  digitalWrite(MOTOR_X_IN3, LOW);
  digitalWrite(MOTOR_X_IN4, LOW);
}

void moveY(long steps) {
  if (steps == 0) return;
  bool cw = (steps > 0);
  steps = abs(steps);
  
  Serial.print("Y ");
  Serial.print(cw ? "+" : "-");
  Serial.println(steps);
  
  for (long i = 0; i < steps; i++) {
    stepY(cw);
  }
  currentY += (cw ? steps : -steps);
  
  digitalWrite(MOTOR_Y_IN1, LOW);
  digitalWrite(MOTOR_Y_IN2, LOW);
  digitalWrite(MOTOR_Y_IN3, LOW);
  digitalWrite(MOTOR_Y_IN4, LOW);
}

void moveZ(long steps) {
  if (steps == 0) return;
  bool cw = (steps > 0);
  steps = abs(steps);
  
  Serial.print("Z ");
  Serial.print(cw ? "⬆️" : "⬇️");
  Serial.print(" ");
  Serial.println(steps);
  
  for (long i = 0; i < steps; i++) {
    stepZ(cw);
  }
  currentZ += (cw ? steps : -steps);
  
  // IMPORTANTE: Si PEN_HOLD_ENABLED = true, NO apagar el motor
  // Esto mantiene el lápiz en posición
  if (!PEN_HOLD_ENABLED) {
    digitalWrite(MOTOR_Z_IN1, LOW);
    digitalWrite(MOTOR_Z_IN2, LOW);
    digitalWrite(MOTOR_Z_IN3, LOW);
    digitalWrite(MOTOR_Z_IN4, LOW);
  }
  // Si PEN_HOLD_ENABLED = true, el motor queda energizado en la última posición
}

void moveXY(long stepsX, long stepsY) {
  // Movimiento diagonal coordinado
  bool cwX = (stepsX > 0);
  bool cwY = (stepsY > 0);
  
  long absX = abs(stepsX);
  long absY = abs(stepsY);
  
  long maxSteps = max(absX, absY);
  
  Serial.print("Movimiento XY: (");
  Serial.print(stepsX);
  Serial.print(", ");
  Serial.print(stepsY);
  Serial.println(")");
  
  for (long i = 0; i < maxSteps; i++) {
    if (i < absX && currentX >= 0 && currentX <= MAX_X_STEPS) {
      stepX(cwX);
      currentX += (cwX ? 1 : -1);
    }
    
    if (i < absY && currentY >= 0 && currentY <= MAX_Y_STEPS) {
      stepY(cwY);
      currentY += (cwY ? 1 : -1);
    }
  }
  
  // Apagar motores
  digitalWrite(MOTOR_X_IN1, LOW);
  digitalWrite(MOTOR_X_IN2, LOW);
  digitalWrite(MOTOR_X_IN3, LOW);
  digitalWrite(MOTOR_X_IN4, LOW);
  digitalWrite(MOTOR_Y_IN1, LOW);
  digitalWrite(MOTOR_Y_IN2, LOW);
  digitalWrite(MOTOR_Y_IN3, LOW);
  digitalWrite(MOTOR_Y_IN4, LOW);
}

// ============================================
// CONTROL DEL LÁPIZ (PEN UP / PEN DOWN)
// ============================================

void penUp() {
  if (penIsDown) {
    Serial.println("✏️ ⬆️ LÁPIZ ARRIBA (Pen Up)");
    moveZ(PEN_UP_STEPS);  // Subir lápiz
    penIsDown = false;
    delay(200);  // Pequeña pausa para estabilizar
  } else {
    Serial.println("ℹ️ Lápiz ya está arriba");
  }
}

void penDown() {
  if (!penIsDown) {
    Serial.println("✏️ ⬇️ LÁPIZ ABAJO (Pen Down)");
    moveZ(-PEN_DOWN_STEPS);  // Bajar lápiz
    penIsDown = true;
    delay(200);  // Pequeña pausa para estabilizar
  } else {
    Serial.println("ℹ️ Lápiz ya está abajo");
  }
}

void releasePenMotor() {
  // Liberar motor Z (apagar bobinas) - usar si el motor se calienta mucho
  Serial.println("🔌 Motor Z liberado");
  digitalWrite(MOTOR_Z_IN1, LOW);
  digitalWrite(MOTOR_Z_IN2, LOW);
  digitalWrite(MOTOR_Z_IN3, LOW);
  digitalWrite(MOTOR_Z_IN4, LOW);
}

// ============================================
// FUNCIONES TEST
// ============================================

void testMotorX() {
  Serial.println("\n=== TEST X - RECORRIDO COMPLETO ===");
  Serial.println("Posicion inicial: ");
  Serial.print("X="); Serial.println(currentX);
  
  Serial.println("→→→ Avanzando AL MÁXIMO (4000 pasos = ~17cm)...");
  moveX(4000);
  delay(2000);
  
  Serial.println("←←← Retrocediendo AL INICIO (4000 pasos)...");
  moveX(-4000);
  delay(2000);
  
  Serial.println("✓ Test X completado - ¿Llegó al final?");
  Serial.print("Posicion final X="); Serial.println(currentX);
  Serial.println();
}

void testMotorY() {
  Serial.println("\n=== TEST Y - RECORRIDO COMPLETO ===");
  Serial.println("Posicion inicial: ");
  Serial.print("Y="); Serial.println(currentY);
  
  Serial.println("↑↑↑ Avanzando AL MÁXIMO (4000 pasos = ~17cm)...");
  moveY(4000);
  delay(2000);
  
  Serial.println("↓↓↓ Retrocediendo AL INICIO (4000 pasos)...");
  moveY(-4000);
  delay(2000);
  
  Serial.println("✓ Test Y completado - ¿Llegó al final?");
  Serial.print("Posicion final Y="); Serial.println(currentY);
  Serial.println();
}

void testMotorZ() {
  Serial.println("\n=== TEST Z (50 pasos) ===");
  Serial.println("Posicion inicial: ");
  Serial.print("Z="); Serial.println(currentZ);
  
  Serial.println("↓ Bajando 50...");
  moveZ(50);
  delay(1000);
  
  Serial.println("↑ Subiendo 50...");
  moveZ(-50);
  delay(1000);
  
  Serial.println("✓ Test Z completado");
  Serial.print("Posicion final Z="); Serial.println(currentZ);
  Serial.println();
}

void drawSquare() {
  Serial.println("\n═══════════════════════════════════════");
  Serial.println("   🔲 DIBUJANDO CUADRADO GRANDE 🔲");
  Serial.println("   (3500 pasos por lado = ~15cm)");
  Serial.println("═══════════════════════════════════════\n");
  
  penDown(); // ✏️ BAJAR LÁPIZ para empezar a dibujar
  delay(500);
  
  Serial.println("→→→ Lado 1 (derecha)");
  moveX(3500);
  delay(500);
  
  Serial.println("↑↑↑ Lado 2 (arriba)");
  moveY(3500);
  delay(500);
  
  Serial.println("←←← Lado 3 (izquierda)");
  moveX(-3500);
  delay(500);
  
  Serial.println("↓↓↓ Lado 4 (abajo)");
  moveY(-3500);
  delay(500);
  
  penUp(); // ✏️ SUBIR LÁPIZ al terminar
  Serial.println("\n✅ ¡Cuadrado completado!\n");
}

void goHome() {
  Serial.println("\n=== HOME ===");
  if (currentX != 0) moveX(-currentX);
  if (currentY != 0) moveY(-currentY);
  if (currentZ != 0) moveZ(-currentZ);
  Serial.println("HOME OK\n");
}

void printPosition() {
  Serial.println("\n=== POSICION ===");
  Serial.print("X: "); Serial.print(currentX); 
  Serial.print(" / "); Serial.println(MAX_X_STEPS);
  Serial.print("Y: "); Serial.print(currentY); 
  Serial.print(" / "); Serial.println(MAX_Y_STEPS);
  Serial.print("Z: "); Serial.print(currentZ); 
  Serial.print(" / "); Serial.println(MAX_Z_STEPS);
  Serial.println();
}

// ============================================
// FUNCIONES IMU - DETECCIÓN DE LÍMITES
// ============================================

bool isAtLimit(int16_t current, int16_t previous, int threshold) {
  // Retorna true si la diferencia entre lecturas es menor al umbral
  // Esto indica que el motor ya no se está moviendo
  int16_t diff = abs(current - previous);
  return (diff < threshold);
}

void printIMUData() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  
  Serial.println("\n=== DATOS IMU ===");
  Serial.print("Acelerómetro: X="); Serial.print(ax);
  Serial.print(" Y="); Serial.print(ay);
  Serial.print(" Z="); Serial.println(az);
  Serial.print("Giroscopio:   X="); Serial.print(gx);
  Serial.print(" Y="); Serial.print(gy);
  Serial.print(" Z="); Serial.println(gz);
  Serial.println();
}

void calibrateXWithIMU() {
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║  CALIBRACION COMPLETA EJE X CON IMU       ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  Serial.println("🔍 Buscando ORIGEN y LÍMITES en ambas direcciones...\n");
  
  if (!mpu.testConnection()) {
    Serial.println("⚠ ERROR: IMU no disponible");
    Serial.println("Verifica conexión del MPU6050");
    return;
  }
  
  delay(500);
  
  // ============================================
  // FASE 1: Buscar límite IZQUIERDO (negativo)
  // ============================================
  Serial.println("═══ FASE 1: Buscando límite IZQUIERDO ═══");
  Serial.println("Moviendo hacia la IZQUIERDA...\n");
  
  int16_t ax, ay, az, gx, gy, gz;
  int16_t prevAx = 0, prevAy = 0;
  int stableCount = 0;
  long stepCountLeft = 0;
  
  // Pasos iniciales
  for (int i = 0; i < MIN_STEPS_BEFORE_CHECK; i++) {
    stepX(false);  // Izquierda
    stepCountLeft++;
    if (CALIBRATION_DELAY > 0) delay(CALIBRATION_DELAY);
  }
  
  // Obtener lectura inicial
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  prevAx = ax;
  prevAy = ay;
  
  // Buscar límite izquierdo
  while (stepCountLeft < 10000) {
    stepX(false);  // Izquierda
    stepCountLeft++;
    if (CALIBRATION_DELAY > 0) delay(CALIBRATION_DELAY);
    
    if (stepCountLeft % IMU_CHECK_INTERVAL == 0) {
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      
      int16_t diffAx = abs(ax - prevAx);
      int16_t diffAy = abs(ay - prevAy);
      bool atLimit = (diffAx < IMU_THRESHOLD) && (diffAy < IMU_THRESHOLD);
      
      if (atLimit) {
        stableCount++;
        Serial.print("●");
      } else {
        stableCount = 0;
        Serial.print(".");
      }
      
      if (stableCount >= STABLE_COUNT) {
        Serial.println("\n✓ ¡LÍMITE IZQUIERDO ENCONTRADO!");
        break;
      }
      
      prevAx = ax;
      prevAy = ay;
      
      if (stepCountLeft % 400 == 0) {
        Serial.print("\n["); Serial.print(stepCountLeft);
        Serial.print(" pasos] ΔAx="); Serial.print(diffAx);
        Serial.print(" ΔAy="); Serial.println(diffAy);
      }
    }
  }
  
  Serial.print("\n📍 Límite IZQUIERDO: "); Serial.print(stepCountLeft); Serial.println(" pasos\n");
  delay(500);
  
  // ============================================
  // FASE 2: Buscar límite DERECHO (positivo)
  // ============================================
  Serial.println("═══ FASE 2: Buscando límite DERECHO ═══");
  Serial.println("Moviendo hacia la DERECHA...\n");
  
  stableCount = 0;
  long stepCountRight = 0;
  
  // Pasos iniciales
  for (int i = 0; i < MIN_STEPS_BEFORE_CHECK; i++) {
    stepX(true);  // Derecha
    stepCountRight++;
    if (CALIBRATION_DELAY > 0) delay(CALIBRATION_DELAY);
  }
  
  // Obtener lectura inicial
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  prevAx = ax;
  prevAy = ay;
  
  // Buscar límite derecho
  while (stepCountRight < 10000) {
    stepX(true);  // Derecha
    stepCountRight++;
    if (CALIBRATION_DELAY > 0) delay(CALIBRATION_DELAY);
    
    if (stepCountRight % IMU_CHECK_INTERVAL == 0) {
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      
      int16_t diffAx = abs(ax - prevAx);
      int16_t diffAy = abs(ay - prevAy);
      bool atLimit = (diffAx < IMU_THRESHOLD) && (diffAy < IMU_THRESHOLD);
      
      if (atLimit) {
        stableCount++;
        Serial.print("●");
      } else {
        stableCount = 0;
        Serial.print(".");
      }
      
      if (stableCount >= STABLE_COUNT) {
        Serial.println("\n✓ ¡LÍMITE DERECHO ENCONTRADO!");
        break;
      }
      
      prevAx = ax;
      prevAy = ay;
      
      if (stepCountRight % 400 == 0) {
        Serial.print("\n["); Serial.print(stepCountRight);
        Serial.print(" pasos] ΔAx="); Serial.print(diffAx);
        Serial.print(" ΔAy="); Serial.println(diffAy);
      }
    }
  }
  
  Serial.print("\n📍 Límite DERECHO: "); Serial.print(stepCountRight); Serial.println(" pasos\n");
  delay(500);
  
  // ============================================
  // FASE 3: Calcular rango total y establecer origen
  // ============================================
  long totalRange = stepCountLeft + stepCountRight;
  
  Serial.println("╔════════════════════════════════════════════╗");
  Serial.println("║           RESULTADOS CALIBRACIÓN           ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  Serial.print("📏 Límite IZQUIERDO:  "); Serial.print(stepCountLeft); Serial.println(" pasos");
  Serial.print("📏 Límite DERECHO:    "); Serial.print(stepCountRight); Serial.println(" pasos");
  Serial.print("📏 RANGO TOTAL:       "); Serial.print(totalRange); Serial.println(" pasos\n");
  
  Serial.println("💡 Actualiza en el código:");
  Serial.print("   #define MAX_X_STEPS "); Serial.println(totalRange);
  Serial.println();
  
  // ============================================
  // FASE 4: Regresar al origen (0,0)
  // ============================================
  Serial.println("═══ Regresando al ORIGEN (límite izquierdo) ═══\n");
  
  // Volver al límite izquierdo (origen 0,0)
  Serial.println("Moviendo hacia ORIGEN...");
  for (long i = 0; i < stepCountRight; i++) {
    stepX(false);  // Regresar todo hacia la izquierda
    if (CALIBRATION_DELAY > 0) delay(CALIBRATION_DELAY);
    if (i % 500 == 0) Serial.print(".");
  }
  
  currentX = 0;
  Serial.println("\n\n✅ ¡Calibración X COMPLETADA!");
  Serial.println("📍 Posición actual: X = 0 (ORIGEN)\n");
}

void calibrateYWithIMU() {
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║  CALIBRACION COMPLETA EJE Y CON IMU       ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  Serial.println("🔍 Buscando ORIGEN y LÍMITES en ambas direcciones...\n");
  
  if (!mpu.testConnection()) {
    Serial.println("⚠ ERROR: IMU no disponible");
    Serial.println("Verifica conexión del MPU6050");
    return;
  }
  
  delay(500);
  
  // ============================================
  // FASE 1: Buscar límite INFERIOR (negativo)
  // ============================================
  Serial.println("═══ FASE 1: Buscando límite INFERIOR ═══");
  Serial.println("Moviendo hacia ABAJO...\n");
  
  int16_t ax, ay, az, gx, gy, gz;
  int16_t prevAx = 0, prevAy = 0;
  int stableCount = 0;
  long stepCountDown = 0;
  
  // Pasos iniciales
  for (int i = 0; i < MIN_STEPS_BEFORE_CHECK; i++) {
    stepY(false);  // Abajo
    stepCountDown++;
    if (CALIBRATION_DELAY > 0) delay(CALIBRATION_DELAY);
  }
  
  // Obtener lectura inicial
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  prevAx = ax;
  prevAy = ay;
  
  // Buscar límite inferior
  while (stepCountDown < 10000) {
    stepY(false);  // Abajo
    stepCountDown++;
    if (CALIBRATION_DELAY > 0) delay(CALIBRATION_DELAY);
    
    if (stepCountDown % IMU_CHECK_INTERVAL == 0) {
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      
      int16_t diffAx = abs(ax - prevAx);
      int16_t diffAy = abs(ay - prevAy);
      bool atLimit = (diffAx < IMU_THRESHOLD) && (diffAy < IMU_THRESHOLD);
      
      if (atLimit) {
        stableCount++;
        Serial.print("●");
      } else {
        stableCount = 0;
        Serial.print(".");
      }
      
      if (stableCount >= STABLE_COUNT) {
        Serial.println("\n✓ ¡LÍMITE INFERIOR ENCONTRADO!");
        break;
      }
      
      prevAx = ax;
      prevAy = ay;
      
      if (stepCountDown % 400 == 0) {
        Serial.print("\n["); Serial.print(stepCountDown);
        Serial.print(" pasos] ΔAx="); Serial.print(diffAx);
        Serial.print(" ΔAy="); Serial.println(diffAy);
      }
    }
  }
  
  Serial.print("\n📍 Límite INFERIOR: "); Serial.print(stepCountDown); Serial.println(" pasos\n");
  delay(500);
  
  // ============================================
  // FASE 2: Buscar límite SUPERIOR (positivo)
  // ============================================
  Serial.println("═══ FASE 2: Buscando límite SUPERIOR ═══");
  Serial.println("Moviendo hacia ARRIBA...\n");
  
  stableCount = 0;
  long stepCountUp = 0;
  
  // Pasos iniciales
  for (int i = 0; i < MIN_STEPS_BEFORE_CHECK; i++) {
    stepY(true);  // Arriba
    stepCountUp++;
    if (CALIBRATION_DELAY > 0) delay(CALIBRATION_DELAY);
  }
  
  // Obtener lectura inicial
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  prevAx = ax;
  prevAy = ay;
  
  // Buscar límite superior
  while (stepCountUp < 10000) {
    stepY(true);  // Arriba
    stepCountUp++;
    if (CALIBRATION_DELAY > 0) delay(CALIBRATION_DELAY);
    
    if (stepCountUp % IMU_CHECK_INTERVAL == 0) {
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      
      int16_t diffAx = abs(ax - prevAx);
      int16_t diffAy = abs(ay - prevAy);
      bool atLimit = (diffAx < IMU_THRESHOLD) && (diffAy < IMU_THRESHOLD);
      
      if (atLimit) {
        stableCount++;
        Serial.print("●");
      } else {
        stableCount = 0;
        Serial.print(".");
      }
      
      if (stableCount >= STABLE_COUNT) {
        Serial.println("\n✓ ¡LÍMITE SUPERIOR ENCONTRADO!");
        break;
      }
      
      prevAx = ax;
      prevAy = ay;
      
      if (stepCountUp % 400 == 0) {
        Serial.print("\n["); Serial.print(stepCountUp);
        Serial.print(" pasos] ΔAx="); Serial.print(diffAx);
        Serial.print(" ΔAy="); Serial.println(diffAy);
      }
    }
  }
  
  Serial.print("\n📍 Límite SUPERIOR: "); Serial.print(stepCountUp); Serial.println(" pasos\n");
  delay(500);
  
  // ============================================
  // FASE 3: Calcular rango total y establecer origen
  // ============================================
  long totalRange = stepCountDown + stepCountUp;
  
  Serial.println("╔════════════════════════════════════════════╗");
  Serial.println("║           RESULTADOS CALIBRACIÓN           ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  Serial.print("📏 Límite INFERIOR:   "); Serial.print(stepCountDown); Serial.println(" pasos");
  Serial.print("📏 Límite SUPERIOR:   "); Serial.print(stepCountUp); Serial.println(" pasos");
  Serial.print("📏 RANGO TOTAL:       "); Serial.print(totalRange); Serial.println(" pasos\n");
  
  Serial.println("💡 Actualiza en el código:");
  Serial.print("   #define MAX_Y_STEPS "); Serial.println(totalRange);
  Serial.println();
  
  // ============================================
  // FASE 4: Regresar al origen (0,0)
  // ============================================
  Serial.println("═══ Regresando al ORIGEN (límite inferior) ═══\n");
  
  // Volver al límite inferior (origen 0,0)
  Serial.println("Moviendo hacia ORIGEN...");
  for (long i = 0; i < stepCountUp; i++) {
    stepY(false);  // Regresar todo hacia abajo
    if (CALIBRATION_DELAY > 0) delay(CALIBRATION_DELAY);
    if (i % 500 == 0) Serial.print(".");
  }
  
  currentY = 0;
  Serial.println("\n\n✅ ¡Calibración Y COMPLETADA!");
  Serial.println("📍 Posición actual: Y = 0 (ORIGEN)\n");
}

// Mantener funciones antiguas como respaldo
void calibrateX() {
  Serial.println("\n⚠ Usando calibración MANUAL");
  Serial.println("Recomendado: Usa comando 'C' para calibración con IMU");
  Serial.println();
  calibrateXWithIMU();  // Redirigir a la versión IMU
}

void calibrateY() {
  Serial.println("\n⚠ Usando calibración MANUAL");
  Serial.println("Recomendado: Usa comando 'D' para calibración con IMU");
  Serial.println();
  calibrateYWithIMU();  // Redirigir a la versión IMU
}

void testAreaCompleta() {
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║     TEST ÁREA COMPLETA - 4 DIRECCIONES    ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  Serial.println("🔍 Probando movimiento en TODAS las direcciones");
  Serial.println("   desde la posición actual...\n");
  
  // Guardar posición inicial
  long startX = currentX;
  long startY = currentY;
  
  Serial.print("📍 Posición inicial: X=");
  Serial.print(startX);
  Serial.print(" Y=");
  Serial.println(startY);
  Serial.println();
  
  // ============================================
  // TEST 1: DERECHA (X+)
  // ============================================
  Serial.println("═══ TEST 1/4: DERECHA (X+) ═══");
  long stepsRight = MAX_X_STEPS - currentX;  // Hasta el límite derecho
  if (stepsRight > 500) stepsRight = 500;     // Máximo 500 pasos de prueba
  Serial.print("→→→ Moviendo ");
  Serial.print(stepsRight);
  Serial.println(" pasos a la DERECHA...");
  moveX(stepsRight);
  delay(1000);
  
  // Regresar
  Serial.println("←←← Regresando...");
  moveX(-stepsRight);
  delay(1000);
  Serial.println("✓ Test DERECHA OK\n");
  
  // ============================================
  // TEST 2: IZQUIERDA (X-)
  // ============================================
  Serial.println("═══ TEST 2/4: IZQUIERDA (X-) ═══");
  long stepsLeft = currentX;  // Hasta el límite izquierdo (0)
  if (stepsLeft > 500) stepsLeft = 500;  // Máximo 500 pasos de prueba
  Serial.print("←←← Moviendo ");
  Serial.print(stepsLeft);
  Serial.println(" pasos a la IZQUIERDA...");
  moveX(-stepsLeft);
  delay(1000);
  
  // Regresar
  Serial.println("→→→ Regresando...");
  moveX(stepsLeft);
  delay(1000);
  Serial.println("✓ Test IZQUIERDA OK\n");
  
  // ============================================
  // TEST 3: ARRIBA (Y+)
  // ============================================
  Serial.println("═══ TEST 3/4: ARRIBA (Y+) ═══");
  long stepsUp = MAX_Y_STEPS - currentY;  // Hasta el límite superior
  if (stepsUp > 500) stepsUp = 500;  // Máximo 500 pasos de prueba
  Serial.print("↑↑↑ Moviendo ");
  Serial.print(stepsUp);
  Serial.println(" pasos hacia ARRIBA...");
  moveY(stepsUp);
  delay(1000);
  
  // Regresar
  Serial.println("↓↓↓ Regresando...");
  moveY(-stepsUp);
  delay(1000);
  Serial.println("✓ Test ARRIBA OK\n");
  
  // ============================================
  // TEST 4: ABAJO (Y-)
  // ============================================
  Serial.println("═══ TEST 4/4: ABAJO (Y-) ═══");
  long stepsDown = currentY;  // Hasta el límite inferior (0)
  if (stepsDown > 500) stepsDown = 500;  // Máximo 500 pasos de prueba
  Serial.print("↓↓↓ Moviendo ");
  Serial.print(stepsDown);
  Serial.println(" pasos hacia ABAJO...");
  moveY(-stepsDown);
  delay(1000);
  
  // Regresar
  Serial.println("↑↑↑ Regresando...");
  moveY(stepsDown);
  delay(1000);
  Serial.println("✓ Test ABAJO OK\n");
  
  // ============================================
  // VERIFICAR POSICIÓN FINAL
  // ============================================
  Serial.println("╔════════════════════════════════════════════╗");
  Serial.println("║          RESULTADOS TEST ÁREA             ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  
  Serial.print("📍 Posición inicial: X=");
  Serial.print(startX);
  Serial.print(" Y=");
  Serial.println(startY);
  
  Serial.print("📍 Posición final:   X=");
  Serial.print(currentX);
  Serial.print(" Y=");
  Serial.println(currentY);
  
  if (currentX == startX && currentY == startY) {
    Serial.println("\n✅ ¡PERFECTO! Regresó a la posición inicial");
  } else {
    Serial.println("\n⚠️ ATENCIÓN: No regresó a la posición inicial");
    Serial.print("   Error X: ");
    Serial.println(currentX - startX);
    Serial.print("   Error Y: ");
    Serial.println(currentY - startY);
  }
  
  Serial.println("\n✅ Test completo de 4 direcciones COMPLETADO\n");
}

// ============================================
// PROCESAR COMANDOS
// ============================================

void processCommand(char cmd) {
  cmd = toupper(cmd);
  Serial.print("\n> ");
  Serial.println(cmd);
  
  switch(cmd) {
    case 'X': testMotorX(); break;
    case 'Y': testMotorY(); break;
    case 'Z': testMotorZ(); break;
    case 'S': drawSquare(); break;
    case 'H': goHome(); break;
    case 'P': printPosition(); break;
    case 'C': calibrateXWithIMU(); break;  // Calibrar eje X con IMU
    case 'D': calibrateYWithIMU(); break;  // Calibrar eje Y con IMU (nota: D está duplicado)
    case 'A': testAreaCompleta(); break;   // Test área completa
    case 'I': printIMUData(); break;       // Ver datos IMU
    case 'U': penUp(); break;              // ⬆️ SUBIR lápiz
    case 'B': penDown(); break;            // ⬇️ BAJAR lápiz (B en vez de D porque D es calibrar Y)
    case 'R': releasePenMotor(); break;    // 🔌 Liberar motor Z
    default:
      Serial.println("\n╔════════════════════════════════════╗");
      Serial.println("║     COMANDOS DISPONIBLES          ║");
      Serial.println("╚════════════════════════════════════╝");
      Serial.println("📐 MOTORES:");
      Serial.println("  X = Test Motor X");
      Serial.println("  Y = Test Motor Y");
      Serial.println("  Z = Test Motor Z");
      Serial.println();
      Serial.println("🎨 DIBUJO:");
      Serial.println("  S = Cuadrado");
      Serial.println("  U = ⬆️ Lápiz ARRIBA (Pen Up)");
      Serial.println("  B = ⬇️ Lápiz ABAJO (Pen Down)");
      Serial.println("  R = 🔌 Liberar Motor Z");
      Serial.println();
      Serial.println("🏠 NAVEGACIÓN:");
      Serial.println("  H = Home (0,0,0)");
      Serial.println("  P = Posición actual");
      Serial.println();
      Serial.println("⚙️ CALIBRACIÓN:");
      Serial.println("  C = Calibrar eje X con IMU");
      Serial.println("  D = Calibrar eje Y con IMU");
      Serial.println("  A = Test área completa");
      Serial.println("  I = Ver datos IMU");
      Serial.println();
      break;
  }
  Serial.flush();
}
