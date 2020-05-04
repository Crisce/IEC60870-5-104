# IEC60870-5-104
The software implements communication via IEC60870-5-104 on arduino, you can use multiple instances of "MASTER" and "SLAVE" objects to connect Arduino with PLCs or RTU that use this protocol.

In v1.0.0 are supported:
M_SP_NA_1 - Single-point information
M_ME_NA_1 - Measured value, normalised value
C_SC_NA_1 - Single command
C_DC_NA_1 - Double command
C_IC_NA_1 - Interrogation Command
M_EI_NA_1 - End of Initialization

The library was tested on ESP32 with WiFi. Actually the "SLAVE" instance works only in WiFi mode.
