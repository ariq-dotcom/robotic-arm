#include <Arduino.h>
#include <Servo.h>

#define pinhome 13 // posisi awal
#define pinwork 12 // mode kerja
#define pindemo 8 // mode demonstrasi (beberapa gerakan kemampuan robot)
//sensor
#define sensor 11 //sensor untuk mode kerja (belum tau sensor apa)
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
const float L3 = 13.0;
const float L4 = 6.5;
const float L5 = 12.0;

int sekarang_s0 = 90;
int sekarang_s1 = 20;
int sekarang_s2 = 20;
int sekarang_s3 = 90;
int sekarang_s4 = 90;

// ===== PROTOTIPE FUNGSI =====

void ik_task(float x, float y, float z_target, float phi_deg,
                      float *t0_deg, float *t1_deg, float *t2_deg, 
                      float *t3_deg, float *t4_deg);

int keServo(int sudut);

void mapkeServo(int t0, int t1, int t2, int t3, int t4,
                int *s0, int *s1, int *s2, int *s3, int *s4);

void smoothMove(Servo &servo,
                int &currentPos,
                int targetPos,
                int delaySpeed);

void setup() {
  pinMode(pinhome, INPUT_PULLUP);
  pinMode(pinwork, INPUT_PULLUP);
  pinMode(pindemo, INPUT_PULLUP);
  
  base.attach(5);
  shoulder.attach(4);
  elbow.attach(6);
  wristP.attach(2);
  wristR.attach(7);
  gripper.attach(3);

  Serial.begin(115200);
  
  base.write(sekarang_s0);
  shoulder.write(sekarang_s1);
  elbow.write(sekarang_s2);
  wristP.write(sekarang_s3);
  wristR.write(sekarang_s4);

  delay(10000);
}

//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW
//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW
//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW





//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

void loop() {

  // ===== VARIABEL PENAMPUNG IK =====
  float x, y, z, phi;

  float t0, t1, t2, t3, t4;

  // ===== VARIABEL SERVO =====
  int s0, s1, s2, s3, s4;
  
  //satu
  x = -10.0;
  y = 15.0;
  z = 0.0;
  phi = -90;
  ik_task(x, y, z, phi, &t0, &t1, &t2, &t3, &t4);
  mapkeServo((int)t0, (int)t1, (int)t2, (int)t3, (int)t4,
             &s0, &s1, &s2, &s3, &s4);
  // ===== CETAK KE SERIAL (untuk monitor) =====
  Serial.print("Base: "); Serial.print((int)s0);
  Serial.print(" | Shoulder: "); Serial.print((int)s1);
  Serial.print(" | Elbow: "); Serial.print((int)s2);
  Serial.print(" | WristP: "); Serial.print((int)s3);
  Serial.print(" | WristR: "); Serial.println((int)s4);
  smoothMove(base, sekarang_s0, s0, 20);
  delay(1000);
  smoothMove(shoulder, sekarang_s1, s1, 20);
  smoothMove(elbow, sekarang_s2, s2, 20);
  smoothMove(wristP, sekarang_s3, s3, 20);
  smoothMove(wristR, sekarang_s4, s4, 20);
  delay(10000);
  gripper.write(25);
  delay(500);
  gripper.write(90);
  

  //tiga
  x = 10.0;
  y = 15.0;
  z = 0.0;
  phi = -90;
  ik_task(x, y, z, phi, &t0, &t1, &t2, &t3, &t4);
  mapkeServo((int)t0, (int)t1, (int)t2, (int)t3, (int)t4,
             &s0, &s1, &s2, &s3, &s4);
  // ===== CETAK KE SERIAL (untuk monitor) =====
  Serial.print("Base: "); Serial.print((int)s0);
  Serial.print(" | Shoulder: "); Serial.print((int)s1);
  Serial.print(" | Elbow: "); Serial.print((int)s2);
  Serial.print(" | WristP: "); Serial.print((int)s3);
  Serial.print(" | WristR: "); Serial.println((int)s4);
  smoothMove(base, sekarang_s0, s0, 20);
  delay(1000);
  smoothMove(shoulder, sekarang_s1, s1, 20);
  smoothMove(elbow, sekarang_s2, s2, 20);
  smoothMove(wristP, sekarang_s3, s3, 20);
  smoothMove(wristR, sekarang_s4, s4, 20);
  delay(10000);
  gripper.write(25);
  delay(500);
  gripper.write(90);

  
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
int keServo(int sudut) {
  if (sudut < 0) return 0;
  if (sudut > 180) return 180;
  return sudut;
}

//==== MAPPING ====
void mapkeServo(int t0, int t1, int t2, int t3, int t4,
                int *s0, int *s1, int *s2, int *s3, int *s4) {
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

//==== GERAK HALUS INTEGER ====
void smoothMove(Servo &servo,
                int &currentPos,
                int targetPos,
                int delaySpeed) {

  // ===== GERAK NAIK =====
  if (currentPos < targetPos) {

    for (int pos = currentPos;
         pos <= targetPos;
         pos++) {

      servo.write(pos);
      delay(delaySpeed);
    }
  }

  // ===== GERAK TURUN =====
  else if (currentPos > targetPos) {

    for (int pos = currentPos;
         pos >= targetPos;
         pos--) {

      servo.write(pos);
      delay(delaySpeed);
    }
  }

  // ===== UPDATE POSISI SEKARANG =====
  currentPos = targetPos;
}
//################################################################
//################################################################




//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW
//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW
//AAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSNNNNNNNNNNNNNNGGGGGGGGGGGGGWWWWWWWWWWWWWWWWW



