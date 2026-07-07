//KELOMPOK 2

#include <Arduino.h>
#include <Servo.h>

#define pinHOME 53 // posisi awal
#define pinKERJA 51 // mode kerja
#define pinDEMO 49 // mode demonstrasi (beberapa gerakan kemampuan robot)

#define ledHOME 4
#define ledKERJA 3
#define ledDEMO 2

enum Mode {
  HOME,
  KERJA,
  DEMO
};

Mode modeSekarang = HOME;
Mode modeDiminta = HOME;

bool tugasSelesai = true;

//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW
//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW
//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW

//nama servo
Servo base;
Servo shoulder;
Servo elbow;
Servo wristP;
Servo wristR;
Servo gripper;

#include <math.h>
//dimensi robot
const float L1 = 7.9;
const float L2 = 15.;
const float L3 = 13.;
const float L4 = 7.3;
const float L5 = 11;

//untuk nilai sekarang lalu penyimpanan nilai saat berjalan
float sekarang_s0 = 90;
float sekarang_s1 = 20;
float sekarang_s2 = 20;
float sekarang_s3 = 90;
float sekarang_s4 = 90;

//nilai sudut untuk ke posisi home
const float HOME_s0 = 90;
const float HOME_s1 = 20;
const float HOME_s2 = 20;
const float HOME_s3 = 90;
const float HOME_s4 = 90;

// ===== PROTOTIPE FUNGSI =====
void ik_task(float x, float y, float z_target, float phi_deg,
                      float *t0_deg, float *t1_deg, float *t2_deg, 
                      float *t3_deg, float *t4_deg);

float keServo(float sudut);

void mapkeServo(float t0, float t1, float t2, float t3, float t4,
                float *s0, float *s1, float *s2, float *s3, float *s4);

void gerakKe(float x, float y, float z, float phi, int stepDelay);

void gerakServo(float s0, float s1, float s2,
                float s3, float s4);

void bacaTombol();

void updateLED();

void gerakHome();

//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW

void setup() {
  Serial.begin(115200);

  pinMode(pinHOME, INPUT_PULLUP);
  pinMode(pinKERJA, INPUT_PULLUP);
  pinMode(pinDEMO, INPUT_PULLUP);

  pinMode(ledHOME, OUTPUT);
  pinMode(ledKERJA, OUTPUT);
  pinMode(ledDEMO, OUTPUT);

  base.attach(25);
  shoulder.attach(27);
  elbow.attach(23);
  wristP.attach(29);
  wristR.attach(31);
  gripper.attach(33);

  base.write((int)sekarang_s0);
  shoulder.write((int)sekarang_s1);
  elbow.write((int)sekarang_s2);
  wristP.write((int)sekarang_s3);
  wristR.write((int)sekarang_s4);

  // ===== CETAK KE SERIAL (untuk monitor) =====
    Serial.print("Base: "); Serial.print((int)sekarang_s0);
    Serial.print(" | Shoulder: "); Serial.print((int)sekarang_s1);
    Serial.print(" | Elbow: "); Serial.print((int)sekarang_s2);
    Serial.print(" | WristP: "); Serial.print((int)sekarang_s3);
    Serial.print(" | WristR: "); Serial.println((int)sekarang_s4);

  delay(5000);
  updateLED();
}
//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW

//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

void loop() {

  bacaTombol();

  // Menjalankan mode yang aktif
  switch (modeSekarang) {

    case HOME: {
      // Program home
      if (sekarang_s0 != HOME_s0 ||
      sekarang_s1 != HOME_s1 ||
      sekarang_s2 != HOME_s2 ||
      sekarang_s3 != HOME_s3 ||
      sekarang_s4 != HOME_s4) {
      tugasSelesai = false;
      delay(1000);
      gripper.write(95);
      gerakHome();
      tugasSelesai = true;
      }
      break;
    }

    case KERJA: {
      // Program kerja
      tugasSelesai = false;
      Serial.println("hai");
      delay(500);
      tugasSelesai = true;
      break;
    }

    case DEMO: {
      // Program demo
      tugasSelesai = false;

      // x,y,z,phi,delayStep
      gripper.write(25);
      delay(2000);
      gerakKe(0, 15, 1, -90, 7);
      delay(3000);
      gerakKe(0, 15, -5, -90, 7);
      gripper.write(95);
      delay(2000);

      gerakKe(0, 15, 1, -90, 7);
      delay(2000);
      gerakKe(15, 0, 1, -90, 7);
      delay(2000);
      gerakKe(15, 0, -5, -90, 7);
      delay(2000);
      gripper.write(25);
      delay(2000);
      gripper.write(95);
      delay(2000);
      gerakKe(15, 0, 1, -90, 7);
      delay(2000);


      gerakKe(-15, 0, 0, -90, 7);
      delay(2000);
      gerakKe(-15, 0, -5, -90, 7);
      delay(2000);
      gripper.write(25);
      delay(2000);
      gripper.write(95);
      delay(2000);



    

      tugasSelesai = true;
      break;
    }
  }

    if (tugasSelesai && modeDiminta != modeSekarang) {
      modeSekarang = modeDiminta;
      updateLED();
      Serial.print("Pindah ke mode : ");

      switch (modeSekarang) {

        case HOME:
          Serial.println("HOME");
          break;

        case KERJA:
          Serial.println("KERJA");
          break;

        case DEMO:
          Serial.println("DEMO");
          break;
      }
    }
}
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

//################################################################
// FUNGSI IK, FUNGSI IK, FUNGSI IK
//################################################################

// ==== FUNGSI IK ====
void ik_task(float x, float y, float z_target, float phi_deg,
                      float *t0_deg, float *t1_deg, float *t2_deg, 
                      float *t3_deg, float *t4_deg) {
  
  // ===== Base rotation =====
  float theta0 = atan2(y, x);
  float theta0_deg = theta0 * 180.0 / PI;
  
  // ===== Wrist rotation (kompensasi) =====
  // menjaga agar gripper tetap menghadap arah global
  float theta4_deg = theta0_deg;
  
  // ===== Transformasi ke plane =====
  float x_plane_tcp = x * cos(theta0) + y * sin(theta0);
  float y_plane_tcp = z_target - L1;

  // ===== Tool offset =====
  float L_tool = L4 + L5;
  float phi = phi_deg * PI / 180.0;  // konversi ke radian
  
  float x_plane = x_plane_tcp - L_tool * cos(phi);
  float y_plane = y_plane_tcp - L_tool * sin(phi);
  
  // ===== Jarak ke shoulder =====
  //float r = sqrt(x_plane * x_plane + y_plane * y_plane);
  
  // ===== Elbow-up solution =====
  float cos_theta2 = (x_plane*x_plane + y_plane*y_plane - L2*L2 - L3*L3) / (2.0 * L2 * L3);
  cos_theta2 = constrain(cos_theta2, -1.0, 1.0);
  float theta2 = -acos(cos_theta2);
  
  float k1 = L2 + L3 * cos(theta2);
  float k2 = L3 * sin(theta2);

  float theta1 = atan2(y_plane, x_plane) - atan2(k2, k1);
  
  // ===== Wrist pitch =====
  float theta3 = phi - (theta1 + theta2);
  
  // ===== Konversi ke derajat =====
  float theta1_deg = theta1 * 180.0 / PI;
  float theta2_deg = theta2 * 180.0 / PI;
  float theta3_deg = theta3 * 180.0 / PI;

  // ===== KEMBALIKAN HASIL via pointer =====
  *t0_deg = theta0_deg;
  *t1_deg = theta1_deg;
  *t2_deg = theta2_deg;
  *t3_deg = theta3_deg;
  *t4_deg = theta4_deg;
}
//################################################################
//################################################################

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// FUNGSI MAPPING DAN CLAMP
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//==== CLAMP ====
float keServo(float sudut) {
  if (sudut < 0) return 0;
  if (sudut > 180) return 180;
  return sudut;
}

//==== MAPPING ====
void mapkeServo(float t0, float t1, float t2, float t3, float t4,
                float *s0, float *s1, float *s2, float *s3, float *s4) {
  // ==============================
  // KOREKSI DARI GAMBAR KE MEKANIK
  // ==============================
  *s0 = t0;
  *s1 = 180 - t1;
  *s2 = t2 + 180;
  *s3 = 180 - (t3 + 90);
  *s4 = t4;
  // ============
  // CLAMP 0–180
  // ============
  *s0 = keServo(*s0);
  *s1 = keServo(*s1);
  *s2 = keServo(*s2);
  *s3 = keServo(*s3);
  *s4 = keServo(*s4);
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//################################################################
//FUNGSI GERAKAN HALUS
//################################################################

void gerakKe(float x, float y, float z, float phi, int stepDelay) {

  // ===== TARGET IK =====
  float t0, t1, t2, t3, t4;

  // ===== TARGET SERVO =====
  float s0, s1, s2, s3, s4;

  // ===== IK =====
  ik_task(x, y, z, phi,
          &t0, &t1, &t2, &t3, &t4);

  // ===== MAPPING =====
  mapkeServo(t0, t1, t2, t3, t4,
             &s0, &s1, &s2, &s3, &s4);

  gerakServo(s0, s1, s2, s3, s4, stepDelay);
}
//################################################################
//################################################################

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// FUNGSI BACA TOMBOL
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

void bacaTombol() {

  static unsigned long lastDebounce = 0;

  if (millis() - lastDebounce < 200)
    return;

  if (digitalRead(pinHOME) == LOW &&
      modeSekarang != HOME) {

    modeDiminta = HOME;
    Serial.println("Mode HOME diminta");
    lastDebounce = millis();
  }

  else if (digitalRead(pinKERJA) == LOW &&
           modeSekarang != KERJA) {

    modeDiminta = KERJA;
    Serial.println("Mode KERJA diminta");
    lastDebounce = millis();
  }

  else if (digitalRead(pinDEMO) == LOW &&
           modeSekarang != DEMO) {

    modeDiminta = DEMO;
    Serial.println("Mode DEMO diminta");
    lastDebounce = millis();
  }
}

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//################################################################
// FUNGSI GERAK, LINEAR INTERPOLATION
//################################################################
void gerakServo(float s0, float s1, float s2,
                float s3, float s4, int stepDelay)
{
  float start_s0 = sekarang_s0;
  float start_s1 = sekarang_s1;
  float start_s2 = sekarang_s2;
  float start_s3 = sekarang_s3;
  float start_s4 = sekarang_s4;

  float d0 = fabs(s0 - start_s0);
  float d1 = fabs(s1 - start_s1);
  float d2 = fabs(s2 - start_s2);
  float d3 = fabs(s3 - start_s3);
  float d4 = fabs(s4 - start_s4);

  float maxDelta = d0;

  if (d1 > maxDelta) maxDelta = d1;
  if (d2 > maxDelta) maxDelta = d2;
  if (d3 > maxDelta) maxDelta = d3;
  if (d4 > maxDelta) maxDelta = d4;

  int steps = (int)maxDelta;

  if (steps < 1)
    steps = 1;

  for (int i = 1; i <= steps; i++) {

    float p = (float) i / steps;

    sekarang_s0 = start_s0 + (s0 - start_s0) * p;
    sekarang_s1 = start_s1 + (s1 - start_s1) * p;
    sekarang_s2 = start_s2 + (s2 - start_s2) * p;
    sekarang_s3 = start_s3 + (s3 - start_s3) * p;
    sekarang_s4 = start_s4 + (s4 - start_s4) * p;
    
    bacaTombol();

    base.write((int)sekarang_s0);
    shoulder.write((int)sekarang_s1);
    elbow.write((int)sekarang_s2);
    wristP.write((int)sekarang_s3);
    wristR.write((int)sekarang_s4);

    Serial.print("Base: "); Serial.print((int)sekarang_s0);
    Serial.print(" | Shoulder: "); Serial.print((int)sekarang_s1);
    Serial.print(" | Elbow: "); Serial.print((int)sekarang_s2);
    Serial.print(" | WristP: "); Serial.print((int)sekarang_s3);
    Serial.print(" | WristR: "); Serial.println((int)sekarang_s4);

    delay(stepDelay);
  }

  sekarang_s0 = s0;
  sekarang_s1 = s1;
  sekarang_s2 = s2;
  sekarang_s3 = s3;
  sekarang_s4 = s4;
}

//################################################################
//################################################################

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// FUNGSI UPDATE LED
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void updateLED() {

  digitalWrite(ledHOME, modeSekarang == HOME);
  digitalWrite(ledKERJA, modeSekarang == KERJA);
  digitalWrite(ledDEMO, modeSekarang == DEMO);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//################################################################
// FUNGSI KEMBALI KE POSISI HOME
//################################################################
void gerakHome()
{
    gerakServo(
        HOME_s0,
        HOME_s1,
        HOME_s2,
        HOME_s3,
        HOME_s4,
        7
    );
}
//################################################################
//################################################################




//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW
//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW
//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW



