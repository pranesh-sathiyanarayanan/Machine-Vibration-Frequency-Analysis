#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD on GPIO 25 (SDA) and GPIO 26 (SCL)
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int AE_PIN = 34;      // Acoustic Emission
const int US_PIN = 35;      // Ultrasonic

const int GREEN_LED = 18;   // Healthy
const int YELLOW_LED = 19;  // Monitor/Warning
const int RED_LED = 21;     // Critical

const int BUZZER = 22;

// Variables
float ae_reading = 0;
float us_reading = 0;
float bearing_health = 100.0;
int fault_score = 0;
String fault_type = "HEALTHY";
String fault_diagnosis = "";

int lcd_page = 0;  // Track which page is displaying

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  // Initialize I2C with GPIO 25 (SDA), GPIO 26 (SCL)
  Wire.begin(25, 26);
  delay(1000);
  
  Serial.println("I2C initialized on GPIO 25 (SDA), GPIO 26 (SCL)");
  
  // Initialize LCD
  lcd.init();
  delay(500);
  lcd.backlight();
  delay(500);
  lcd.clear();
  delay(500);
  
  Serial.println("LCD initialized successfully");
  
  // Setup LED pins
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  // Startup sequence
  displayStartupMessage();
  
  Serial.println("\n╔════════════════════════════════════════════════════╗");
  Serial.println("║   BEARING FAULT DETECTION SYSTEM - ESP32 IoT       ║");
  Serial.println("║   Dual-Modal: Acoustic Emission + Ultrasonic       ║");
  Serial.println("║   LCD Display: Multi-Page Format (GPIO 25, 26)     ║");
  Serial.println("╚════════════════════════════════════════════════════╝\n");
  
  delay(3000);
}

void loop() {
  // Read both sensors
  ae_reading = analogRead(AE_PIN);
  us_reading = analogRead(US_PIN);
  
  // Normalize to 0-100
  float ae_normalized = (ae_reading / 4095.0) * 100.0;
  float us_normalized = (us_reading / 4095.0) * 100.0;
  
  // Calculate combined fault score
  fault_score = (ae_normalized * 0.6) + (us_normalized * 0.4);
  bearing_health = 100.0 - fault_score;
  
  // Constrain values
  if (bearing_health < 0) bearing_health = 0;
  if (bearing_health > 100) bearing_health = 100;
  
  // Predict fault type
  predictFaultType(ae_normalized, us_normalized, bearing_health);
  
  // Rotate LCD display between 3 pages every 10 seconds
  displayLCDPage(ae_normalized, us_normalized);
  
  // Move to next page
  lcd_page++;
  if (lcd_page > 2) lcd_page = 0;
  
  // Generate alerts
  generateAlerts(bearing_health);
  
  // Print detailed dashboard
  printDetailedDashboard(ae_normalized, us_normalized, bearing_health);
  
  // Wait 10 seconds before next update
  delay(10000);
}

void displayStartupMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MACHINE HEALTH");
  lcd.setCursor(0, 1);
  lcd.print("MONITOR v1.0");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
  delay(2000);
  
  lcd.clear();
}

void displayLCDPage(float ae, float us) {
  lcd.clear();
  delay(100);
  
  if (lcd_page == 0) {
    // PAGE 1: Health Status
    displayPage1_Health(ae, us);
  }
  else if (lcd_page == 1) {
    // PAGE 2: Fault Type & Diagnosis
    displayPage2_Fault();
  }
  else if (lcd_page == 2) {
    // PAGE 3: Sensor Readings
    displayPage3_Sensors(ae, us);
  }
}

void displayPage1_Health(float ae, float us) {
  // PAGE 1: Health & Status
  // ┌────────────────┐
  // │Health: 95%     │
  // │Status: HEALTHY │
  // └────────────────┘
  
  lcd.setCursor(0, 0);
  lcd.print("Health: ");
  if (bearing_health < 10) lcd.print(" ");
  lcd.print((int)bearing_health);
  lcd.print("%");
  
  lcd.setCursor(0, 1);
  lcd.print("Status: ");
  
  if (bearing_health > 80) {
    lcd.print("HEALTHY");
  }
  else if (bearing_health > 60) {
    lcd.print("MONITOR");
  }
  else if (bearing_health > 40) {
    lcd.print("WARNING");
  }
  else if (bearing_health > 20) {
    lcd.print("CRITICAL");
  }
  else {
    lcd.print("EMERGENC");
  }
}

void displayPage2_Fault() {
  // PAGE 2: Fault Type & Diagnosis
  // ┌────────────────┐
  // │Fault: SPALLING│
  // │Lubrication OK │
  // └────────────────┘
  
  lcd.setCursor(0, 0);
  lcd.print("Fault:");
  
  String fault_short = fault_type;
  if (fault_short.length() > 10) {
    fault_short = fault_short.substring(0, 10);
  }
  lcd.print(fault_short);
  
  lcd.setCursor(0, 1);
  
  if (fault_type == "BEARING_SPALLING") {
    lcd.print("Spalling!!");
  }
  else if (fault_type == "LUBRICATION_LOSS") {
    lcd.print("Lub Loss!!");
  }
  else if (fault_type == "SEVERE_DAMAGE") {
    lcd.print("Multi-Flt!");
  }
  else if (fault_type == "WEAR_DETECTED") {
    lcd.print("Wear Prog");
  }
  else if (fault_type == "FRICTION_ISSUE") {
    lcd.print("Friction!");
  }
  else if (fault_type == "WARNING") {
    lcd.print("Warn Maint");
  }
  else if (fault_type == "CRITICAL") {
    lcd.print("Critical!");
  }
  else if (fault_type == "EMERGENCY") {
    lcd.print("EMERGENCY");
  }
  else {
    lcd.print("OK Normal");
  }
}

void displayPage3_Sensors(float ae, float us) {
  // PAGE 3: Sensor Readings
  // ┌────────────────┐
  // │AE: 45% US:50%  │
  // │FaultScr: 48/100│
  // └────────────────┘
  
  lcd.setCursor(0, 0);
  lcd.print("AE:");
  if (ae < 10) lcd.print(" ");
  lcd.print((int)ae);
  lcd.print("% US:");
  if (us < 10) lcd.print(" ");
  lcd.print((int)us);
  lcd.print("%");
  
  lcd.setCursor(0, 1);
  lcd.print("Score:");
  if (fault_score < 10) lcd.print(" ");
  lcd.print((int)fault_score);
  lcd.print("/100 ");
  
  if (bearing_health > 80) {
    lcd.print("OK");
  }
  else if (bearing_health > 60) {
    lcd.print("WN");  // WATCH
  }
  else if (bearing_health > 40) {
    lcd.print("WR");  // WARN
  }
  else {
    lcd.print("CR");  // CRIT
  }
}

void predictFaultType(float ae, float us, float health) {
  if (health > 85) {
    fault_type = "HEALTHY";
    fault_diagnosis = "Normal operation - All systems OK";
  }
  else if (health > 75) {
    fault_type = "MONITOR";
    fault_diagnosis = "Minor wear detected - Continue monitoring";
  }
  else if (ae > 70 && us < 50) {
    fault_type = "BEARING_SPALLING";
    fault_diagnosis = "Inner race spalling or crack propagation detected";
  }
  else if (us > 70 && ae < 50) {
    fault_type = "LUBRICATION_LOSS";
    fault_diagnosis = "Lubrication deficiency - friction increasing";
  }
  else if (ae > 60 && us > 60) {
    fault_type = "SEVERE_DAMAGE";
    fault_diagnosis = "Multiple faults detected - URGENT replacement needed";
  }
  else if (ae > 55) {
    fault_type = "WEAR_DETECTED";
    fault_diagnosis = "Progressive bearing wear - schedule maintenance";
  }
  else if (us > 55) {
    fault_type = "FRICTION_ISSUE";
    fault_diagnosis = "Abnormal friction - check alignment and lubrication";
  }
  else if (health > 50) {
    fault_type = "WARNING";
    fault_diagnosis = "Multiple degradation signals - inspect bearing";
  }
  else if (health > 30) {
    fault_type = "CRITICAL";
    fault_diagnosis = "Critical condition - maintenance required immediately";
  }
  else {
    fault_type = "EMERGENCY";
    fault_diagnosis = "EMERGENCY STOP - bearing failure imminent!";
  }
}

void generateAlerts(float health) {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);
  
  if (health > 80) {
    digitalWrite(GREEN_LED, HIGH);
  }
  else if (health > 60) {
    digitalWrite(YELLOW_LED, HIGH);
  }
  else if (health > 40) {
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
  }
  else if (health > 20) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);
    delay(150);
    digitalWrite(BUZZER, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);
  }
  else {
    digitalWrite(RED_LED, HIGH);
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUZZER, HIGH);
      delay(100);
      digitalWrite(BUZZER, LOW);
      delay(100);
    }
  }
}

void printDetailedDashboard(float ae, float us, float health) {
  // Clear screen
  Serial.write(27);
  Serial.print("[2J");
  Serial.write(27);
  Serial.print("[H");
  
  // Header
  Serial.println("\n╔════════════════════════════════════════════════════╗");
  Serial.println("║        BEARING HEALTH MONITORING DASHBOARD         ║");
  Serial.println("║   Acoustic Emission + Ultrasonic Dual-Sensor       ║");
  Serial.println("║          Update Interval: 10 Seconds               ║");
  Serial.println("╚════════════════════════════════════════════════════╝\n");
  
  // Timestamp
  Serial.print("⏱️  Timestamp: ");
  Serial.println(millis() / 1000);
  Serial.println();
  
  // Current Status
  Serial.println("📊 REAL-TIME SENSOR READINGS:");
  Serial.print("   Acoustic Emission (AE):  ");
  Serial.print((int)ae);
  Serial.println("% [Internal damage detection]");
  
  Serial.print("   Ultrasonic (US):          ");
  Serial.print((int)us);
  Serial.println("% [Friction & lubrication]");
  
  // Health Analysis
  Serial.println("\n⚙️  BEARING HEALTH ANALYSIS:");
  Serial.print("   Overall Health Score:    ");
  Serial.print((int)bearing_health);
  Serial.println("%");
  
  Serial.print("   Combined Fault Score:    ");
  Serial.print((int)fault_score);
  Serial.println("/100");
  
  // Health bar
  Serial.print("   Health Bar:              [");
  int bars = (int)(bearing_health / 10);
  for (int i = 0; i < 10; i++) {
    if (i < bars) Serial.print("█");
    else Serial.print("░");
  }
  Serial.println("]");
  
  // Fault Diagnosis
  Serial.println("\n🔍 FAULT DIAGNOSIS:");
  Serial.print("   Detected Type:           ");
  Serial.println(fault_type);
  Serial.print("   Diagnosis:               ");
  Serial.println(fault_diagnosis);
  
  // Status
  Serial.print("\n🔔 CURRENT STATUS:           ");
  if (bearing_health > 80) {
    Serial.println("✓ HEALTHY");
  }
  else if (bearing_health > 60) {
    Serial.println("⚠ MONITOR");
  }
  else if (bearing_health > 40) {
    Serial.println("⚠⚠ WARNING");
  }
  else if (bearing_health > 20) {
    Serial.println("🚨 CRITICAL");
  }
  else {
    Serial.println("🚨🚨 EMERGENCY");
  }
  
  // Recommendations
  Serial.println("\n💡 RECOMMENDED ACTIONS:");
  if (bearing_health > 80) {
    Serial.println("   • Continue normal operation");
    Serial.println("   • Next check: 2 weeks");
  }
  else if (bearing_health > 60) {
    Serial.println("   • Increase monitoring to weekly");
    Serial.println("   • Schedule maintenance: 1-2 weeks");
  }
  else if (bearing_health > 40) {
    Serial.println("   • Schedule maintenance: 3-5 days");
    Serial.println("   • Reduce load if possible");
  }
  else if (bearing_health > 20) {
    Serial.println("   • STOP and inspect bearing");
    Serial.println("   • Replace before resuming");
  }
  else {
    Serial.println("   • EMERGENCY SHUTDOWN NOW");
    Serial.println("   • Bearing failure imminent");
  }
  
  // LED Status
  Serial.println("\n💡 LED INDICATORS:");
  if (bearing_health > 80) {
    Serial.println("   🟢 GREEN - Healthy");
  }
  else if (bearing_health > 60) {
    Serial.println("   🟡 YELLOW - Monitor");
  }
  else if (bearing_health > 40) {
    Serial.println("   🟡🔴 YELLOW+RED - Warning");
  }
  else {
    Serial.println("   🔴 RED - Critical");
  }
  
  // Sensor Status
  Serial.println("\n📈 SENSOR SIGNAL QUALITY:");
  Serial.print("   AE Signal: ");
  if (ae < 20) Serial.println("Low (Good)");
  else if (ae < 50) Serial.println("Medium (Caution)");
  else Serial.println("High (Problematic)");
  
  Serial.print("   US Signal: ");
  if (us < 20) Serial.println("Low (Good)");
  else if (us < 50) Serial.println("Medium (Caution)");
  else Serial.println("High (Problematic)");
  
  // LCD Display Info
  Serial.println("\n📱 LCD DISPLAY:");
  Serial.print("   Current Page: ");
  if (lcd_page == 0) Serial.println("1/3 - Health & Status");
  else if (lcd_page == 1) Serial.println("2/3 - Fault Type");
  else Serial.println("3/3 - Sensor Readings");
  Serial.println("   Rotating every 10 seconds on 16x2 LCD");
  
  // Footer
  Serial.println("\n╔════════════════════════════════════════════════════╗");
  Serial.println("║     Next update in 10 seconds...                   ║");
  Serial.println("╚════════════════════════════════════════════════════╝\n");
}
  