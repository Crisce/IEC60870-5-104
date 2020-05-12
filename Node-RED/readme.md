The software implements communication via IEC60870-5-104 on Node-RED as "MASTER" to connect Arduino with PLCs or RTU that use this protocol.

In v1.0.1 are supported:
M_SP_NA_1 - Single-point information
M_DP_NA_1 - Double-point information
M_ME_NA_1 - Measured value, normalised value
C_IC_NA_1 - Interrogation Command

The function block "IEC60870-5-104" included in a flow, provide a message with four attributes: type, common address, info object address and value for each measurement received.
