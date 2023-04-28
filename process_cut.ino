

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
#define data_set_cut 0
#define data_set_per_cut 1
#define data_set_reload 2
byte data_cut;
byte data_per_cut;
byte data_reload;
int corsor_y = 1;
int corsor_x = 0;
int cs_pmy;
int cs_pmx;
String setting_palamitor;
String sfu; //show filter unit
String select_set; //status select mode
String status_set; // select  mode setup Reloan or cut
String status_select;
String sum_input_kb; // palamitor input key board
String sum_input_Pre;
String sum_input_cut;
String tag_satus_setup; // tagname status mode
String status_program ;
float set_reload; //set paramiter reloadcablen
float temp_sfu = set_reload ; // vlue filter para mitor
float set_cut = 50000; // set paramiter Lenghtcut cablen
float set_pre = 2200;  // set paramiter first cut
float temp_endcode;
float calcular_comprimento;
//float calcular_rotary = (2 * 3.143 * 157.5); // คำนวนความยาวที่  end code หมุน 1 รอบ
float itam_set_cut;
float itam_set_reload;
String itam_sucut;
String itam_sureload;
String  status_mode;
bool status_program_eng;
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
 // monitor_test();
 
  digitalWrite(relay_motor_lo, off);
  digitalWrite(relay_motor_cut, off);
  Serial.print("set_reload ");
  Serial.print(set_reload);
  firter_set_unit(set_reload , "reload_unit");
  firter_set_unit(set_cut , "cut_unit");
  monitor_main(5, 5);
}


void loop() {
  select_mode();
  endcodeder_run();
}

void select_mode() {

  corsor_main();
  select_setting();
  if  (status_mode  == "running") {
    Serial.println("running");
    running();

  }
  else if (status_mode == "setting") {
    setting();
  }



}

void corsor_main() {
  show_input_keypad();
  if (key == '*')
  {
    if ( setting_palamitor == "setting" ) {
      setting_palamitor = "running";
    }
    else {
      setting_palamitor = "setting";
    }
    monitor_main(0, 0) ;
  }
  if (key == '#') {
    Serial.println("Statr:" + setting_palamitor );
    if (setting_palamitor  == "running") {
      status_mode = "running";


    }
  }
   if (key == 'C') {
         
      //Serial.println("calcular_comprimento :" + String(calcular_comprimento));
      
      encoder_value = 0 ;
      //Serial.println("calcular_comprimento :" + String(calcular_comprimento));
      return (monitor_main(corsor_x,corsor_y));
    }


}

void setting() {


}
void select_setting() {
  if (setting_palamitor == "setting" ) {
    if (key == 'A') {
      corsor_x = 0;
      corsor_y = 2;
      status_select = "relond";

      return (monitor_main(corsor_x, corsor_y));
    }
    if (key == 'B') {
      corsor_x = 3;
      corsor_y = 3;

      status_select = "cut";
      return (monitor_main(corsor_x, corsor_y));
    }
    if (key == '#') {
      Serial.println("Status Select:" + status_select);
      if (status_select == "relond") {
        status_set = "set_relond";
        monitor_set_relond(0, 1);
        setting_reload();

      }
       if (key == 'C') {
         
      //Serial.println("calcular_comprimento :" + String(calcular_comprimento));
      
      encoder_value = 0 ;
      //Serial.println("calcular_comprimento :" + String(calcular_comprimento));
      return (monitor_main(corsor_x,corsor_y));
    }
      else if (status_select == "cut") {
        status_set = "set_cut";
        monitor_set_cut(0, 1);
        setting_cut();
        
        }

    else{
      
      }
    }
   

  }
}
void running() {
  status_program_eng = false;
  while (true)
  {
    show_input_keypad();
    endcodeder_run();
    auto_readcut();
    if (status_program_eng == true  || key == '*' )
    {
      status_program_eng = false;
      status_mode = "stop";
      break;
    }





  }   //monitor_test();
  Serial.println("End program runing");
}



//code end codeder
void endcodeder_run() {
  rotary_endcoder();
  calcular();
  //เช็คค่าการเปลียนแปลงระหว่างการทำงานเครื่องเหมือมีการเปลียนแปลงให้แสดงค่า
  //Serial.println("calcular_comprimento :" + String(calcular_comprimento)+ ":" +String(temp_endcode));
  if (calcular_comprimento != temp_endcode) {
    monitor_main(5,5);
    //    cls_display_role_i();
    //  ระบุตำแหน่งในการแสดงใน  lcd  บรรทัด 0 และ ก็เริ่มต้นที่บรรรทัด
    lcd.setCursor(0, 1);
    lcd.print("Length: " + String(temp_sfu, 3) + " " + sfu);
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
  float calcular_rotary = (2 * 3.142 * 32/2); // คำนวนความยาวที่  end code หมุน 1 รอบ
  calcular_comprimento = calcular_rotary * turn;
  Serial.print("ความยาวสาย  ");
  Serial.println(calcular_comprimento, 3);
}

void firter_unit() {
  float temp_calcular = calcular_comprimento + set_reload;
  if (temp_calcular < 10.000 ) {
    sfu = "mm";
    temp_sfu = temp_calcular;
  }
  else if (temp_calcular > 10.000  && temp_calcular <= 999) {
    sfu = "cm";
    temp_sfu = temp_calcular / 10;
  }
  else if (temp_calcular > 1000 ) {
    sfu = "m";
    temp_sfu =  temp_calcular / 1000;
  }
}

void firter_set_unit(float temp_set_calcular , String name_set) {
  float temp_set_sfu;
  String sfu_wait;
  if (temp_set_calcular < 10.000 ) {
    sfu_wait = "mm";
    temp_set_sfu = temp_set_calcular;
  }
  else if (temp_set_calcular > 100.000  && temp_set_calcular < 999.999) {
    sfu_wait = "cm";
    temp_set_sfu = temp_set_calcular / 10;
  }
  else if (temp_set_calcular > 1000 ) {
    sfu_wait = "m";
    temp_set_sfu =  temp_set_calcular / 1000;
  }
  if (name_set == "reload_unit") {
    itam_set_reload = temp_set_sfu;
    itam_sureload =  sfu_wait;
    Serial.print(" itam_sreload ");
    Serial.print(itam_set_reload );
  }
  else if (name_set == "cut_unit") {
    itam_set_cut = temp_set_sfu;
    itam_sucut =  sfu_wait;
    Serial.print(" itam_scut ");
    Serial.print(itam_set_cut);
  }

}


//function auto cut and clacualr ระยะสายที่ตั้งค่าไว้
void auto_readcut() {
  float length_cable_now  = calcular_comprimento;
  float setpoint_cut = set_cut - set_pre - set_reload;
  Serial.print("setpoint : " + String( setpoint_cut) );
  if (length_cable_now < setpoint_cut) {
    Serial.print("relsy motor_lo ON ");
    digitalWrite(relay_motor_lo, on);
  }
  else if (length_cable_now >= setpoint_cut && length_cable_now <= (setpoint_cut + 100) ) {
    // motor reload rocable stop
    // motor cut cable start 3 s.
    // motor reload rocable start
    Serial.print("Cutt  ");
    digitalWrite(relay_motor_lo, off);
    digitalWrite(relay_motor_cut, on);

    delay(6000);
    digitalWrite(relay_motor_cut, off);
    digitalWrite(relay_motor_lo, on);
    delay(3000);
  }
  else if (length_cable_now >= (set_cut - set_reload) ) {
    // motor reload stop and end program
    digitalWrite(relay_motor_lo, off);
    digitalWrite(relay_motor_cut, off);
    status_program_eng = true ;
  }
}

void monitor_test() {
  firter_unit();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print("Length: " + String(temp_sfu, 3) + " " + sfu);
}

// mode setting


// void display_select(){

// }


//keypad
void show_input_keypad() {
  key = keypad.getKey();  // สร้างตัวแปรชื่อ key ชนิด char เพื่อเก็บตัวอักขระที่กด
  String setting_palamitor;
  if (key) { // ถ้าหากตัวแปร key มีอักขระ
    Serial.println(key);
  }
}


void monitor_main(int cs_pmx, int cs_pmy) {
firter_unit();
  //EEPROM.get(0,calcular_comprimento);
  firter_set_unit(set_reload , "reload_unit");
   firter_set_unit(set_cut , "cut_unit");
  lcd.init();
  lcd.setCursor(cs_pmx, cs_pmy);
  lcd.print("->");
  lcd.backlight();
  lcd.setCursor(0, 0);
  // แสดงเลือก โมดในการทำงาน
  lcd.print("  Lenght: " + setting_palamitor );
  lcd.setCursor(0, 1);
  lcd.print("Length: " + String(temp_sfu, 3) + " " + sfu);
  lcd.setCursor(2, 2);
  lcd.print("reload: " + String(itam_set_reload) + itam_sureload);
  lcd.setCursor(5, 3);
  lcd.print("Cut: " + String(itam_set_cut) + itam_sucut);



}





//  monitor setreload
void monitor_set_relond(int cs_x, int cs_y) {
  lcd.init();

  lcd.backlight();
  lcd.setCursor(0, 0);
  // แสดงเลือก โมดในการทำงาน
  lcd.print(" Setup : relond(mm)"  );
  //  ระบุตำแหน่งในการแสดงใน  lcd  บรรทัด 0 และ ก็เริ่มต้นที่บรรรทัด
  lcd.setCursor(0, 1);
  lcd.print("    Length: " + String(sum_input_kb) );
  lcd.setCursor(5, 3);
  lcd.print("Cancel   Ok ");
  lcd.setCursor(cs_x, cs_y);
  lcd.print("->");
  return (setting_reload());
}

void setting_reload()
{
  //Serial.println("setting_reload");
  while (true)
  {
    if (status_set == "set_relond") {


      show_input_keypad(); // สร้างตัวแปรชื่อ key ชนิด char เพื่อเก็บตัวอักขระที่กด

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
        if (key == 'C' && corsor_y ==  1) {
          sum_input_kb = "";
          return (monitor_set_relond(corsor_x, corsor_y));
        }

        if (key == '#') {
          if (select_set == "ok") {
            status_set = select_set;
            set_reload =  sum_input_kb.toFloat();
            //EEPROM.put(data_set_reload,set_reload);
            Serial.println(set_reload);
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
  // return(setting_reload());
  //return(monitor_set_relond(corsor_x,corsor_y));
  Serial.println("updata : succeed");
  status_select = "";
  return (monitor_main(corsor_x, corsor_y));
}



// cut setting
void monitor_set_cut(int cs_x, int cs_y) {
  lcd.init();

  lcd.backlight();
  lcd.setCursor(0, 0);
  // แสดงเลือก โมดในการทำงาน
  lcd.print("   Setup : Cut"  );
  //  ระบุตำแหน่งในการแสดงใน  lcd  บรรทัด 0 และ ก็เริ่มต้นที่บรรรทัด
  lcd.setCursor(3, 1);
  lcd.print(" cut(m): " + String(sum_input_cut) );
  lcd.setCursor(3, 2);
  lcd.print("Pre(mm): " + String(sum_input_Pre));
  lcd.setCursor(5, 3);
  lcd.print("Cancel   Ok ");
  lcd.setCursor(cs_x, cs_y);
  lcd.print("->");
  return (setting_cut());
}

void setting_cut()
{
  //Serial.println("setting_reload");
  while (true)
  {
    if (status_set == "set_cut") {


      show_input_keypad(); // สร้างตัวแปรชื่อ key ชนิด char เพื่อเก็บตัวอักขระที่กด

      if (key) { // ถ้าหากตัวแปร key มีอักขระ
        Serial.println(key);
        /*Keypad select mode
          A = set wire input  ระยะที่ต้องการตัด
          B = set cut wire  จำนวนที่ต้องการตัด
        */
        if (key == 'A') {
          corsor_y = 1;
          corsor_x = 0;
          return (monitor_set_cut(corsor_x, corsor_y));
        }
        if (key == 'B') {
          if (corsor_y < 2 ){corsor_y = 2 ;}
          else if (corsor_y >=2){
            corsor_y = 3 ;
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
          }
          return (monitor_set_cut(corsor_x, corsor_y));
        }
        if (key == 'C' && corsor_y ==  1) {
          sum_input_kb = "";
          return (monitor_set_cut(corsor_x, corsor_y));
        }
        if (key == 'C' && corsor_y ==  2) {
          sum_input_Pre = "";
          return (monitor_set_cut(corsor_x, corsor_y));
        }
        if (key == '#') {
          if (select_set == "ok") {
            status_set = select_set;
            set_cut =  (sum_input_cut.toFloat()*1000);
            set_pre = sum_input_Pre.toFloat();
            EEPROM.write(data_set_cut,set_cut);
            EEPROM.write(data_set_per_cut,set_pre); 
            Serial.println(set_cut);
            Serial.println("status  set : " + status_set);
          }
          else if (select_set == "cancel") {
            status_set = select_set;
            Serial.println("status  set : " + status_set);
          }

        }
        if (corsor_y ==  1) {
          if (key == '*') {
            sum_input_cut  = sum_input_cut+ String(".");
            return (monitor_set_cut(corsor_x, corsor_y));
          }
          if (key != 'A', 'B', 'D', '#', 'C', '*') {

            sum_input_cut = sum_input_cut + String(key);
            return (monitor_set_cut(corsor_x, corsor_y));
          }

        }
        else if (corsor_y == 2) {

          if (key == '*') {
            sum_input_Pre = sum_input_Pre + String(".");
            return (monitor_set_cut(corsor_x, corsor_y));
          }
          if (key != 'A', 'B', 'D', '#', 'C', '*') {

            sum_input_Pre = sum_input_Pre + String(key);
            return (monitor_set_cut(corsor_x, corsor_y));
          }

        }


      }
    }
    else if (status_set == "ok" || status_set == "cancel") {
      Serial.println("break : whlie");
      break;
    }

  }
  // return(setting_reload());
  //return(monitor_set_relond(corsor_x,corsor_y));
  Serial.println("updata : succeed");
  status_select = "";
  return (monitor_main(corsor_x, corsor_y));
}