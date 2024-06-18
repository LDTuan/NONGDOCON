#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "3 Anh Em";  // Tên wifi của bạn
const char* password = "987654320"; // Mật khẩu wifi của bạn

const String GOOGLE_SCRIPT_URL = "https://script.google.com/macros/s/AKfycbxMF3bpN9HkeMHkY1YrbZOk2uw0bpCR3UcNoLz7u5ZWerJQVbIO9Ld9IKvZWJ5CeQNA/exec";

const int MQ3_pin = 36;
const int buzzer_pin = 14;

void setup() {
  Serial.begin(9600);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(MQ3_pin,INPUT);
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();
  // Kết nối WiFi
  connectToWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi mất kết nối. Đang kết nối lại...");
    connectToWiFi();
  }
  float alcohol_level = readAlcoholLevel();
  displayData(alcohol_level);
  
  if (alcohol_level >= 0.4) { // 0.4 mg/l là giới hạn pháp luật
    digitalWrite(buzzer_pin, HIGH);
    delay(1000);
    digitalWrite(buzzer_pin, LOW);
    String params = "&giatri1=" + String(alcohol_level, 2);
    write_to_google_sheet(params);
    delay(7000); // Chờ 7 giây trước khi đọc lại cảm biến và gửi dữ liệu
  } else {
    digitalWrite(buzzer_pin, LOW);
  }
  delay(1000);
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  int maxRetries = 10;
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
    delay(500);
    Serial.print("."); // chờ kết nối wifi 5S .........
    retries++;
  }
}

float readAlcoholLevel() {
   int sensorValue = analogRead(MQ3_pin); 
   float ppm = sensorValue*((4-0.04)/4096);
 // float ppm = (sensorValue*0.00172) - 1.9037;
  Serial.print("PPM: ");
  Serial.println(ppm);
  return ppm; // Trả về giá trị nồng độ cồn tính bằng ppm
}

void displayData(float alcohol_level) {
  display.clearDisplay();
  display.setTextSize(2); // Kích thước chữ nhỏ hơn để hiển thị nhiều nội dung hơn
  display.setTextColor(WHITE);
  display.setCursor(15, 15); // Di chuyển con trỏ đến dòng 15,15
  display.print(alcohol_level, 2); // Hiển thị 2 chữ số thập phân
  display.print(" mg/L");
  display.display(); // Cập nhật màn hình với dữ liệu mới
}

void write_to_google_sheet(String params) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http; // tạo đối tượng HTTPclients đặt tên là http  
    String url = GOOGLE_SCRIPT_URL + "?" + params; // tham số param
    Serial.println("Đang gửi dữ liệu tới Google Sheet: " + url); // In URL ra để kiểm tra
    http.begin(url.c_str()); // khởi tạo với ulr chuyển sang string
    //http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); 
    int httpCode = http.GET();// Gửi yêu cầu HTTP GET tới URL và lưu mã trạng thái HTTP trả về trong biến httpCode.
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    if (httpCode > 0) {
      String payload = http.getString(); // mã trạng thái HTTP lớn hơn 0 (tức là có phản hồi từ server) thì dữ liệu đã gửi thành công
      Serial.println("Payload: " + payload);
    } else {
      Serial.println("Lỗi khi gửi yêu cầu HTTP");
    }
    http.end();
  } else {
    Serial.println("Không thể gửi dữ liệu, WiFi không được kết nối");
  }
}