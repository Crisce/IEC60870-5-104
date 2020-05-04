/*=============================================================================|
|  PROJECT IEC60870-5-104 for Arduino                                    1.0.0 |
|==============================================================================|
|  Copyright (C) 2020 Michele Criscenzo                                        |
|  All rights reserved.                                                        |
|==============================================================================|
|  IEC60870-5-104 for Arduino is free software: you can redistribute it and/or |
|  modify it under the terms of the Lesser GNU General Public License as       |
|  published by the Free Software Foundation, either version 3 of the License, |
|  or (at your option) any later version.                                      |
|                                                                              |
|  It means that you can distribute your commercial software linked with       |
|  IEC60870-5-104 for Arduino without the requirement to distribute the        |
|  source code of your application and without the requirement that your       |
|  application be itself distributed under LGPL.                               |
|                                                                              |
|  IEC60870-5-104 for Arduino is distributed in the hope that it will be       |
|  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of      |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                        |
|  See the Lesser GNU General Public License for more details.                 |
|=============================================================================*/

#include "IEC60870-5-104.h"

IEC104_SLAVE::IEC104_SLAVE(WiFiServer *srv)
{
  //if(wired) *iecServer = new EthernetServer(port);
  //else 
  iecServer = &*srv;
  //iecMaster = new WiFiClient;
  //*iecServer.begin();
}

IEC104_SLAVE::~IEC104_SLAVE()
{}

void IEC104_SLAVE::read(byte *type, int *ca, long *ioa, long *value)
{
  for(int i=0; i<MAX_SRV_CLIENTS; i++) connection[0].read(&*type, &*ca, &*ioa, &*value);
}

//IEC 60870-5-104 SLAVE (SERVER)

int IEC104_SLAVE::available()
{
  if(!avvio)
  {
    iecServer->begin();
    avvio=true;
  }
  uint8_t i;
  if (iecServer->hasClient())
  //if (*iecServerW.available() > 0)
  {
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      //find free/disconnected spot
      if (!iecMaster[i] || !iecMaster[i].connected())
      {
        if (iecMaster[i]) iecMaster[i].stop();
        iecMaster[i] = iecServer->available();
        if (!iecMaster[i]) Serial.println("available broken");
        else connection[i].setClient(&iecMaster[i]);
        break;
      }
    }
    if (i >= MAX_SRV_CLIENTS) {
      //no free/disconnected spot so reject
      iecServer->available().stop();
    }
  }
  int risultato=0;
  //check clients for data
  for (i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (iecMaster[i] && iecMaster[i].connected())
    {
      risultato += connection[i].check(&iecMaster[i]);
    }
    else {
      if (iecMaster[i]) {
        iecMaster[i].stop();
      }
    }
  }
  return risultato;
}

int IEC104_SLAVE::availableWiFi(WiFiServer iecServer)
{
  int result=0;
  //if(wired) {}//result=startWired(2404);
  //else result=
  //availableWifi(*iecServer);
}

void IEC104_SLAVE::send(byte type, byte num, int ca, long* IOA, long* val) //Invio le misure verso il client (master)
{
  for(int i=0; i<MAX_SRV_CLIENTS; i++)
  {
    connection[i].send(type, num, ca, IOA, val);
  }
}

IEC104_HELPER::IEC104_HELPER()
{
  //client2=cli;
}

void IEC104_HELPER::send(byte type, byte num, int ca, long* IOA, long* val)
{
  if(client0 && dataTransfer)
  {
    int dim = 1;
    if (type == 9) dim = 3;
    int msgLength = 12 + (num * (3 + dim));
    int byteCount = 12;
    byte bufferOut[msgLength];
    bufferOut[0] = 0x68; //Iniziatore
    bufferOut[1] = 10 + (num * (3 + dim)); //Lunghezza
    bufferOut[2] = (byte)(sequenceTx << 1);
    bufferOut[3] = (byte)(sequenceTx >> 7);
    bufferOut[4] = (byte)(sequenceRx << 1);
    bufferOut[5] = (byte)(sequenceRx >> 7);
    bufferOut[6] = type; //Type
    bufferOut[7] = num; //Numero di misure
    bufferOut[8] = 0x03; //Cause of Transmission
    bufferOut[9] = 0x00;
    bufferOut[10] = (byte)(ca); //Address
    bufferOut[11] = (byte)(ca >> 8); //Address
    for (int x = 0; x < num; x++)
    {
      bufferOut[byteCount + 0] = (byte)(IOA[x]); //IOA
      bufferOut[byteCount + 1] = (byte)(IOA[x] >> 8); //IOA
      bufferOut[byteCount + 2] = (byte)(IOA[x] >> 16); //IOA
      byteCount = byteCount + 3;
      for (int j = 0; j < dim; j++)
      {
        bufferOut[byteCount] = (byte)(val[x] >> (8 * j)); //Valore
        byteCount++;
      }
    }
    if(client0->connected()) for(int x=0; x<msgLength; x++) client0->write(bufferOut[x]);
    sequenceTx++;
  }
}

int IEC104_HELPER::check(Client *client)
{
  int risultato=0;
  if (client->available())
  {
    if (client->read() == 0x68) //Iniziatore di stringa
    {
      byte lung = client->read();
      byte bufferIn[64];
      for (int x = 0; x < lung; x++) bufferIn[x] = client->read();
      risultato = elaboraBuffer(bufferIn,lung,client); //Analizzo i dati appena ricevuti
    }
  }
  return risultato;
}

int IEC104_HELPER::elaboraBuffer(byte* bufferIn, byte lunghezza, Client *client)
{
  if (!bitRead(bufferIn[0], 0)) //I-FORMAT
  {
    sequenceRx++; //Incremento il contatore dei messaggi ricevuti
    byte type = bufferIn[4];
    int ca = (int)bufferIn[8] | bufferIn[9] << 8;
    byte count = 0;
    for (int i = 0; i < 7; i++) bitWrite(count, i, bitRead(bufferIn[5], i)); //Numero di IOA in stringa (primi 7 bit)
    bool SQ = bitRead(bufferIn[5], 7);
    byte cot = 0;
    for (int i = 0; i < 6; i++) bitWrite(cot, i, bitRead(bufferIn[6], i)); //Cause Of Transmission - Primi 6 bit
    bool PN = bitRead(bufferIn[6], 6);
    bool T = bitRead(bufferIn[6], 7);

    //M_SP_NA_1 Single Point information //C_SC_NA_1 Single command //C_DC_NA_1 Double command
    if (type == M_SP_NA_1 || type == C_SC_NA_1 || type == C_DC_NA_1)
    {
      for (int h = 0; h < count; h++)
      {
        long ioa = bufferIn[10 + (h * 4)] | (int)bufferIn[11 + (h * 4)] << 8 | (long)bufferIn[12 + (h * 4)] << 16;
        long val = bufferIn[13 + (h * 4)];      
        bufferTag[bufferCount][0] = type;
        bufferTag[bufferCount][1] = ca;
        bufferTag[bufferCount][2] = ioa;
        bufferTag[bufferCount][3] = val;
        bufferCount++;
      }
      inviaS(client); //S-FORMAT
    }
    else if (type == 0x09) //M_ME_NA_1 Measured value, normalised value
    {
      for (int h = 0; h < count; h++)
      {
        long ioa = bufferIn[10 + (h * 6)] | (int)bufferIn[11 + (h * 6)] << 8 | (long)bufferIn[12 + (h * 6)] << 16;
        long val = bufferIn[13 + (h * 6)] | (int)bufferIn[14 + (h * 6)] << 8 | (long)bufferIn[15 + (h * 6)] << 16;      
        bufferTag[bufferCount][0] = type;
        bufferTag[bufferCount][1] = ca;
        bufferTag[bufferCount][2] = ioa;
        bufferTag[bufferCount][3] = val;
        bufferCount++;
      }
      inviaS(client); //S-FORMAT
    }
    else if (type == 0x64) //C_IC_NA_1 Interrogation command
    {
      byte msg[] = {0x00, 0x64, 0x01, 0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14};
      inviaI(client, msg);
      Serial.println("Interrogation command");
    }
    else if (type == 0x65) //C_CI_NA_1 Counter interrogation command
    {
      Serial.println("Counter interrogation command");
      inviaS(client); //S-FORMAT
    }
    else if (type == 0x67) //C_CS_NA_1 Clock synchronisation command
    {
      bufferIn[6] = 0x6E; // eIEC870_COT_FILE + P/N + T
      inviaBuf(client, bufferIn, lunghezza);
      Serial.println("Clock synchronisation command");
    }
    else
    {
      Serial.print("Type sconosciuto: ");
      Serial.println(type);
      inviaS(client); //S-FORMAT
    }
  }
  else if (bitRead(bufferIn[0], 0) && !bitRead(bufferIn[0], 1)) //S-FORMAT
  {
    //Serial.println("S-TYPE: "+String(sequenceRx));
  }
  else if (bitRead(bufferIn[0], 0) && bitRead(bufferIn[0], 1)) //U-FORMAT
  {
    if (bitRead(bufferIn[0], 2)) {
      Serial.println("Start_DT");  //<--Se ricevo la richiesta (ma come client la devo inviare io)
      inviaU(client, 0x0B);
      dataTransfer=true;
    }
    else if (bitRead(bufferIn[0], 4)) {
      Serial.println("Stop_DT");  //<--Se ricevo la richiesta (ma come client la devo inviare io)
      inviaU(client, 0x23);
      dataTransfer=false;
    }
    else if (bitRead(bufferIn[0], 6)) {
      Serial.println("Test_FR");  //<--Se ricevo la richiesta
      inviaU(client, 0x83);
    }
    else if (bufferIn[0] == 0x0B) {
      Serial.println("Start_DT");  //Risposta dal server
      inviaS(client); /*avvio=true;*/
    }
    else if (bufferIn[0] == 0x23) Serial.println("Stop_DT"); //Risposta dal server
    else if (bufferIn[0] == 0x83) {
      Serial.println("Test_FR");  //Risposta dal server
    }
  }
  return bufferCount;
}

void IEC104_HELPER::setClient(Client *client)
{
  client0 = &*client;
  sequenceTx=0;
  sequenceRx=0;
}

void IEC104_HELPER::read(byte *type, int *ca, long *ioa, long *value)
{
  if(bufferCount>0)
  {
    *type=bufferTag[bufferCount-1][0];
    *ca=bufferTag[bufferCount-1][1];
    *ioa=bufferTag[bufferCount-1][2];
    *value=bufferTag[bufferCount-1][3];
    bufferCount--; //Ho svuotato un blocco del buffer
  }
}

void IEC104_HELPER::invia(Client *cli, byte type)
{
  byte bufferOut[14] = {0x00, 0x00, 0x00, 0x00, 0x64, 0x01, 0x06, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x14}; //Interrogation Command
  if (C_IC_NA_1) inviaBuf(cli, bufferOut, 14);
  byte msg[] = {0x00, 0x00, 0x00, 0x00, 0x46, 0x01, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}; //M_EI_NA_1 End of Initialisation
  if (M_EI_NA_1) inviaBuf(cli,msg,sizeof(msg));
}

void IEC104_HELPER::inviaBuf(Client *cli, byte* bufferOut, byte lung)
{
  cli->write(0x68); //Iniziatore di stringa
  cli->write(lung); //Lunghezza stringa (minimo 4)
  for (int i = 0; i < lung; i++) cli->write(bufferOut[i]);
}

//Invia il U-Format
void IEC104_HELPER::inviaU(Client *client, byte msg)
{
  byte buf[4];
  buf[0] = msg; //I primi due BIT (alti) indicano l' U-Format
  buf[1] = 0x00;
  buf[2] = 0x00;
  buf[3] = 0x00;
  inviaBuf(client, buf, 4);
}

//Invia il S-Format (conferma messaggio)
void IEC104_HELPER::inviaS(Client *client)
{
  byte buf[4];
  buf[0] = 0x01; //Il primo BIT indica l' S-Format
  buf[1] = 0x00;
  buf[2] = (byte)(sequenceRx << 1);
  buf[3] = (byte)(sequenceRx >> 7);
  inviaBuf(client, buf, 4);
}

void IEC104_HELPER::inviaI(Client *client, byte* bufferOut)
{
  int dim = sizeof(bufferOut);
  byte buf[dim + 4];
  buf[0] = (byte)(sequenceTx << 1) | 1; //Il primo BIT indica l' S-Format
  buf[1] = (byte)(sequenceTx >> 7);
  buf[2] = (byte)(sequenceRx << 1);
  buf[3] = (byte)(sequenceRx >> 7);
  for (int a = 0; a < dim; a++) buf[a + 4] = bufferOut[a];
  inviaBuf(client, buf, dim + 4);
  sequenceTx++; //Incremento il contatore dei messaggi inviati
}

IEC104_MASTER::IEC104_MASTER(IPAddress host, int port, bool wired)
{
  serverIP = host;
  serverPort = port;
  if(wired) iecSlave = new(EthernetClient); //Connessione cablata
  else iecSlave = new(WiFiClient); //Connessione WiFi
}
IEC104_MASTER::~IEC104_MASTER()
{

}

void IEC104_MASTER::read(byte *type, int *ca, long *ioa, long *value)
{
  connection.read(&*type, &*ca, &*ioa, &*value);
}

int IEC104_MASTER::available()
{
  int result = 0;
  if (!iecSlave->connected())
  {
    iecSlave->stop();
    iecSlave->connect(serverIP, serverPort);
    timeout0 = millis() + IEC_T0; //Timeout connessione
    timeout1 = millis() + IEC_T1; //Timeout risposta
    timeout2 = millis() + IEC_T2; //Timeout boh?
    timeout3 = millis() + IEC_T3; //Timeout boh?
    connection.setClient(&*iecSlave);
    avvio = true;
  }
  else if (timeout1 < millis())
  {
    iecSlave->stop();
    //error = 2; //Timeout 0 (mancata connessione)
    error = 2; //Timeout 1 (mancata risposta)
    return -1;
  }
  else
  {
    if (avvio) //Invio questi comandi al primo avvio
    {
      //connection.invia(iecSlave,); //End of initialization
      connection.inviaU(iecSlave, 0x07); // Start_DT
      connection.invia(iecSlave, C_IC_NA_1); //Interrogation command
      delay(100);
      avvio = false;
    }
    if (iecSlave->available() > 0) //Se c'è qualcosa in arrivo
    {
      delay(100);
      if (iecSlave->read() == 0x68) //Iniziatore di stringa
      {
        testSent = false;
        if (IEC_T1 < 5000) IEC_T1 = 5000; //T1 non può avere valore inferiore a 5"
        timeTest = millis() + IEC_T_TEST;
        timeout1 = millis() + IEC_T1; //Finché ricevo dati resetto il timeout 1
        byte lung = iecSlave->read(); //Leggo la lunghezza della stringa
        if (lung <= 253) //Lo standard richiede una lunghezza massima di 253 bytes
        {
          byte bufferIn[lung]; //Creo il buffer per la lettura
          for (int i = 0; i < lung; i++) bufferIn[i] = iecSlave->read(); //Leggo i restanti bytes
          result = connection.elaboraBuffer(bufferIn, lung, iecSlave); //Analizzo i dati appena ricevuti
        }
        else Serial.println("ERRORE, lunghezza APDU oltre il limite");
      }
    }
    else if (timeTest < millis() && !testSent)
    {
      connection.inviaU(iecSlave, 0x43); //Invio il TEST
      testSent = true;
    }
  }
  return result;
}

void IEC104_MASTER::setTimeout(int timer, int value)
{
  if (value > 0)
  {
    if (timer == 0) IEC_T0 = value;
    if (timer == 1) IEC_T1 = value;
    if (timer == 2) IEC_T2 = value;
    if (timer == 3) IEC_T3 = value;
  }
}
