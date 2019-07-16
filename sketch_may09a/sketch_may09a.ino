#include <MsTimer2.h>
#include <Keyboard.h>

#define SSI_ENC_1_INI 6
#define SSI_ENC_1_CLK 9
#define SSI_ENC_1_DAT 10

//+5 Brown GND: Blue A:black, B:purple Z:yellow                              
word rD_SSI_Encoder_1_LpTm;          // 타이머 버퍼.
word rd_SSI_Encoder_1_Data = 0;      // 엔코더 데이타 버퍼
word Old_rd_SSI_Encoder_1_Data = 0;  // 비교용 엔코더 데이타 버퍼
word Fix_SSI_Encoder_1_Data = 0;     // 최종 엔코더 데이타 버퍼
byte rD_SSI_Encoder_1_LpCnt = 0;     // 스위치문 카운터.
byte rD_SSI_Encoder_1_Flag = 0;      // 데이타가 2번 틀릴경우 강제 적용.

const word SS_HEX[12] =
{
  0x0800, 0x0400, 0x0200, 0x0100, 
  0x0080, 0x0040, 0x0020, 0x0010, 
  0x0008, 0x0004, 0x0002, 0x0001 
};

void Call_EncData_Sum(byte inbit, word *sumdata, byte rp)
{
   // 입력되는 데이타를 더함.
  if(inbit) *sumdata +=  SS_HEX[rp];
}

void KeyPress(int Fix_SSI_Encoder_1_Data){
    Serial.println(Fix_SSI_Encoder_1_Data);
    if(Fix_SSI_Encoder_1_Data < 3900 && Fix_SSI_Encoder_1_Data > 2000){
        Keyboard.press(0xD8);
    }
    else if((Fix_SSI_Encoder_1_Data > 180 && Fix_SSI_Encoder_1_Data <= 2000)){
        Keyboard.press(0xD7);
    }
    else{
        Keyboard.release(0xD8);
        Keyboard.release(0xD7);
    }
}

void rD_SSI_Encoder_1_Loop()
{
 
  byte rd_Data = 0,i;
  switch(rD_SSI_Encoder_1_LpCnt)
  {
  case 0:
    // 출력핀 데이타  초기화.
    
    pinMode(SSI_ENC_1_CLK, OUTPUT); 
    pinMode(SSI_ENC_1_DAT, INPUT); 
    
    rD_SSI_Encoder_1_LpCnt = 0x10;
    break;
  case 0x10:
    rD_SSI_Encoder_1_LpTm = 0;       // 타이머 초기화.
    rD_SSI_Encoder_1_LpCnt = 0x12;
    break;
  case 0x12:
    if(rD_SSI_Encoder_1_LpTm > 100)  // 100 mS 동안 대기.
    {
        rD_SSI_Encoder_1_LpCnt = 0x20;
    }
    break;

  case 0x20:
    rD_SSI_Encoder_1_LpTm = 0;       // 타이머 초기화
    rD_SSI_Encoder_1_LpCnt = 0x24;
    break;
  case 0x24:
    if(rD_SSI_Encoder_1_LpTm < 10)  // 100 mS 간격으로 읽음 ( 짧게해도 무관)
    {
    }
    else
    {
      rD_SSI_Encoder_1_LpCnt = 0x30;
    }
    break;
  case 0x30:
    digitalWrite(SSI_ENC_1_CLK,0);  // 클럭 출력 0
    digitalWrite(SSI_ENC_1_CLK,1);  // 클럭 출력 1
    rD_SSI_Encoder_1_LpCnt = 0x32;
    break;
  case 0x32:
    rd_SSI_Encoder_1_Data = 0;      // 입력버퍼 비움.
    for(i=0;i<12;i++)
    {
      rd_Data = digitalRead(SSI_ENC_1_DAT);                   // 데이타 읽음.
      digitalWrite(SSI_ENC_1_CLK,0);                          // 클럭 출력 0
      Call_EncData_Sum(rd_Data, &rd_SSI_Encoder_1_Data, i);   // 엔코더 데이타 취합
      digitalWrite(SSI_ENC_1_CLK,1);                          // 클럭 출력 1
    }
    rD_SSI_Encoder_1_LpCnt = 0x34;
    break;
  case 0x34:
    // 데이타 읽은후 2회중 1회 적용.
    if(rD_SSI_Encoder_1_Flag == 0) rD_SSI_Encoder_1_LpCnt = 0x36;
    else rD_SSI_Encoder_1_LpCnt = 0x38;
    break;
  case 0x36:
    if(Old_rd_SSI_Encoder_1_Data>>2 != rd_SSI_Encoder_1_Data>>2)
    {
       // 이전데이타와 10 비트만 비교.
       Old_rd_SSI_Encoder_1_Data = rd_SSI_Encoder_1_Data;
       rD_SSI_Encoder_1_Flag = 1;
       rD_SSI_Encoder_1_LpCnt = 0x20;
    }
    else
    {
      rD_SSI_Encoder_1_LpCnt = 0xf0;
    }
    break;
  case 0x38:
      rD_SSI_Encoder_1_Flag = 0;
      Old_rd_SSI_Encoder_1_Data = rd_SSI_Encoder_1_Data;
      Fix_SSI_Encoder_1_Data = rd_SSI_Encoder_1_Data; // 데이타를 최종 버퍼로 이동.
      rD_SSI_Encoder_1_LpCnt = 0xf0;
    break;
  case 0xf0:
     KeyPress(Fix_SSI_Encoder_1_Data);
    
    rD_SSI_Encoder_1_LpCnt = 0x20;
    break;
  default:
    rD_SSI_Encoder_1_LpCnt = 0;
    break;
  }
}

#define Encoder_Reset_Timer_Offset ( 1000 * 30)

word Reset_Encoder_1_Point_LpTm = 0;
byte Reset_Encoder_1_Point_LpCnt = 0;
void Reset_Encoder_1_Point()
{
  switch(Reset_Encoder_1_Point_LpCnt)
  {
    case 0:
    // 출력핀 데이타  초기화.
      pinMode(SSI_ENC_1_INI, OUTPUT); 
      digitalWrite(SSI_ENC_1_INI,0);
      Reset_Encoder_1_Point_LpCnt = 0x10;
    break;
  case 0x10:
    Reset_Encoder_1_Point_LpTm = 0;  // 타이머 초기화.
    Reset_Encoder_1_Point_LpCnt = 0x12;
    break;
  case 0x12:
    if(Reset_Encoder_1_Point_LpTm > Encoder_Reset_Timer_Offset)  
    {
      // 일정시간 대기. 
      Reset_Encoder_1_Point_LpCnt = 0x20;
    }
    break;

  case 0x20:
    Reset_Encoder_1_Point_LpTm = 0;  // 타이머 초기화.
    digitalWrite(SSI_ENC_1_INI,1);   // reset
    Reset_Encoder_1_Point_LpCnt = 0x22;
    break;
  case 0x22:
    if(Reset_Encoder_1_Point_LpTm > 10)  // 100mS 동안 초기화 진행.
    {
       digitalWrite(SSI_ENC_1_INI,0);
      Reset_Encoder_1_Point_LpCnt = 0x10;
    }
    break;
  
    default:
      Reset_Encoder_1_Point_LpCnt = 0;
      break;
  }

}

void Timer_1ms()
{
  rD_SSI_Encoder_1_LpTm++;
  Reset_Encoder_1_Point_LpTm++;
}


void setup()
{
  // Timer 1mS
  MsTimer2::set(1, Timer_1ms); // 500ms period
  MsTimer2::start();
  Keyboard.begin();
  Reset_Encoder_1_Point(); 
}

void loop()
{
  rD_SSI_Encoder_1_Loop();
}
