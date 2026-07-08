#include <Servo.h>

#define pinhome 13 // posisi awal
#define pinwork 12 // mode kerja
#define pindemo 8 // mode demonstrasi (beberapa gerakan kemampuan robot)

//*****
#define sensor 7 // sensor untuk mode kerja (belum tau sensor apa)
//*****

//nama servo
Servo base;
Servo shoulder;
Servo elbow;
Servo wristP;
Servo wristR;
Servo gripper;

#include <math.h>
//dimensi robot
const float L1 = 5.0;
const float L2 = 10.0;
const float L3 = 10.0;
const float L4 = 2.0;
const float L5 = 3.0;

// ===== PROTOTIPE FUNGSI =====
void ik_task(float x, float y, float z_target, float phi_deg,
                      float *t0_deg, float *t1_deg, float *t2_deg, 
                      float *t3_deg, float *t4_deg);


//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
void setup() {
  pinMode(pinhome, INPUT_PULLUP);
  pinMode(pinwork, INPUT_PULLUP);
  pinMode(pindemo, INPUT_PULLUP);
  
  base.attach(11);
  shoulder.attach(10);
  elbow.attach(9);
  wristP.attach(6);
  wristR.attach(5);
  gripper.attach(3);

  Serial.begin(115200);
}
//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW


//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
void loop() {
  // ===== HARD CODE TARGET =====
  float x = -1.0;
  float y = 1.0;
  float z = 0.0;
  float phi = -90;

  // ===== VARIABEL PENAMPUNG =====
  float t0, t1, t2, t3, t4;

  // ===== PANGGIL IK =====
  ik_task(x, y, z, phi, &t0, &t1, &t2, &t3, &t4);

  // ===== CETAK KE SERIAL (untuk monitor) =====
  Serial.print("Base: "); Serial.print(t0);
  Serial.print(" | Shoulder: "); Serial.print(t1);
  Serial.print(" | Elbow: "); Serial.print(t2);
  Serial.print(" | WristP: "); Serial.print(t3);
  Serial.print(" | WristR: "); Serial.println(t4);

  // ==== MAPPING & PEMBATAS ====
  // belum

  // ===== KONTROL SERVO =====
  //sementara 90 semua
  base.write(90);
  shoulder.write(90);
  elbow.write(90);
  wristP.write(90);
  wristR.write(90);
  
  // gripper sementara di posisi (60°)
  gripper.write(60);
  delay(1000);
  gripper.write(30);
  
}
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA


//################################################################
//################################################################
// ==== FUNGSI IK ====
void ik_task(float x, float y, float z_target, float phi_deg,
                      float *t0_deg, float *t1_deg, float *t2_deg, 
                      float *t3_deg, float *t4_deg) {
  
  // ===== Base rotation =====
  float theta0 = atan2(y, x);
  float theta0_deg = theta0 * 180.0 / PI;
  
  // ===== Wrist rotation (kompensasi) =====
  // menjaga agar gripper tetap (dikunci) posisi
  float theta4_deg = 180.0 - theta0_deg;
  
  // ===== Transformasi ke plane =====
  float x_plane_tcp = x * cos(theta0) + y * sin(theta0);
  float y_plane_tcp = z_target - L1;

  // ===== Tool offset =====
  float L_tool = L4 + L5;
  float phi = phi_deg * PI / 180.0;  // konversi ke radian
  
  float x_plane = x_plane_tcp - L_tool * cos(phi);
  float y_plane = y_plane_tcp - L_tool * sin(phi);
  
  // ===== Jarak ke shoulder =====
  float r = sqrt(x_plane * x_plane + y_plane * y_plane);
  
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




