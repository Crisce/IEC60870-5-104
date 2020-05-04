#ifndef IEC104_H
#define IEC104_H


//Type of data
#define M_SP_NA_1 0x01 //Single-point information TESTED
#define M_SP_TA_1 0x02 //Single-point information with time tag
#define M_DP_NA_1 0x03 //Double-point information
#define M_DP_TA_1 0x04 //Double-point information with time tag
#define M_ST_NA_1 0x05 //Step position information
#define M_ST_TA_1 0x06 //Step position information with time tag
#define M_BO_NA_1 0x07 //Bitstring of 32 bit
#define M_BO_TA_1 0x08 //Bitstring of 32 bit with time tag
#define M_ME_NA_1 0x09 //Measured value, normalised value TESTED
#define M_ME_TA_1 0x0A //Measured value, normalized value with time tag
#define M_ME_NB_1 0x0B //Measured value, scaled value
#define M_ME_TB_1 0x0C //Measured value, scaled value wit time tag
#define M_ME_NC_1 0x0D //Measured value, short floating point number
#define M_ME_TC_1 0x0E //Measured value, short floating point number with time tag
#define M_IT_NA_1 0x0F //Integrated totals
#define M_IT_TA_1 0x10 //Integrated totals with time tag
#define M_EP_TA_1 0x11 //Event of protection equipment with time tag
#define M_EP_TB_1 0x12 //Packed start events of protection equipment with time tag
#define M_EP_TC_1 0x13 //Packed output circuit information of protection equipment with time tag
#define M_PS_NA_1 0x14 //Packed single point information with status change detection
#define M_ME_ND_1 0x15 //Measured value, normalized value without quality descriptor
#define C_SC_NA_1 0x2D //Single command TESTED
#define C_DC_NA_1 0x2E //Double command TESTED
#define C_RC_NA_1 0x2F //Regulating step command
#define C_SE_NA_1 0x30 //Set-point Command, normalised value
#define C_SE_NB_1 0x31 //Set-point Command, scaled value
#define C_SE_NC_1 0x32 //Set-point Command, short floating point number
#define C_BO_NA_1 0x33 //Bitstring 32 bit command
#define M_EI_NA_1 0x46 //End of Initialisation
#define C_IC_NA_1 0x64 //Interrogation command
#define C_CI_NA_1 0x65 //Counter interrogation command
#define C_RD_NA_1 0x66 //Read Command
#define C_CS_NA_1 0x67 //Clock synchronisation command
#define C_TS_NA_1 0x68 //Test command
#define C_RP_NA_1 0x69 //Reset process command
#define C_CD_NA_1 0x6A //Delay acquisition command
#define C_TS_TA_1 0x6B //Test command with time tag CP56Time2a

//Cause Of Transmission
#define eIEC870_COT_UNUSED := 0,
#define eIEC870_COT_CYCLIC 1
#define eIEC870_COT_BACKGROUND 2
#define eIEC870_COT_SPONTAN 3
#define eIEC870_COT_INIT 4
#define eIEC870_COT_REQ 5
#define eIEC870_COT_ACT 6
#define eIEC870_COT_ACT_CON 7
#define eIEC870_COT_DEACT 8
#define eIEC870_COT_DEACT_CON 9
#define eIEC870_COT_ACT_TERM 10
#define eIEC870_COT_RETREM 11
#define eIEC870_COT_RETLOC 12
#define eIEC870_COT_FILE 13
#define eIEC870_COT_INROGEN 20
#define eIEC870_COT_INRO1 21
#define eIEC870_COT_INRO2 22
#define eIEC870_COT_INRO3 23
#define eIEC870_COT_INRO4 24
#define eIEC870_COT_INRO5 25
#define eIEC870_COT_INRO6 26
#define eIEC870_COT_INRO7 27
#define eIEC870_COT_INRO8 28
#define eIEC870_COT_INRO9 29
#define eIEC870_COT_INRO10 30
#define eIEC870_COT_INRO11 31
#define eIEC870_COT_INRO12 32
#define eIEC870_COT_INRO13 33
#define eIEC870_COT_INRO14 34
#define eIEC870_COT_INRO15 35
#define eIEC870_COT_INRO16 36
#define eIEC870_COT_REQCOGEN 37
#define eIEC870_COT_REQCO1 38
#define eIEC870_COT_REQCO2 39
#define eIEC870_COT_REQCO3 40
#define eIEC870_COT_REQCO4 41
#define eIEC870_COT_UNKNOWN_TYPE 44
#define eIEC870_COT_UNKNOWN_CAUSE 45
#define eIEC870_COT_UNKNOWN_ASDU_ADDRESS 46
#define eIEC870_COT_UNKNOWN_OBJECT_ADDRESS 47

//Timeout
#define TIMEOUT_0 0
#define TIMEOUT_1 1
#define TIMEOUT_2 2
#define TIMEOUT_3 3
#define WIFI 0
#define WIRED 1

//Librerie
#include <Ethernet.h>
#include <WiFi.h>

//IEC60870-5-104 MASTER / CLIENT TCP
class IEC104_HELPER
{
  private:
    int sequenceRx=0;
    int sequenceTx=0;
    Client *client0;
    int IEC_T0 = 30000; //Timeout connessione
    int IEC_T1 = 15000; //Timeout risposta
    int IEC_T2 = 10000; //Non utilizzato ancora
    int IEC_T3 = 20000; //Non utilizzato ancora
    unsigned long timeout0=0;
    unsigned long timeout1=0;
    unsigned long timeout2=0;
    unsigned long timeout3=0;
    long bufferTag[64][4];
    int bufferCount=0;
    bool dataTransfer=false;
  public:
    IEC104_HELPER();
    void inviaBuf(Client *cli, byte* bufferOut, byte lung);
    void inviaU(Client *client, byte msg);
    void inviaS(Client *client);
    void inviaI(Client *client, byte* bufferOut);
    void invia(Client *cli, byte type);
    int elaboraBuffer(byte* bufferIn, byte lunghezza, Client *client);
    void setClient(Client *cli);
    void read(byte *type, int *ca, long *ioa, long *value);
    int check(Client *client);
    void send(byte type, byte num, int ca, long* IOA, long* val); //Invio le misure verso il client (master)
};

class IEC104_MASTER
{
 private:
  IPAddress serverIP;
  int serverPort;
  int IEC_T0 = 30000; //Timeout connessione
  int IEC_T1 = 15000; //Timeout risposta
  int IEC_T2 = 10000; //Non utilizzato ancora
  int IEC_T3 = 20000; //Non utilizzato ancora
  int IEC_T_TEST = 10000; //Non utilizzato ancora
  unsigned long timeout0=0;
  unsigned long timeout1=0;
  unsigned long timeout2=0;
  unsigned long timeout3=0;
  unsigned long timeTest=0;
  byte error = 0;
  bool avvio=false;
  bool testSent=false;
  Client *iecSlave; //Connessione MASTER in uscita (mi connetto ad uno SLAVE)
  IEC104_HELPER connection; //Istanza della gestione telegrammi IEC60870-5-104
  
 public:
  void setTimeout(int timer, int value);
  int available();
  void read(byte *type, int *ca, long *ioa, long *value);
  IEC104_MASTER(IPAddress host, int port, bool wired);
  ~IEC104_MASTER();
};

class IEC104_SLAVE
{  
  private:
    #define MAX_SRV_CLIENTS 4 //Numero massimo di CLIENT/MASTER IEC60870-5-104 connessi

    //Parametri IEC 104
    uint16_t serverPort = 2404;
    const int IEC_T0 = 30000; //Timeout connessione
    const int IEC_T1 = 15000; //Timeout risposta
    //const int T2 = 10000; //Non utilizzato ancora
    //const int T3 = 20000; //Non utilizzato ancora
    int sequence = 0;
    int sequenceRx = 0;
    unsigned long timeout0=0;
    unsigned long timeout1=0;
    unsigned long timeTest=0;
    //Client *iecMaster[MAX_SRV_CLIENTS]; //Connessione MASTER in ingresso (gli SLAVE si connettono a me)
    WiFiClient iecMaster[MAX_SRV_CLIENTS]; //Connessione MASTER in ingresso (gli SLAVE si connettono a me)
    IEC104_HELPER connection[MAX_SRV_CLIENTS]; //Connessione MASTER in ingresso (gli SLAVE si connettono a me)
    WiFiServer *iecServer;
    long bufferTag[64][4];
    bool wired = false;
    bool avvio=false;

  public:
    IEC104_SLAVE(WiFiServer *srv);
    ~IEC104_SLAVE();
    void read(byte *type, int *ca, long *ioa, long *value);
    int available();
    int availableWiFi(WiFiServer iecServer);
    void send(byte type, byte num, int ca, long* IOA, long* val); //Invio le misure verso il client (master)

}; //IEC104_SLAVE

#endif
