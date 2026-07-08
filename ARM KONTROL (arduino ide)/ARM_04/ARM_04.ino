#include <Arduino.h>
#include <Servo.h>

#define pinhome 13 // posisi awal
#define pinwork 12 // mode kerja
#define pindemo 9 // mode demonstrasi (beberapa gerakan kemampuan robot)

//sensor
#define sensor 8 // megaatmega2560/ sensor untuk mode kerja (belum ditentukan sensor apa)


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
const float L1 = 8.5;
const float L2 = 15.0;
const float L3 = 15.0;
const float L4 = 6.5;
const float L5 = 12.0;

float sekarang_s0 = 90;
float sekarang_s1 = 20;
float sekarang_s2 = 25;
float sekarang_s3 = 90;
float sekarang_s4 = 90;

// ===== PROTOTIPE FUNGSI =====
void ik_task(float x, float y, float z_target, float phi_deg,
                      float *t0_deg, float *t1_deg, float *t2_deg, 
                      float *t3_deg, float *t4_deg);

float keServo(float sudut);

void mapkeServo(float t0, float t1, float t2, float t3, float t4,
                float *s0, float *s1, float *s2, float *s3, float *s4);

void gerakKe(float x, float y, float z, float phi);

//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW

void setup() {
  pinMode(pinhome, INPUT_PULLUP);
  pinMode(pinwork, INPUT_PULLUP);
  pinMode(pindemo, INPUT_PULLUP);
  
  base.attach(25);
  shoulder.attach(27);
  elbow.attach(23);
  wristP.attach(29);
  wristR.attach(31);
  gripper.attach(33);

  Serial.begin(115200);
  
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

  delay(10000);
}
//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW

//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

void loop() {

  gerakKe(10, 15, -2, -90);

  delay(1000);
  gripper.write(25);
  delay(500);
  
  gerakKe(-10, 20, -2, -90);
  delay(1000);
  gripper.write(90);
  delay(500);

  
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
  // =========================
  // KOREKSI MEKANIK
  // =========================
  *s0 = t0;
  *s1 = 180 - t1;
  *s2 = t2 + 180;
  *s3 = 180 - (t3 + 90);
  *s4 = t4;
  // =========================
  // CLAMP 0–180
  // =========================
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

void gerakKe(float x, float y, float z, float phi) {

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

  // =========================================
  // LINEAR INTERPOLATION
  // =========================================

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

    float p = (float)i / steps;

    sekarang_s0 = start_s0 + (s0 - start_s0) * p;
    sekarang_s1 = start_s1 + (s1 - start_s1) * p;
    sekarang_s2 = start_s2 + (s2 - start_s2) * p;
    sekarang_s3 = start_s3 + (s3 - start_s3) * p;
    sekarang_s4 = start_s4 + (s4 - start_s4) * p;

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

    delay(25);
  }

  sekarang_s0 = s0;
  sekarang_s1 = s1;
  sekarang_s2 = s2;
  sekarang_s3 = s3;
  sekarang_s4 = s4;

}
//################################################################
//################################################################


//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW
//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW
//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW



