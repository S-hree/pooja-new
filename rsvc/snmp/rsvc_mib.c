#include <platform/common.h>
#include <atlk/mibs/rsvc-mib.h>

#include "snmp.h"

SNMP_IPv4_SCALAR_GET(rsvcDefaultDestIpAddressIPv4, "1.3.6.1.4.1.38405.1.7.2.1")
SNMP_IPv4_SCALAR_SET(rsvcDefaultDestIpAddressIPv4, "1.3.6.1.4.1.38405.1.7.2.1")
SNMP_TRUTHVALUE_SCALAR_GET(rsvcDefaultDestIpAddressIPv4_set, "1.3.6.1.4.1.38405.1.7.7.1.0")
SNMP_TRUTHVALUE_SCALAR_SET(rsvcDefaultDestIpAddressIPv4_set, "1.3.6.1.4.1.38405.1.7.7.1.0")
SNMP_ENUM_COLUMN_GET(rsvcWlanFwdRowStatus, mib_RowStatus_t, "1.3.6.1.4.1.38405.1.7.3.2.1.2")
SNMP_ENUM_COLUMN_SET(rsvcWlanFwdRowStatus, mib_RowStatus_t, "1.3.6.1.4.1.38405.1.7.3.2.1.2")
SNMP_Unsigned16_COLUMN_GET(rsvcWlanFwdDestPortNumber, "1.3.6.1.4.1.38405.1.7.3.2.1.3")
SNMP_Unsigned16_COLUMN_SET(rsvcWlanFwdDestPortNumber, "1.3.6.1.4.1.38405.1.7.3.2.1.3")
SNMP_Integer32_COLUMN_GET(rsvcWlanFwdIfIndex, "1.3.6.1.4.1.38405.1.7.3.2.1.4")
SNMP_Integer32_COLUMN_SET(rsvcWlanFwdIfIndex, "1.3.6.1.4.1.38405.1.7.3.2.1.4")
SNMP_ENUM_COLUMN_GET(rsvcWlanFwdFrameType, mib_rsvcWlanFwdFrameType_t, "1.3.6.1.4.1.38405.1.7.3.2.1.5")
SNMP_ENUM_COLUMN_SET(rsvcWlanFwdFrameType, mib_rsvcWlanFwdFrameType_t, "1.3.6.1.4.1.38405.1.7.3.2.1.5")
SNMP_Unsigned64_COLUMN_GET(rsvcWlanFwdProtocolId, "1.3.6.1.4.1.38405.1.7.3.2.1.6")
SNMP_Unsigned64_COLUMN_SET(rsvcWlanFwdProtocolId, "1.3.6.1.4.1.38405.1.7.3.2.1.6")
SNMP_TRUTHVALUE_SCALAR_GET(rsvcWlanBridgeEnabled, "1.3.6.1.4.1.38405.1.7.3.3.1")
SNMP_TRUTHVALUE_SCALAR_SET(rsvcWlanBridgeEnabled, "1.3.6.1.4.1.38405.1.7.3.3.1")
SNMP_Integer32_SCALAR_GET(rsvcWlanBridgeIfIndex, "1.3.6.1.4.1.38405.1.7.3.3.2")
SNMP_Integer32_SCALAR_SET(rsvcWlanBridgeIfIndex, "1.3.6.1.4.1.38405.1.7.3.3.2")
SNMP_Integer32_SCALAR_GET(rsvcWlanBridgeVlanId, "1.3.6.1.4.1.38405.1.7.3.3.3")
SNMP_Integer32_SCALAR_SET(rsvcWlanBridgeVlanId, "1.3.6.1.4.1.38405.1.7.3.3.3")
