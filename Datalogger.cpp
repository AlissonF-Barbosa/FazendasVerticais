#include "DHT.h"
Biblioteca para o sensor

#include <Wire.h>
Biblioteca para o display

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LARGURA 128
#define ALTURA 64

Adafruit_SSD1306 display(LARGURA, ALTURA, &Wire, -1);


#define SensorDHT 4    

#define DHTTYPE DHT22

const int ilum = 18;
const int irrig = 2;

float umid = 5;

unsigned long previousMillis = 0;


const long interval = 30000; // intervalo em milissegundos

int cont_irrig;
int cont_ilum;

#define siclos_ilum 2
#define siclos_irrig 1

int rele_aquec = 15, rele_refrig = 27;

int acido = 23, base = 19;


DHT dht(SensorDHT, DHTTYPE);


// Usar no sensor de pH
//float valor_calibracao = 20.24; // 21.34 - 0.7

// Substituir no potenciometro para teste
float valor_calibracao = 14.0;
#define valor_ph 0
unsigned long int valor_medio;
int valores_buffer[10], temp;

float ph_atual;

const int potenciometro = 35;

void aquec_refrig()
{

  // lê a temperatura
  float temp = dht.readTemperature();
 
  if(isnan(temp))
  {
    Serial.println("A leitura falhou");
    return;
  }

  //indice de calor	
  float hic = dht.computeHeatIndex(temp, false);

  digitalRead(rele_aquec);
  digitalRead(rele_refrig);

  if(temp < 18.0)
  {
    digitalWrite(rele_aquec, HIGH);
    digitalWrite(rele_refrig, LOW);

  }
  else if(temp > 18.0 && temp < 24.0)
  {
   digitalWrite(rele_refrig, LOW);
   digitalWrite(rele_aquec, LOW);
  }
  else if(temp > 24.0)
  {
   
   digitalWrite(rele_refrig, HIGH);
   digitalWrite(rele_aquec, LOW);
 
  }
 

  Serial.print("Temperatura:");
  Serial.print(temp);
  Serial.print("°C  ");

}


void umidade()
{
  
  float hum = dht.readHumidity();
 
  if(isnan(hum))
  {
    Serial.println("Falha na leitura");
    return;

  }

  float hic = dht.computeHeatIndex(hum, false);
 
  digitalRead(hum);

  if(hum < 65.0)
  {
    digitalWrite(umid, HIGH);
  }
  else if(hum > 78.0)
  {
    digitalWrite(umid, LOW);
  }
 
  Serial.print("Umidade:");
  Serial.print(hum);
  Serial.println("%");

}



void ilum_irrig()
{

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    cont_irrig++;
    cont_ilum++;

  }

 if(cont_ilum == siclos_ilum)
  {

    if(digitalRead(ilum) == LOW)
    {
      digitalWrite(ilum, HIGH);
    }else
    {
      digitalWrite(ilum, LOW);
    }
 
    cont_ilum = 0;

  }

  if(cont_irrig == siclos_irrig)
  {
   
    if(digitalRead(irrig)== LOW)
    {
      digitalWrite(irrig, HIGH);
    }else
    {
      digitalWrite(irrig, LOW);
    }

    cont_irrig = 0;

  }

}




void sensor_ph()
{
  for(int i=0;i<10;i++)
  {
    valores_buffer[i]=analogRead(35);
    delay(30);
  }

  for(int i=0;i<9;i++)
  {
    for(int j=i+1;j<10;j++)
    {
      if(valores_buffer[i]>valores_buffer[j])
      {
        temp=valores_buffer[i];
        valores_buffer[i]=valores_buffer[j];
        valores_buffer[j]=temp;
      }
    }
  }

  valor_medio=0;
  for(int i=2;i<8;i++)
  valor_medio+=valores_buffer[i];

  // Usar no sensor de pH
  //float tensao=(float)valor_medio*3.3/4096.0/6;
 
  // Substituir no potenciometro para teste
  float tensao = valor_medio/6.0;  
  //Serial.print("Tensao: ");
  //Serial.println(tensao);

  // Usar no sensor de pH
  //ph_atual = -5.70 * tensao + valor_calibracao;
 
  // Substituir no potenciometro para teste
  ph_atual = (tensao / 4095.0) * valor_calibracao;
 
  // mostra o valor do pH 
  Serial.print("pH: ");
  Serial.print(ph_atual);



  if(ph_atual < 5.5)
  {
    Serial.println("  Add Base");
    digitalWrite(base, HIGH);
    digitalWrite(acido, LOW);
  }
  else if(ph_atual > 5.5 && ph_atual < 6.5)
  {
    Serial.println("  pH Ideal");
    digitalWrite(acido, LOW);
    digitalWrite(base, LOW);
  }
  else if(ph_atual > 6.5)
  {
    Serial.println("  Add Acido");
    digitalWrite(acido, HIGH);
    digitalWrite(base, LOW);
  }


  delay(1000);

}



void setup()
{
  dht.begin();

  Wire.begin();
  Serial.begin(115000);
  Serial.println("Teste DHT22");

  pinMode(potenciometro, INPUT);

  pinMode(SensorDHT, INPUT);

  pinMode(rele_aquec, OUTPUT);
  pinMode(rele_refrig, OUTPUT);
  pinMode(umid, OUTPUT);

  pinMode(ilum, OUTPUT);
  digitalWrite(ilum, HIGH);
  pinMode(irrig, OUTPUT);
  digitalWrite(irrig, HIGH);

  pinMode(acido, OUTPUT);
  pinMode(base, OUTPUT);

 
 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("Erro ao iniciar o display"));
    for(;;);
  }
}


void loop()
{

 

  ilum_irrig();
 
  aquec_refrig();

  umidade();

  // chama a função do sensor de pH
  sensor_ph();




  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("GreenFarms");
  display.display();


  delay(2000);


 
}
