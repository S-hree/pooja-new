#ifndef _LIBSNMP_BER_H
#define _LIBSNMP_BER_H

/* Basic Encoding Rules definitions */
/* Refer to ITU-T X.690 (7/2002) for more information */
#define BER_EOC                 0x00
#define BER_BOOLEAN             0x01
#define BER_INTEGER             0x02
#define BER_INETADDRESSIPV4     0x40
#define BER_COUNTER             0x41
#define BER_GAUGE               0x42
#define BER_COUNTER64           0x46
#define BER_BIT_STRING          0x03
#define BER_OCTET_STRING        0x04
#define BER_NULL                0x05
#define BER_OBJECT_IDENTIFIER   0x06
#define BER_CHARACTER_STRING    0x1D
#define BER_SEQUENCE            0x30
#define BER_GET_REQUEST         0xA0
#define BER_SET_REQUEST         0xA3
#define BER_GET_RESPONSE        0xA2
#define BER_MORE_BIT            0x80

#endif /* _LIBSNMP_BER_H */
