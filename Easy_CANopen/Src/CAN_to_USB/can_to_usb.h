#include <stdio.h>
#include <stdbool.h>
#include <stdint.h> // 추가된 헤더 파일
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/ioctl.h> // 추가된 헤더 파일

#include "../Hardware/Hardware.h"   

STATUS_CODE QT_USB_Transmit(uint16_t COB_ID, uint8_t data[], uint8_t DLC);
STATUS_CODE QT_USB_Get_ID_Data(uint16_t *COB_ID, uint8_t data[], bool *is_new_message);