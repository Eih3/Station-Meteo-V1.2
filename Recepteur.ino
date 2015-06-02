#include <dht.h>
#include <VirtualWire.h>

dht DHT;

#define DHT11_PIN 8
#define LED       2
#define TX_PIN    12

/**** Fonction configuration*/
void setup()
{
  Serial.begin(9600);
  Serial.println("Station Meteo Arduino Recepteur");
  
  pinMode(LED, OUTPUT);

  vw_set_tx_pin(TX_PIN); 
  vw_setup(2000); 
}

void loop()
{
  int chk = DHT.read11(DHT11_PIN); //Lecture des données DHT11
  
  // Envoi de la trame temp + hum ex: 24.0050.00
  EnvoiDonnees((String)DHT.temperature + ((String)DHT.humidity));
  
  Serial.println("Temperature : " + (String)DHT.temperature + " oC");    
  Serial.println("Humidite : " + (String)DHT.humidity + " %");    
  Serial.println("\n"); 
  
  delay(2000);
}

/**** Fonction envoi des données DHT11 ****/
void EnvoiDonnees(String donnees)
{
  const char* data = donnees.c_str();
  
  digitalWrite(LED, HIGH);

  vw_send((uint8_t *)data, strlen(data));
  vw_wait_tx();
  
  digitalWrite(LED, LOW);
}
