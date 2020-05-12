The software implements communication via IEC60870-5-104 on Node-RED as "MASTER" to connect with PLCs or RTU that use this protocol.<br/>
<br/>
In v1.0.1 are supported:<br/>
M_SP_NA_1 - Single-point information<br/>
M_DP_NA_1 - Double-point information<br/>
M_ME_NA_1 - Measured value, normalised value<br/>
C_IC_NA_1 - Interrogation Command<br/>
<br/>
The function block "IEC60870-5-104" included in a flow, provide a message with four attributes: type, common address, info object address and value for each measurement received.
