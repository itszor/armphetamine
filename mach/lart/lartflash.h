#ifndef LART_FLASH_H
#define LART_FLASH_H 1
/* portions are stolen from Linux:drivers/mtd/devices/lart.c */

/* Mangle data (x) */
#define DATA_TO_FLASH(x)                                \
        (                                               \
                (((x) & 0x08009000) >> 11)      +       \
                (((x) & 0x00002000) >> 10)      +       \
                (((x) & 0x04004000) >> 8)       +       \
                (((x) & 0x00000010) >> 4)       +       \
                (((x) & 0x91000820) >> 3)       +       \
                (((x) & 0x22080080) >> 2)       +       \
                ((x) & 0x40000400)              +       \
                (((x) & 0x00040040) << 1)       +       \
                (((x) & 0x00110000) << 4)       +       \
                (((x) & 0x00220100) << 5)       +       \
                (((x) & 0x00800208) << 6)       +       \
                (((x) & 0x00400004) << 9)       +       \
                (((x) & 0x00000001) << 12)      +       \
                (((x) & 0x00000002) << 13)              \
        )

/* Unmangle data (x) */
#define FLASH_TO_DATA(x)                                \
        (                                               \
                (((x) & 0x00010012) << 11)      +       \
                (((x) & 0x00000008) << 10)      +       \
                (((x) & 0x00040040) << 8)       +       \
                (((x) & 0x00000001) << 4)       +       \
                (((x) & 0x12200104) << 3)       +       \
                (((x) & 0x08820020) << 2)       +       \
                ((x) & 0x40000400)              +       \
                (((x) & 0x00080080) >> 1)       +       \
                (((x) & 0x01100000) >> 4)       +       \
                (((x) & 0x04402000) >> 5)       +       \
                (((x) & 0x20008200) >> 6)       +       \
                (((x) & 0x80000800) >> 9)       +       \
                (((x) & 0x00001000) >> 12)      +       \
                (((x) & 0x00004000) >> 13)              \
        )


/* Mangle address (x) on chip U2 */
#define ADDR_TO_FLASH_U2(x)                             \
        (                                               \
                (((x) & 0x00000f00) >> 4)       +       \
                (((x) & 0x00042000) << 1)       +       \
                (((x) & 0x0009c003) << 2)       +       \
                (((x) & 0x00021080) << 3)       +       \
                (((x) & 0x00000010) << 4)       +       \
                (((x) & 0x00000040) << 5)       +       \
                (((x) & 0x00000024) << 7)       +       \
                (((x) & 0x00000008) << 10)              \
        )

/* Unmangle address (x) on chip U2 */
#define FLASH_U2_TO_ADDR(x)                             \
        (                                               \
                (((x) << 4) & 0x00000f00)       +       \
                (((x) >> 1) & 0x00042000)       +       \
                (((x) >> 2) & 0x0009c003)       +       \
                (((x) >> 3) & 0x00021080)       +       \
                (((x) >> 4) & 0x00000010)       +       \
                (((x) >> 5) & 0x00000040)       +       \
                (((x) >> 7) & 0x00000024)       +       \
                (((x) >> 10) & 0x00000008)              \
        )

/* Mangle address (x) on chip U3 */
#define ADDR_TO_FLASH_U3(x)                             \
        (                                               \
                (((x) & 0x00000080) >> 3)       +       \
                (((x) & 0x00000040) >> 1)       +       \
                (((x) & 0x00052020) << 1)       +       \
                (((x) & 0x00084f03) << 2)       +       \
                (((x) & 0x00029010) << 3)       +       \
                (((x) & 0x00000008) << 5)       +       \
                (((x) & 0x00000004) << 7)               \
        )

/* Unmangle address (x) on chip U3 */
#define FLASH_U3_TO_ADDR(x)                             \
        (                                               \
                (((x) << 3) & 0x00000080)       +       \
                (((x) << 1) & 0x00000040)       +       \
                (((x) >> 1) & 0x00052020)       +       \
                (((x) >> 2) & 0x00084f03)       +       \
                (((x) >> 3) & 0x00029010)       +       \
                (((x) >> 5) & 0x00000008)       +       \
                (((x) >> 7) & 0x00000004)               \
        )


#define READ_ARRAY         0x00FF00FF    /* Read Array/Reset */
#define READ_ID_CODES      0x00900090    /* Read Identifier Codes */
#define ERASE_SETUP        0x00200020    /* Block Erase */
#define ERASE_CONFIRM      0x00D000D0    /* Block Erase and Program Resume */
#define PGM_SETUP          0x00400040    /* Program */
#define STATUS_READ        0x00700070    /* Read Status Register */
#define STATUS_CLEAR       0x00500050    /* Clear Status Register */
#define STATUS_BUSY        0x00800080    /* Write State Machine Status (WSMS) */
#define STATUS_ERASE_ERR   0x00200020    /* Erase Status (ES) */
#define STATUS_PGM_ERR     0x00100010    /* Program Status (PS) */

#endif
