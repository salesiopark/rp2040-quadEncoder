/* 2025년10월18일 초안 작성
 * rp2040-quadEncoder를 사용한
 * 모터 위치(radian)/각속도(radian/sec) 측정
 */
#include "quadEncoder.h"

QuadEncoder  qEnc(14, 420);//pinA, encoderPPR*GearRatio

void setup() {
  Serial.begin(460800);
  while(!Serial) delay(10);
}

void loop() {
  double rad = qEnc.get_radian_normalized();
  double spd = qEnc.get_speed_radian();
  
  Serial.print(rad);
  Serial.print("rad, ");
  Serial.print(spd);
  Serial.println("rad/sec");
  delay(10);
}