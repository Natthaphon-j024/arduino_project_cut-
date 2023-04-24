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
//int i = 1;
//int num = 0, aa = 0;
//int ans = 0;
// long count = 100000;
// int count1 = 100;
// int total = 0;
// int sum_m = 0;
// int sum_in = 0;
// int sum_out = 0;
// int step = 0;
// int sel = 0;
// int slide = 0;

int corsor_y = 1;
int corsor_x = 0;
int cs_pmy;
int cs_pmx;
//int temp_cls; //clspalamitor แถวที่ 1
//int check_cle;
String sfu; //show filter unit
String select_set; //status select mode 
String status_set; // select  mode setup Reloan or cut
String status_select;
String sum_input_kb; // palamitor input key board 
String tag_satus_setup; // tagname status mode 
float temp_sfu; // vlue filter para mitor 
float set_reload; //set paramiter reloadcablen
float set_cut; // set paramiter Lenghtcut cablen
float set_first;  // set paramiter first cut
float temp_endcode; 
float calcular_comprimento;
float calcular_rotary = (2 * 3.143 * 157.5); // คำนวนความยาวที่  end code หมุน 1 รอบ
//bool main = false, stat2 = false, runn = false;
bool cut = false, stat1 = false;

void setup() {
  Wire.begin();
  keypad.begin( makeKeymap(keys) );
  Serial.begin(9600);

  pinMode(12, INPUT_PULLUP); // internal pullup input pin 2
  pinMode(13, INPUT_PULLUP); // internal pullup input pin 3

  //  pinMode(motor_roll, OUTPUT);
  //
  //  pinMode(motor_cut, OUTPUT);
  //
digitalWrite(motor_roll, 1);
  //digitalWrite(motor_cut, 1);
  //
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  //
  //  //Attaching the ISR to encoder A
  attachInterrupt(ENCODER_A, encoder_a, CHANGE);
  attachInterrupt(ENCODER_B, encoder_b, CHANGE);
  // Declare pins as output:


  //  EEPROM.get(0,sum_in);
  monitor_main(corsor_x,corsor_y);
}

void loop() {
  //monitor_main
  show_input_keypad();
  endcodeder_run();
  
  
  // * rotary_endcoder ให้ทำงานต่อเมือค่ามีการเปลียนแปลง
  //rotary_endcoder();

  //calcular();

}

void show_input_keypad() {
  key = keypad.getKey();  // สร้างตัวแปรชื่อ key ชนิด char เพื่อเก็บตัวอักขระที่กด


  if (key) { // ถ้าหากตัวแปร key มีอักขระ
    Serial.println(key);
    /*Keypad select mode
      A = set wire input  ระยะที่ต้องการตัด
      B = set cut wire  จำนวนที่ต้องการตัด

    */

    if (key == 'A') {
      corsor_x = 0;
      corsor_y = 2;
      status_select = "relond";

      return (monitor_main(corsor_x,corsor_y));
    }
    if (key == 'B') {
      corsor_x = 3;
      corsor_y = 3;
      
      status_select = "cut";
      return (monitor_main(corsor_x,corsor_y));
    }
     if (key == 'C') {
         
      //Serial.println("calcular_comprimento :" + String(calcular_comprimento));
      
      encoder_value = 0 ;
      //Serial.println("calcular_comprimento :" + String(calcular_comprimento));
      return (monitor_main(corsor_x,corsor_y));
    }
    if (key == '#') {
      Serial.println("Status Select:" + status_select);
      if (status_select == "relond") {
        status_set = "set_relond";
        monitor_set_relond(0, 1);
        seting_reload();

      }
      else if (status_select == "cut") {

      }
    }
  }
}

void monitor_main(int cs_pmx,int cs_pmy) {

  //EEPROM.get(0,calcular_comprimento);

  lcd.init();
lcd.setCursor(cs_pmx, cs_pmy);
  lcd.print("->");
  lcd.backlight();
  lcd.setCursor(0, 0);
  // แสดงเลือก โมดในการทำงาน
  lcd.print("    Lenght: " );
  lcd.setCursor(0, 1);
  lcd.print("Length: " + String(temp_sfu, 3) + " " + sfu);
  lcd.setCursor(2, 2);
  lcd.print("reload: " + String(set_reload) + " cm");
  lcd.setCursor(5, 3);
  lcd.print("Cut : " + String(set_cut) + " cm");
  
  

}

void auto_readcut(){
  float length_cable_now  = calcular_comprimento;
  float temp_cut = set_cut - set_first - set_reload;
  if (length_cable_now > temp_cut ){
    // motor reload rocable stop
    // motor cut cable start 3 s.
    // motor reload rocable start 
  }
  else if (length_cable_now >= set_cut ){
    // motor reload stop and end program
    
  }

}
void firter_unit(){
  float temp_calcular = calcular_comprimento ;
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

void cls_display_role_i(){
    lcd.setCursor(7, 1);
    lcd.clear();
}

void endcodeder_run() {
  rotary_endcoder();
  calcular();
  //เช็คค่าการเปลียนแปลงระหว่างการทำงานเครื่องเหมือมีการเปลียนแปลงให้แสดงค่า
  Serial.println("calcular_comprimento :" + String(calcular_comprimento)+ ":" +String(temp_endcode)); 
  if (calcular_comprimento != temp_endcode) {
    firter_unit();
    cls_display_role_i();
    //  ระบุตำแหน่งในการแสดงใน  lcd  บรรทัด 0 และ ก็เริ่มต้นที่บรรรทัด
    lcd.setCursor(0, 1);
    lcd.print("Length: " + String(temp_sfu, 3) + " " + sfu);
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

void calcular() {

  calcular_comprimento = calcular_rotary * turn;
  Serial.print("ความยาวสาย  ");
  Serial.println(calcular_comprimento,3);

}


void monitor_set_relond(int cs_x, int cs_y) {
  lcd.init();

  lcd.backlight();
  lcd.setCursor(0, 0);
  // แสดงเลือก โมดในการทำงาน
  lcd.print("    Setup : " );
  //  ระบุตำแหน่งในการแสดงใน  lcd  บรรทัด 0 และ ก็เริ่มต้นที่บรรรทัด
  lcd.setCursor(0, 1);

  lcd.print("    Length: " + String(sum_input_kb) );

  lcd.setCursor(10, 2);
  lcd.print("cm");
  lcd.setCursor(5, 3);
  lcd.print("Cancel   Ok ");
  lcd.setCursor(cs_x, cs_y);
  lcd.print("->");
  return (seting_reload());
}

void seting_reload()
{
  //Serial.println("seting_reload");
  while (true)
  {
    if (status_set == "set_relond") {


      key = keypad.getKey();  // สร้างตัวแปรชื่อ key ชนิด char เพื่อเก็บตัวอักขระที่กด

      if (key) { // ถ้าหากตัวแปร key มีอักขระ
        Serial.println(key);
        /*Keypad select mode
          A = set wire input  ระยะที่ต้องการตัด
          B = set cut wire  จำนวนที่ต้องการตัด
        */
        if (key == 'A') {
          corsor_y = 1;
          corsor_x = 0;
          return (monitor_set_relond(corsor_x, corsor_y));
        }
        if (key == 'B') {
          corsor_y = 3;
          if ( corsor_x == 0 || corsor_x == 12  ) {
            corsor_x = 2;
            select_set = "cancel";
            Serial.println("scelect set : " + select_set);
          }
          else if ( corsor_x == 2) {
            corsor_x = 12;
            select_set = "ok";
            Serial.println("scelect  set : " + select_set);
          }
          return (monitor_set_relond(corsor_x, corsor_y));
        }
        if (key == 'C') {
          sum_input_kb = "";
          return (monitor_set_relond(corsor_x, corsor_y));
        }
        if (key == '#') {
          if (select_set == "ok") {
            status_set = select_set;
            set_reload=  sum_input_kb.toFloat();
            Serial.println("status  set : " + status_set);
          }
          else if (select_set == "cancel") {
            status_set = select_set;
            Serial.println("status  set : " + status_set);
          }

        }
        if (corsor_y ==  1) {
          if (key == '*') {
            sum_input_kb = sum_input_kb + String(".");
            return (monitor_set_relond(corsor_x, corsor_y));
          }
          if (key != 'A', 'B', 'D', '#', 'C', '*') {

            sum_input_kb = sum_input_kb + String(key);
            return (monitor_set_relond(corsor_x, corsor_y));
          }

        }

      }
    }
    else if (status_set == "ok" || status_set == "cancel") {
      Serial.println("break : whlie");
      break;
    }

  }
  // return(seting_reload());
  //return(monitor_set_relond(corsor_x,corsor_y));
  Serial.println("updata : succeed");
  status_select = "";
  return (monitor_main(corsor_x,corsor_y));
}



//write function   
void monitor_set_cut(int cs_x, int cs_y) {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  // แสดงเลือก โมดในการทำงาน
  lcd.print("    Setup : " );
  //  ระบุตำแหน่งในการแสดงใน  lcd  บรรทัด 0 และ ก็เริ่มต้นที่บรรรทัด
  lcd.setCursor(0, 1);

  lcd.print("    Length: " + String(sum_input_kb) + " cm" );

  lcd.setCursor(0, 2);
  lcd.print("");
  lcd.setCursor(5, 3);
  lcd.print("Cancel   Ok ");
  lcd.setCursor(cs_x, cs_y);
  lcd.print("->");
  return (seting_reload());
}

void seting_cut()
{
  //Serial.println("seting_reload");
  while (true)
  {
    if (status_set == "set_cut") {


      key = keypad.getKey();  // สร้างตัวแปรชื่อ key ชนิด char เพื่อเก็บตัวอักขระที่กด

      if (key) { // ถ้าหากตัวแปร key มีอักขระ
        Serial.println(key);
        /*Keypad select mode
          A = set wire input  ระยะที่ต้องการตัด
          B = set cut wire  จำนวนที่ต้องการตัด
        */
        if (key == 'A') {
          corsor_y = 1;
          corsor_x = 0;
          return (monitor_set_relond(corsor_x, corsor_y));
        }
        if (key == 'B') {
          
          if (corsor_y == 1 ){
            corsor_x = 1;
            corsor_y = 2;

          } 
          else if(corsor_y == 2 || corsor_y == 3 ){ 
            corsor_y == 3;
          if ( corsor_x == 0 || corsor_x == 12  ) {
            corsor_x = 2;
            select_set = "cancel";
          }
          else if ( corsor_y == 3 || corsor_x == 2) {
            corsor_x = 12;
            select_set = "ok";
          }
          return (monitor_set_relond(corsor_x, corsor_y));
        }
        }
        if (key == 'C') {
          sum_input_kb = "";
          return (monitor_set_relond(corsor_x, corsor_y));
        }
        if (key == '#') {
          if (select_set == "ok") {
            status_set = select_set;
            set_reload=  sum_input_kb.toFloat();
            Serial.println("status  set : " + status_set);
          }
          if (select_set == "cancel") {
            status_set = select_set;
            set_reload=  sum_input_kb.toFloat();
            Serial.println("status  set : " + status_set);
          }

        }
        if (corsor_y ==  1) {
          if (key == '*') {
            sum_input_kb = sum_input_kb + String(".");
            return (monitor_set_relond(corsor_x, corsor_y));
          }
          if (key != 'A', 'B', 'D', '#', 'C', '*') {

            sum_input_kb = sum_input_kb + String(key);
            return (monitor_set_relond(corsor_x, corsor_y));
          }

        }

      }
    }
    else if (status_set == "ok" || status_set == "cancel") {
      Serial.println("break : whlie");
      break;
    }

  }

  // return(seting_reload());
  //return(monitor_set_relond(corsor_x,corsor_y));
  Serial.println("updata : succeed");
  status_select = "";
  return (monitor_main(corsor_x,corsor_y));
}
