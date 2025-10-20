/* 2025년10월18일 초안 작성
 * rp2040-encoder-library(by G.D.Bruno)를 사용한
 * 모터 위치(radian)/각속도(radian/sec) 측정
 * overflow에 safe한 방식으로 작성됨
 */
#include <pio_encoder.h>

class QuadEncoder {
public:
  // pinA는 A상의 핀번호(B상은 pinA+1로 자동 지정됨)
  QuadEncoder(byte pinA, int encoderPPR, int gearRatio = 1
  ) : _pioEnc(pinA),
      _CPR(4*encoderPPR*gearRatio),
      _radPerCount(TWO_PI/_CPR)
  {
    _pioEnc.begin();
    _countLL = 0;
    _prvCount = _pioEnc.getCount();      // 현재 카운트로 초기화
    _prvCount2 = _prvCount;              // 동일하게 설정
    _prvMillis = millis();               // 현재 시간으로 초기화
    _prvSpeed = 0.0;
  }


  //누적된 (연속값) radian값을 반환
  double get_radian() {
    _updateCountLL();
    return _countLL*_radPerCount;
  }


  // 0~2pi 값(normalized radian)을 반환
  // count가 overflow해도 deltaCount(=count - _prvCount)는
  // 정확하게 계산되므로 overflow에 safe하다
  double get_radian_normalized() {
    _updateCountLL();
    // 0 ~ 2π 범위로 정규화
    long long normalized = _countLL % _CPR;
    if (normalized < 0) normalized += _CPR;
    return normalized*_radPerCount;
  }


  // 회전각속도(radian/sec)를 반환
  double get_speed_radian() {
    unsigned long timeMs = millis();
    // millis()에 overflow가 발생(49일 후)해도
    // deltaMillis는 정확한 값
    unsigned long deltaMillis = timeMs - _prvMillis;
    // dt가 너무 짧으면 이전 속도 반환
    if (deltaMillis < 5) return _prvSpeed;

    long count = _pioEnc.getCount();
    long deltaCount = count - _prvCount2;

    double dt = deltaMillis/1000.0;
    _prvSpeed = deltaCount*_radPerCount/dt;
    _prvMillis = timeMs;
    _prvCount2 = count;

    return  _prvSpeed;
  }

  // 리셋: 현재의 로터위치를 0으로 리셋
  void reset() {
    long count = _pioEnc.getCount();
    _countLL = 0;
    _prvCount = count;
    _prvCount2 = count;
    _prvMillis = millis();
    _prvSpeed = 0.0;
  }
  
  
  long long get_count() {
    _updateCountLL();
    return _countLL;
  }


private:
  //CPR: Count (Edges of A/B) Per Revolution
  //AB상의 모든 edge를 카운트하여 해상도를 4배 높인다
  //(PioEncoder 클래스의 x4모드)
  //get_rad()와 get_speed()메서드에서 사용되는 변수들을 분라하여
  //독립적으로 동작하게금 작성

  //공통으로 사용되는 멤버들
  PioEncoder    _pioEnc;
  const int     _CPR; // 
  const double  _radPerCount;

  // getRad()s메서드에서만 사용되는 변수들
  long long     _countLL; 
  long          _prvCount;

  // getSpeed()메서드에서만 사용되는 변수들
  long          _prvCount2;    
  unsigned long _prvMillis;
  double        _prvSpeed;


  // deltaCount를 계산하여 long long형 변수에 더함으로써
  // 오버플로우를 방지한다.
  inline void _updateCountLL(){
    long count = _pioEnc.getCount();
    _countLL += count - _prvCount;
    _prvCount = count;
  }

};