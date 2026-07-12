import cv2 
import numpy as np
import math

# Program ini adalah simulasi Inverse Kinematik (IK) untuk robot arm 5 DOF dengan 
# joint base, shoulder, elbow, wrist pitch, dan wrist rotation.
# menghasilkan gambar untuk memudahkan sinkronisasi model IK dengan kondisi fisik robot

# setelah program berjalan tekan esc untuk keluar

#####################################
#####################################
# ganti koordinat disini (cm)
z_target = -5
x_cm = -2
y_cm = 15

# ganti dimensi robot disini (cm)
L1 = 7.9
L2 = 15.0
L3 = 13.0
L4 = 7.3
L5 = 11.0
#####################################
#####################################

#######################################################
# INVERSE KINEMATICS 4DOF + WRIST ROTATION mirror BASE
#######################################################
def ik_task(x, y, z_target, phi_deg=-90):
    """ IK 4DOF + tambahan wrist rotation (T4) """

    # ===== Base rotation =====
    theta0 = math.atan2(y, x)
    theta0_deg = math.degrees(theta0)

    # ===== Wrist rotation (kompensasi) =====
    # menjaga agar gripper menyesuaikan arah base
    # 180 untuk membalik arah di simulasi
    # untuk pemakaian ke fisik hapus bagian (180 -) untuk phi -90
    theta4_deg = 180 - theta0_deg

    # ===== Transformasi ke plane =====
    x_plane_tcp = x * math.cos(theta0) + y * math.sin(theta0)
    y_plane_tcp = z_target - L1

    # ===== Tool offset =====
    L_tool = L4 + L5
    phi = math.radians(phi_deg)

    x_plane = x_plane_tcp - L_tool * math.cos(phi)
    y_plane = y_plane_tcp - L_tool * math.sin(phi)

    # ===== Jarak ke shoulder =====
    r = math.hypot(x_plane, y_plane) 
    # belum dipakai, jarak ini bisa digunakan untuk pembatas jangkauan

    # ===== Elbow-up solution =====
    cos_theta2 = (x_plane**2 + y_plane**2 - L2**2 - L3**2) / (2 * L2 * L3)
    theta2 = -math.acos(np.clip(cos_theta2, -1.0, 1.0))

    k1 = L2 + L3 * math.cos(theta2)
    k2 = L3 * math.sin(theta2)

    theta1 = math.atan2(y_plane, x_plane) - math.atan2(k2, k1)

    # ===== Wrist pitch =====
    theta3 = phi - (theta1 + theta2)

    # ===== Konversi ke derajat =====
    theta1_deg = math.degrees(theta1)
    theta2_deg = math.degrees(theta2)
    theta3_deg = math.degrees(theta3)

    return theta0_deg, theta1_deg, theta2_deg, theta3_deg, theta4_deg
#######################################################
#######################################################

#######################################################
# Gambar simulasi 3DOF
#######################################################
frame_w, frame_h = 500, 400 #ukuran frame
cx, cy = 200, 250 #pangkal/base
cmpixel = 10 #cm per pixel di simulasi untuk ukuran gambar

def draw_3dof_gripper(theta1_sim, theta2_sim, theta3_sim):
    """
    Visualisasi robot 3DOF planar:
    Base → Shoulder → Elbow → Wrist → Gripper
    """
    # ===== Konversi sudut ke radian =====
    # Konversi sudut dari derajat ke radian untuk perhitungan trig
    theta1s = math.radians(theta1_sim)
    theta2s = math.radians(theta2_sim)
    theta3s = math.radians(theta3_sim)

    # ===== Hitung posisi joint =====
    x0, y0 = cx, cy                    # Base (titik tetap)
    x1, y1 = x0, y0 - int(L1*cmpixel)  # Shoulder (offset vertikal L1)
    
    x2 = x1 + int(L2*cmpixel * math.cos(theta1s))
    y2 = y1 - int(L2*cmpixel * math.sin(theta1s))  # Elbow
    
    x3 = x2 + int(L3*cmpixel * math.cos(theta1s + theta2s))
    y3 = y2 - int(L3*cmpixel * math.sin(theta1s + theta2s))  # Wrist
    
    L_tool = L4 + L5   # Gripper (panjang L4 + L5)
    
    # Hitung posisi ujung gripper (TCP):
    # Geser dari wrist sepanjang L_tool mengikuti total sudut
    # Total sudut = shoulder + elbow + wrist (theta1 + theta2 + theta3)
    xg = x3 + int(L_tool*cmpixel * math.cos(theta1s + theta2s + theta3s))
    yg = y3 - int(L_tool*cmpixel * math.sin(theta1s + theta2s + theta3s))

    # ===== Buat frame kosong =====
    framesim = np.zeros((frame_h, frame_w, 3), dtype=np.uint8)

    # ===== Gambar link =====
    cv2.line(framesim, (x0, y0), (x1, y1), (255, 0, 0), 3)  # Base -> Shoulder
    cv2.line(framesim, (x1, y1), (x2, y2), (0, 255, 0), 3)  # Shoulder -> Elbow
    cv2.line(framesim, (x2, y2), (x3, y3), (0, 0, 255), 3)  # Elbow -> Wrist
    cv2.line(framesim, (x3, y3), (xg, yg), (0, 255, 255), 3) # Wrist -> Gripper

    # ===== Gambar titik joint =====
    cv2.circle(framesim, (x0, y0), 5, (0,0,255), -1)   # Base
    cv2.circle(framesim, (x1, y1), 5, (0,255,0), -1)   # Shoulder
    cv2.circle(framesim, (x2, y2), 5, (255,0,0), -1)   # Elbow
    cv2.circle(framesim, (x3, y3), 7, (0,255,255), -1) # Wrist
    cv2.circle(framesim, (xg, yg), 7, (0,255,255), -1) # Gripper ujung

    # ===== Info sudut di frame =====
    cv2.putText(framesim, f"T1:{theta1_sim:.1f}", (10, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255), 1)
    cv2.putText(framesim, f"T2:{theta2_sim:.1f}", (10, 40), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255), 1)
    cv2.putText(framesim, f"T3:{theta3_sim:.1f}", (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255), 1)

    # ===== Tampilkan =====
    cv2.imshow("Simulasi 3DOF Planar", framesim)
    cv2.waitKey(1)
#######################################################
#######################################################

####################################################### 
# Gambar Base + Gripper 
#######################################################
frame_size = 500
center = (frame_size // 2, frame_size // 2)
length_arrow = 100

def draw_base_and_gripper(theta0_deg, theta4_deg):
    """
    Visualisasi arah base (T0) dan arah gripper (T0 + T4)
    """

    # ===== Frame kosong =====
    frame_base = np.zeros((frame_size, frame_size, 3), dtype=np.uint8)
    # ===== Base =====
    theta0_rad = math.radians(theta0_deg)

    x_base = int(center[0] + length_arrow * math.cos(theta0_rad))
    y_base = int(center[1] - length_arrow * math.sin(theta0_rad))

    # garis base (putih)
    cv2.line(frame_base, center, (x_base, y_base), (255, 255, 255), 7)
    cv2.putText(frame_base, f"T0: {theta0_deg:.1f}°", (x_base + 5, y_base),
                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255), 1)

    # ===== Gripper =====
    theta_gripper_deg = theta4_deg
    theta_gripper_rad = math.radians(theta_gripper_deg)

    xg = int(center[0] + length_arrow * math.cos(theta_gripper_rad))
    yg = int(center[1] - length_arrow * math.sin(theta_gripper_rad))

    # garis gripper (kuning)
    cv2.line(frame_base, center, (xg, yg), (255,0,255), 3)
    cv2.putText(frame_base, f"GRIP: {theta_gripper_deg:.1f}°", (xg + 5, yg),
                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,0,255), 1)

    # ===== Garis referensi =====
    # 0° kanan
    cv2.line(frame_base, center, (center[0] + length_arrow, center[1]), (0, 0, 255), 1)
    cv2.putText(frame_base, "0°", (center[0] + length_arrow + 5, center[1]),
                cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,0,255),1)
    # 90° atas
    cv2.line(frame_base, center, (center[0], center[1] - length_arrow), (0, 255, 0), 1)
    cv2.putText(frame_base, "90°", (center[0], center[1] - length_arrow - 10),
                cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,255,0),1)
    # 180° kiri
    cv2.line(frame_base, center, (center[0] - length_arrow, center[1]), (255, 0, 0), 1)
    cv2.putText(frame_base, "180°", (center[0] - length_arrow - 35, center[1]),
                cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255,0,0),1)
    # 270° bawah
    cv2.line(frame_base, center, (center[0], center[1] + length_arrow), (255, 255, 0), 1)
    cv2.putText(frame_base, "270°", (center[0] + 5, center[1] + length_arrow + 15),
                cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255,255,0),1)

    # ===== Tampilkan =====
    cv2.imshow("Base vs Gripper", frame_base)
    cv2.waitKey(40)
#######################################################
#######################################################

#######################################################
#######################################################
#fungsi batas
def to_servo(angle):
    return max(0, min(180, angle))


def map_to_servo(t0, t1, t2, t3, t4):
    # =========================
    # KOREKSI MEKANIK (sesuai kalibrasi pemasangan servo)
    # =========================
    s0 = t0
    s1 = 180 - t1
    s2 = t2 + 180
    s3 = 180 - (t3 + 90)
    s4 = t4
    # =========================
    # CLAMP 0–180
    # =========================
    s0 = to_servo(s0)
    s1 = to_servo(s1)
    s2 = to_servo(s2)
    s3 = to_servo(s3)
    s4 = to_servo(s4)
    return s0, s1, s2, s3, s4
#######################################################
#######################################################

while True:
     # Hitung kinematika invers
    theta0, theta1, theta2, theta3, theta4 = ik_task(x_cm, y_cm, z_target)
    
     # gambar
    draw_base_and_gripper(theta0, theta4)
    draw_3dof_gripper(theta1, theta2, theta3)
    
    t0, t1, t2, t3, t4 = theta0, theta1, theta2, theta3, theta4
    s0, s1, s2, s3, s4 = map_to_servo(t0, t1, t2, t3, t4)

    print(f"S0:{s0:.2f}, S1:{s1:.2f}, S2:{s2:.2f}, S3:{s3:.2f}, S4:{s4:.2f}")

    # kendali Keyboard (esc untuk keluar)
    key = cv2.waitKey(40) & 0xFF 

    if key == 27:
        break

cv2.destroyAllWindows()