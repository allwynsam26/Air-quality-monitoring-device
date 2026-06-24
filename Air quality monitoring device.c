#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ---------------------- PINS ----------------------
#define DHTPIN 2
#define DHTTYPE DHT11

#define MQ135 A0

#define GREEN_LED A1
#define YELLOW_LED A2
#define RED_LED A3

#define BUZZER 8

// ---------------------- OBJECTS ----------------------
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------------- THRESHOLDS ----------------------
// Adjust after observing your MQ135 values

#define GOOD_MAX      150
#define MODERATE_MAX  300

// ---------------------- FUNCTIONS ----------------------

void startupBuzzerTest()
{
  tone(BUZZER, 1000);
  delay(150);

  tone(BUZZER, 1500);
  delay(150);

  tone(BUZZER, 2000);
  delay(150);

  noTone(BUZZER);
}

void moderateAlert()
{
  // Double Beep

  tone(BUZZER, 2000);
  delay(150);

  noTone(BUZZER);
  delay(150);

  tone(BUZZER, 2000);
  delay(150);

  noTone(BUZZER);
}

void emergencySiren()
{
  // Ambulance Style Siren

  for(int x = 0; x < 2; x++)
  {
    for(int f = 1200; f <= 2500; f += 100)
    {
      tone(BUZZER, f);
      delay(10);
    }

    for(int f = 2500; f >= 1200; f -= 100)
    {
      tone(BUZZER, f);
      delay(10);
    }
  }

  noTone(BUZZER);
}

// ---------------------- SETUP ----------------------

void setup()
{
  Serial.begin(9600);

  dht.begin();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Air Quality");
  lcd.setCursor(0,1);
  lcd.print("Monitoring");

  startupBuzzerTest();

  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("MQ135 Warm Up");
  lcd.setCursor(0,1);
  lcd.print("Wait 10 Sec");

  Serial.println("================================");
  Serial.println(" AIR QUALITY MONITOR ");
  Serial.println("================================");

  delay(10000);
}

// ---------------------- LOOP ----------------------

void loop()
{
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum))
  {
    Serial.println("DHT11 Error");
    return;
  }

  // ---------- MQ135 Averaging ----------

  long total = 0;

  for(int i = 0; i < 20; i++)
  {
    total += analogRead(MQ135);
    delay(10);
  }

  int gasValue = total / 20;

  String status;

  // ---------- Reset Outputs ----------

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // ---------- Air Quality Logic ----------

  if(gasValue <= GOOD_MAX)
  {
    status = "GOOD";

    digitalWrite(GREEN_LED, HIGH);

    noTone(BUZZER);
  }
  else if(gasValue <= MODERATE_MAX)
  {
    status = "MODERATE";

    digitalWrite(YELLOW_LED, HIGH);

    moderateAlert();
  }
  else
  {
    status = "CRITICAL";

    digitalWrite(RED_LED, HIGH);

    emergencySiren();
  }

  // ---------- SERIAL MONITOR ----------

  Serial.println("--------------------------------");

  Serial.print("Temperature : ");
  Serial.print(temp,1);
  Serial.println(" C");

  Serial.print("Humidity    : ");
  Serial.print(hum,1);
  Serial.println(" %");

  Serial.print("Gas Value   : ");
  Serial.println(gasValue);

  Serial.print("Status      : ");
  Serial.println(status);

  // ---------- LCD SCREEN 1 ----------

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(temp,1);
  lcd.print("C");

  lcd.setCursor(9,0);
  lcd.print("H:");
  lcd.print((int)hum);
  lcd.print("%");

  lcd.setCursor(0,1);
  lcd.print("Gas:");
  lcd.print(gasValue);

  delay(2000);

  // ---------- LCD SCREEN 2 ----------

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("AIR QUALITY");

  lcd.setCursor(0,1);

  if(status == "GOOD")
  {
    lcd.print("GOOD");
  }
  else if(status == "MODERATE")
  {
    lcd.print("MODERATE");
  }
  else
  {
    lcd.print("CRITICAL");
  }

  delay(2000);
}