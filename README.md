# IEC60870-5-104
The software implements communication via IEC60870-5-104 on arduino, you can use multiple instances of "MASTER" and "SLAVE" objects to connect Arduino with PLCs or RTU that use this protocol.<br/>
<br/>
In v1.0.1 are supported:<br/>
M_SP_NA_1 - Single-point information<br/>
M_DP_NA_1 - Double-point information<br/>
M_ME_NA_1 - Measured value, normalised value<br/>
C_SC_NA_1 - Single command<br/>
C_DC_NA_1 - Double command<br/>
C_IC_NA_1 - Interrogation Command<br/>
M_EI_NA_1 - End of Initialization<br/>
<br/>
By command "setParam(byte,bool)" you can set some telegrams to be sent to the connection establishment:<br/>
0 - M_EI_NA_1 - End of Initialization<br/>
1 - C_IC_NA_1 - Interrogation Command (Valid only for MASTER instance)<br/>
<br/>
The library was tested on ESP32 with WiFi and Ethernet. To enable WIRED mode you need to #define IECWIRED in IEC60870-5-104.h file.
Only for ESP32 the Ethenet library needs the Server.h file to be modified: you need to change "virtual void begin(uint16_t port=0) =0;" line with this "virtual void begin() =0;" (without commas).
