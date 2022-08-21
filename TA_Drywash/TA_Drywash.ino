#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define waterlvl_pin A0
#define buzzer D0
#define triggerPin  D7
#define echoPin     D8
#define relay1 D4
#define relay2 D6
#define relay3 D5

#define WIFI_SSID "drywash"
#define WIFI_PASS "tasukses"

int kunci = 0 ;
int level;
String kondsRelay;
const char *host = "monitoringair.allbright.my.id";



void setup() {
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  Serial.begin (9600);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(waterlvl_pin, INPUT);
  pinMode(buzzer, OUTPUT);
  lcd.init();
  lcd.backlight();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  lcd.setCursor(0, 0);
  lcd.print("Drywash Machine");
  lcd.setCursor(0, 1);
  lcd.print("Telekomunikasi");
  delay(3000);
  lcd.clear();

}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Silakan Masukan Anda");
  //  delay(1000);
  //  lcd.clear();
  digitalWrite(buzzer, LOW);
  bacaWater();
  //  getNyala();
  //  cekRelay();
  sendDataPhp(String(level));
  long duration, jarak;
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  jarak = (duration / 2) / 29.1;
  Serial.println("jarak :");
  Serial.print(jarak);
  //  Serial.println(" cm");
  //  delay(1000);
  //  digitalWrite(relay1, HIGH);
  //  digitalWrite(relay2, HIGH);
  //  digitalWrite(relay3, HIGH);

  if (jarak <= 12 && kunci == 0) {
    kunci = 1;
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
    digitalWrite(relay3, HIGH);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Steril Mulai");

  }

  else if (jarak > 20 && kunci == 1) {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Proses");
    lcd.setCursor(0, 1);
    lcd.print("Pembuangan");
    digitalWrite(buzzer, HIGH);
    delay(5000);
    lcd.clear();
    kunci = 0;
  }

  else if (jarak > 20 && kunci == 0) {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, HIGH);
  }
}

void bacaWater() {
  level = analogRead(waterlvl_pin);
  level = map(level, 0, 1023, 0, 100); // di map in untuk 100 %
  //  Serial.print("Water = ");
  //  Serial.println(level);

}


void cekRelay() {
  if (kondsRelay != "") {
    if (kondsRelay == "0") digitalWrite(relay3, HIGH);
    if (kondsRelay == "1") digitalWrite(relay3, LOW);
  } else {
    return;
  }
}


//void getNyala() {
//  WiFiClient clientNyala;
//  if (!clientNyala.connect(host, 80)) {
//    Serial.println("connection nyala failed");
//    return;
//  }
//  String url;
//  url =  "http://" + String(host) + "/cek/cek_relay.php";
//  HTTPClient httpNyala;
//  httpNyala.begin(clientNyala, url);
//
//  httpNyala.GET();
//  String responNyala = httpNyala.getString();
//  Serial.println("responNyala = " + responNyala);
//  httpNyala.end();
//  kondsRelay = responNyala;
//}


void sendDataPhp(String level) {
  WiFiClient client;
  if (!client.connect(host, 80)) {
    Serial.println("connection failed");
    return;
  }
  String url;
  url = "http://" + String(host) + "/kirimdata.php?level=" + level ;

  HTTPClient http;
  http.begin(client, url);

  http.GET();
  String respon = http.getString();
  Serial.println(respon);
  http.end();
}
