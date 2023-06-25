#define BLYNK_TEMPLATE_ID "TMPL6TaoEHTUC"
#define BLYNK_TEMPLATE_NAME "doan1"
#define BLYNK_AUTH_TOKEN "BlCEDFbf8Ut9u_Mv1AKVSjTUFOGumBzV"
#define BLYNK_PRINT Serial

#include <WiFi.h>					//Khai báo các thư viện cần thiết .
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "BlCEDFbf8Ut9u_Mv1AKVSjTUFOGumBzV";    //Khai báo mã Blynk và kết nối với wifi.
char ssid[] = "NGHIEN";
char pass[] = "deochodauhehe";


#define motor 2         //Khai báo các biến.    
#define humi 4  
#define buttonPin 5
#define modeButtonPin 16

BlynkTimer timer;

bool isAutoMode = true; // true: auto mode, false: manual mode

void handleAutoButtonPress() {
  int buttonState = digitalRead(modeButtonPin);
  if (buttonState == LOW) {
    isAutoMode = !isAutoMode;
    delay(200);
  }
}

bool isPumpOn = false; // Biến trạng thái của máy bơm (true: bật, false: tắt)

void handleButtonPress() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    // Nếu nút GPIO 5 được nhấn thì đảo trạng thái của máy bơm
    if (digitalRead(buttonPin) == LOW) {
      isPumpOn = !isPumpOn;
      delay(200);
    }
  }
}

void sendSensor() {
  int m;
  int h = digitalRead(humi);
  handleButtonPress(); // Xử lý nhấn nút
  handleAutoButtonPress(); // Xử lý chọn chế độ auto hoặc manual
  Serial.println(h);
  if (isAutoMode) { // Nếu đang ở chế độ auto
    if (h == 1) { // Bật máy bơm nếu độ ẩm > 50%
      digitalWrite(motor, HIGH);
      lcd.setCursor(2, 0);
      lcd.print("Mode: Auto  ");
      lcd.setCursor(2, 1); 
      lcd.print("May bom: ON   "); 
      Serial.println("May bom: ON");
      m=1;
    } else { // Tắt máy bơm nếu độ ẩm <= 50%
      digitalWrite(motor, LOW);
      lcd.setCursor(2, 0);
      lcd.print("Mode: Auto  ");    
      lcd.setCursor(2, 1);
      lcd.print("May bom: OFF  ");
      Serial.println("May bom: OFF");
      m=0;
    }
  } else { // Nếu đang ở chế độ manual
    if (isPumpOn) { // Bật máy bơm nếu trạng thái của nút là true
      digitalWrite(motor, HIGH);
      lcd.setCursor(2, 0);
      lcd.print("Mode: Manual");
      lcd.setCursor(2, 1); 
      lcd.print("May bom: ON   "); 
      Serial.println("May bom: ON");
      m=1;
    } else { // Tắt máy bơm nếu trạng thái của nút là false
      digitalWrite(motor, LOW);
      lcd.setCursor(2, 0);
      lcd.print("Mode: Manual");    
      lcd.setCursor(2, 1);
      lcd.print("May bom: OFF  ");
      Serial.println("May bom: OFF");
      m=0;
    }
  }
  Blynk.virtualWrite(V1, isAutoMode);
  Blynk.virtualWrite(V3, m);
  Blynk.virtualWrite(V5, isPumpOn);
}

BLYNK_WRITE(V1) { // Xử lý sự kiện khi người dùng thay đổi chế độ hoạt động
  isAutoMode = param.asInt();

}

BLYNK_WRITE(V5) { // Xử lý sự kiện khi người dùng thay đổi trạng thái máy bơm
  isPumpOn = param.asInt();

}

void setup() {
  Serial.begin(9600);
  pinMode(motor, OUTPUT);
  pinMode(humi, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(modeButtonPin, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Smart Garden");
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1L, sendSensor);
  Blynk.syncAll(); // Đồng bộ hóa trạng thái của các bộ điều khiển với Blynk server
}

void loop() {           //Chạy vòng lặp để thực thi các hàm.
  Blynk.run();
  timer.run(); 
}