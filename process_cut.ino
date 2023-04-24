/*
งานที่เหลือ คือ 
- modeset cut 
- monitos show cut 
- cut start mode  function
- mode online blynk

- ทดลอง เดินเครื่อง 
*/


#include <EEPROM.h>
#include <Keypad_I2C.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// library Rotary encoder
#define LPD3806  2400//รุ่น 600BM Rotary 3.2 cm.
#define ENCODER_A 12// Pin for Encoder A
#define ENCODER_B 13 // Pin for Encoder B

volatile int32_t temp, counter;

volatile int encoder_value = 0;

#define maxTurn 10
float turn;

// library kyypad 4x4
#define I2CADDR 0x20
LiquidCrystal_I2C lcd(0x3F, 20, 4);

const byte ROWS = 4;
const byte COLS = 4;

// กำหนด Key ที่ใช้งาน (4x4)
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};



byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};


Keypad_I2C keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR, PCF8574 );

int cursorColumn = 0;
char key;

#define relay_motor_lo 25
#define relay_motor_cut 26
#define on 0
#define off 1
int corsor_y = 1;
int corsor_x = 0;
int cs_pmy;
int cs_pmx;
String sfu; //show filter unit
String select_set; //status select mode 
String status_set; // select  mode setup Reloan or cut
String status_select;
String sum_input_kb; // palamitor input key board 
String tag_satus_setup; // tagname status mode 
float set_reload = 3500; //set paramiter reloadcablen
float temp_sfu = set_reload ; // vlue filter para mitor 
float set_cut = 10000; // set paramiter Lenghtcut cablen
float set_first = 2100;  // set paramiter first cut
float temp_endcode; 
float calcular_comprimento;
//float calcular_rotary = (2 * 3.143 * 157.5); // คำนวนความยาวที่  end code หมุน 1 รอบ

String  status_mode;
void setup() {

Wire.begin();
keypad.begin( makeKeymap(keys) );
Serial.begin(9600);

pinMode(relay_motor_lo, OUTPUT);
pinMode(relay_motor_cut, OUTPUT);

pinMode(12, INPUT_PULLUP); // internal pullup input pin 2
pinMode(13, INPUT_PULLUP); // internal pullup input pin 3

  //  //Attaching the ISR to encoder A
  attachInterrupt(ENCODER_A, encoder_a, CHANGE);
  attachInterrupt(ENCODER_B, encoder_b, CHANGE);
  // Declare pins as output:
monitor_test();
digitalWrite(relay_motor_lo, off);
digitalWrite(relay_motor_cut, off);

}


void loop() {
    running();
}

void select_mode(){
if  (status_mode  == "running") {
running();    

}
else if (status_mode == "setting"){
  setting();  
}
if (status_mode == "stop"){
stoping();
}


}
void setting(){

}
void  running(){
    endcodeder_run();
    auto_readcut();
    monitor_test();
}

void stoping(){

}

//code end codeder
void endcodeder_run() {
  rotary_endcoder();
  calcular();
  //เช็คค่าการเปลียนแปลงระหว่างการทำงานเครื่องเหมือมีการเปลียนแปลงให้แสดงค่า
  //Serial.println("calcular_comprimento :" + String(calcular_comprimento)+ ":" +String(temp_endcode)); 
  if (calcular_comprimento != temp_endcode) {
    
    firter_unit();
//    cls_display_role_i();
    //  ระบุตำแหน่งในการแสดงใน  lcd  บรรทัด 0 และ ก็เริ่มต้นที่บรรรทัด
    //lcd.setCursor(0, 1);
   // lcd.print("Length: " + String(temp_sfu, 3) + " " + sfu);
   Serial.println("Length: " + String(temp_sfu, 3) + " " + sfu) ;
    temp_endcode = calcular_comprimento;
  }

}
void rotary_endcoder() {
  if ( encoder_value != temp ) {
    Serial.print("Encoder count : ");
    Serial.print(encoder_value);
    Serial.print("     ");

    turn = (float)encoder_value / LPD3806;

    Serial.print(turn, 3); //นับรอบที่ encoder
    Serial.println(" turn");
    temp = encoder_value;
  }
}


// ฟังชั่น interrupt
void encoder_a() {
  int A = digitalRead(ENCODER_A);
  int B = digitalRead(ENCODER_B);
  if (B != A) {
    encoder_value++;
  } else {
    encoder_value--;
  }
}


void encoder_b() {
  int A = digitalRead(ENCODER_A);
  int B = digitalRead(ENCODER_B);
  if (A == B) {
    encoder_value++;
  } else {
    encoder_value--;
  }
}

//function calcular 



void calcular() {
float calcular_rotary = (2 * 3.143 * 157.5); // คำนวนความยาวที่  end code หมุน 1 รอบ
  calcular_comprimento = calcular_rotary * turn;
  Serial.print("ความยาวสาย  ");
  Serial.println(calcular_comprimento,3);

}

void firter_unit(){
  float temp_calcular = calcular_comprimento + set_reload;
 if (temp_calcular < 100.000 ){
  sfu = "mm";
 temp_sfu = temp_calcular;
 }
  else if(temp_calcular > 100.000  && temp_calcular < 9999.999){

    sfu = "cm";
    temp_sfu = temp_calcular/100;

  }
  else if (temp_calcular > 10000 ){
 
    sfu = "m";
    temp_sfu =  temp_calcular/10000;
  }
 
}

//function auto cut and clacualr ระยะสายที่ตั้งค่าไว้
void auto_readcut(){
  float length_cable_now  = calcular_comprimento;
  float setpoint_cut = set_cut - set_first - set_reload;
  Serial.print("setpoint : " + String( setpoint_cut) );
  if(length_cable_now < setpoint_cut){
    Serial.print("relsy motor_lo ON ");
    digitalWrite(relay_motor_lo, on);

  }
  else if (length_cable_now >= setpoint_cut && length_cable_now <= (setpoint_cut +100) ){
    // motor reload rocable stop
    // motor cut cable start 3 s.
    // motor reload rocable start
    digitalWrite(relay_motor_cut, on);
    digitalWrite(relay_motor_lo, off); 
    delay(6000);
    digitalWrite(relay_motor_cut, off);
    digitalWrite(relay_motor_lo, on);
    delay(3000);
  }
  else if (length_cable_now >= (set_cut - set_reload) ){
    // motor reload stop and end program
    digitalWrite(relay_motor_lo, off);
    digitalWrite(relay_motor_cut, off);
  }

}

void monitor_test(){
  firter_unit();
  lcd.init();
   lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print("Length: " + String(temp_sfu, 3) + " " + sfu);
  }