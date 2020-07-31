/*ESEMPIO IEC60870-5-104 SLAVE*/

#include <WiFi.h>
#include <IEC60870-5-104.h>

//Creo l'istanza SERVER IEC60870-5-104
WiFiServer srv(2404); //Creo il server
IEC104_SLAVE slave(&srv);

//Memoria per timer
unsigned long lastSend=0;

//Setup - viene eseguito all'avvio
void setup()
{
  //Imposto la comunicazione seriale
  Serial.begin(115200);

  //Imposto la scheda di rete
  //WiFi.config(local_IP, gtw_IP, sub_IP);
  WiFi.mode(WIFI_STA); //Imposto il modulo Wi-Fi come client
  WiFi.begin("SSID","password"); //Connetto alla rete
}

//Loop - Ciclo infinito su CORE 1 
void loop()
{
  //------------------------------------------------------------RICEZIONE DI UN COMANDO SP o DP------------------------------------------------------------//
  
  //Con la funzione available aggiorno il server e ottengo in uscita il numero di telegrammi in attesa di essere letti
  int messages = slave.available();

  //Eseguo un ciclo for per leggere tutti i messaggi nel buffer
  for(int i=0; i<messages; i++)
  {
    //Creo delle variabili di appoggio per leggere il buffer
    byte type = 0; int ca = 0; long ioa = 0; long value = 0;

    //Eseguo la lettura di un array nel buffer
    slave.read(&type,&ca,&ioa,&value);

    //Utilizzo il dato ricevuto
    if(type==C_SC_NA_1) Serial.println("Hai ricevuto un Single Point Command da C.A.:"+String(ca)+"-IOA:"+String(ioa)+" uguale a "+String(value));
    else if(type==C_DC_NA_1) Serial.println("Hai ricevuto un Double Point Command da C.A.:"+String(ca)+"-IOA:"+String(ioa)+" uguale a "+String(value));
  }

  //---------------------------------------------------------------INVIO DI UNA MISURA SP o ME---------------------------------------------------------------//

    //Scelgo il mio Common Address
    int ca = 50;

    //Creo quattro variabili da inviare, due booleane (Single Point), e due analogiche (Normalized Measure)
    bool val1 = false;
    bool val2 = true;
    int val3 = 500;
    int val4 = -500;

    //Scalo i valori per renderli normalizzati
    long val3n = map(500,0,1000,0,32767); 
    long val4n = map(-500,-1000,1000,-32767,32767);

    //Posso inviare più oggetti dello stesso tipo in un unico telegramma
    
    long IOA_SP[2] = {10, 11}; //Inserisco gli IOA che userò per i single point
    long IOA_ME[2] = {20, 21}; //Inserisco gli IOA che userò per le misure
    
    long VAL_SP[2] = {val1, val2}; //Inserisco i valori
    long VAL_ME[2] = {val3n, val4n}; //Inserisco i valori

    //Se sono trascorsi 5" dall'ultimo invio, reinvio la misura
    if(millis()-lastSend>5000)
    {
      //Eseguo l'invio dei Measured Normalized Value
      int quantitySP = 2; //Numero di oggetti da inviare
      slave.send(M_ME_NA_1,quantitySP,ca,IOA_SP,VAL_SP);

      //Eseguo l'invio dei Single Point Information
      int quantityME = 2; //Numero di oggetti da inviare
      slave.send(M_SP_NA_1,quantityME,ca,IOA_ME,VAL_ME);

      //Aggiorno la memoria
      lastSend=millis();
    }
}
