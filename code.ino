#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

const int PINO_BOTAO = 2;
const int PINO_TRIG = 3;
const int PINO_ECHO = 4;
const int PINO_LED_VERDE = 5;
const int PINO_LED_AMARELO = 6;
const int PINO_LED_VERMELHO = 7;
const int PINO_BUZZER = 8;
const int PINO_SERVO = 9;

const int PINO_POT_CHUVA = A0;
const int PINO_POT_UMIDADE = A1;
const int PINO_POT_INCLINACAO = A2;

LiquidCrystal_I2C lcd(0x20, 16, 2); 
Servo cancelaServo;

unsigned long tempoAnterior = 0;
bool estadoPisca = false;

void configurarPinos();
void iniciarLCD();
void iniciarServo();
float lerNivelAgua();
int lerChuva();
int lerUmidadeSolo();
int lerInclinacao();
float calcularRisco(float nivel, int chuva, int umidade, int inclinacao);
String classificarEstado(float risco);
void atualizarLeds(String estado);
void atualizarBuzzer(String estado);
void atualizarLCD(String estado, float risco);
void atualizarServo(String estado);

void setup() {
  Serial.begin(9600);
  configurarPinos();
  iniciarLCD();
  iniciarServo();
}

void loop() {
  if (digitalRead(PINO_BOTAO) == LOW) {
    Serial.println("Reset/Confirmação acionado.");
    delay(500);
  }

  float nivel = lerNivelAgua();
  int chuva = lerChuva();
  int umidade = lerUmidadeSolo();
  int inclinacao = lerInclinacao();

  float risco = calcularRisco(nivel, chuva, umidade, inclinacao);
  String estado = classificarEstado(risco);

  if (millis() - tempoAnterior >= 500) {
    estadoPisca = !estadoPisca;
    tempoAnterior = millis();
  }

  atualizarLeds(estado);
  atualizarBuzzer(estado);
  atualizarLCD(estado, risco);
  atualizarServo(estado);

  delay(100);
}

void configurarPinos() {
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);
  
  pinMode(PINO_LED_VERDE, OUTPUT);
  pinMode(PINO_LED_AMARELO, OUTPUT);
  pinMode(PINO_LED_VERMELHO, OUTPUT);
  pinMode(PINO_BUZZER, OUTPUT);
}

void iniciarLCD() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ChuvaViva");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
}

void iniciarServo() {
  cancelaServo.attach(PINO_SERVO);
  cancelaServo.write(0); 
}

float lerNivelAgua() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);
  
  long duracao = pulseIn(PINO_ECHO, HIGH);
  float distanciaCm = duracao * 0.034 / 2;
  float riscoNivel = map(distanciaCm, 5, 100, 100, 0);
  riscoNivel = constrain(riscoNivel, 0, 100);
  return riscoNivel;
}

int lerChuva() {
  int valor = analogRead(PINO_POT_CHUVA);
  return map(valor, 0, 1023, 0, 100);
}

int lerUmidadeSolo() {
  int valor = analogRead(PINO_POT_UMIDADE);
  return map(valor, 0, 1023, 0, 100);
}

int lerInclinacao() {
  int valor = analogRead(PINO_POT_INCLINACAO);
  return map(valor, 0, 1023, 0, 100);
}

float calcularRisco(float nivel, int chuva, int umidade, int inclinacao) {
  return (nivel * 0.40) + (chuva * 0.25) + (umidade * 0.25) + (inclinacao * 0.10);
}

String classificarEstado(float risco) {
  if (risco < 30) return "BAIXO";
  if (risco < 55) return "ATENCAO";
  if (risco < 75) return "ALERTA";
  return "CRITICO";
}

void atualizarLeds(String estado) {
  digitalWrite(PINO_LED_VERDE, LOW);
  digitalWrite(PINO_LED_AMARELO, LOW);
  digitalWrite(PINO_LED_VERMELHO, LOW);

  if (estado == "BAIXO") {
    digitalWrite(PINO_LED_VERDE, HIGH);
  } 
  else if (estado == "ATENCAO") {
    digitalWrite(PINO_LED_AMARELO, HIGH);
  } 
  else if (estado == "ALERTA") {
    digitalWrite(PINO_LED_VERMELHO, estadoPisca ? HIGH : LOW);
  } 
  else if (estado == "CRITICO") {
    digitalWrite(PINO_LED_VERMELHO, HIGH);
  }
}

void atualizarBuzzer(String estado) {
  if (estado == "ALERTA") {
    if (estadoPisca) tone(PINO_BUZZER, 1000);
    else noTone(PINO_BUZZER);
  } 
  else if (estado == "CRITICO") {
    tone(PINO_BUZZER, 1500);
  } 
  else {
    noTone(PINO_BUZZER);
  }
}

void atualizarLCD(String estado, float risco) {
  static String estadoAnterior = "";
  if (estado != estadoAnterior) {
    lcd.clear();
    lcd.setCursor(0, 0);
    
    if (estado == "BAIXO") {
      lcd.print("RISCO BAIXO");
      lcd.setCursor(0, 1);
      lcd.print("Monitorando...");
    } 
    else if (estado == "ATENCAO") {
      lcd.print("ATENCAO");
      lcd.setCursor(0, 1);
      lcd.print("Acompanhe o nivel");
    } 
    else if (estado == "ALERTA") {
      lcd.print("ALERTA ENCHENTE");
      lcd.setCursor(0, 1);
      lcd.print("Evite a area");
    } 
    else if (estado == "CRITICO") {
      lcd.print("RISCO CRITICO");
      lcd.setCursor(0, 1);
      lcd.print("SAIA DA AREA");
    }
    estadoAnterior = estado;
  }
}

void atualizarServo(String estado) {
  if (estado == "CRITICO") {
    cancelaServo.write(90);
  } else {
    cancelaServo.write(0);
  }
}
