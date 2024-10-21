

#include "can_to_usb.h"

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif

#ifndef ifr_newname
#define ifr_newname     ifr_ifru.ifru_slave
#endif



// USB 인터페이스 초기화 상태
static bool usb_interface_initialized = false;

// 추가된 전역 변수
static int can_socket = -1;

/**
 * @brief SocketCAN 인터페이스를 초기화하는 함수
 * 
 * @return true 초기화 성공
 * @return false 초기화 실패
 */
bool Initialize_CAN_Interface() {
    struct ifreq ifr;
    struct sockaddr_can addr;

    // 구조체 초기화
    memset(&ifr, 0, sizeof(struct ifreq));
    memset(&addr, 0, sizeof(struct sockaddr_can));

    // 소켓 생성
    can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (can_socket < 0) {
        perror("Socket 생성 실패");
        return false;
    }

    // CAN 인터페이스 설정 (예: can0)
    strncpy(ifr.ifr_name, "can0", IFNAMSIZ - 1); // 안전한 복사를 위해 strncpy 사용
    if (ioctl(can_socket, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl 실패");
        close(can_socket);
        return false;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // 소켓에 주소 바인딩
    if (bind(can_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind 실패");
        close(can_socket);
        return false;
    }

    return true;
}

/**
 * @brief USB 인터페이스를 초기화하는 함수
 * 
 * @return true 초기화 성공
 * @return false 초기화 실패
 */
bool Initialize_USB_Interface() {
    // USB 초기화 로직 구현
    // 예시: Qt와의 시리얼 포트 연결 설정
    // 실제 구현에 맞게 수정 필요
    printf("USB 인터페이스 초기화 중...\n");
    // 초기화 성공 가정
    usb_interface_initialized = true;
    return usb_interface_initialized;
}

/**
 * @brief USB를 통해 CAN 메시지를 전송하는 함수
 * 
 * @param CAN_ID 전송할 메시지의 CAN-ID
 * @param data 전송할 데이터 배열
 * @param DLC 데이터 길이
 * @return STATUS_CODE 전송 상태 코드
 */
STATUS_CODE QT_USB_Transmit(uint16_t CAN_ID, uint8_t data[], uint8_t DLC) {
    if (can_socket < 0) {
        if (!Initialize_CAN_Interface()) {
            return STATUS_CODE_ERROR_INITIALIZATION;
        }
        usb_interface_initialized = true;
    }

    struct can_frame frame;
    memset(&frame, 0, sizeof(struct can_frame));

    frame.can_id = CAN_ID;
    frame.can_dlc = DLC;
    memcpy(frame.data, data, DLC);

    ssize_t bytes_sent = write(can_socket, &frame, sizeof(struct can_frame));
    if (bytes_sent != sizeof(struct can_frame)) {
        perror("CAN 메시지 전송 실패");
        return STATUS_CODE_ERROR_TRANSMIT;
    }

    printf("SocketCAN으로 데이터 전송: CAN ID=0x%X, DLC=%d, 데이터=", CAN_ID, DLC);
    for (int i = 0; i < DLC; i++) {
        printf("%02X ", frame.data[i]);
    }
    printf("\n");

    return STATUS_CODE_SUCCESSFUL;
}

/**
 * @brief USB를 통해 CAN 메시지를 수신하는 함수
 * 
 * @param COB_ID 수신한 메시지의 COB-ID를 저장할 포인터
 * @param data 수신한 데이터 배열을 저장할 배열
 * @param is_new_message 새로운 메시지 수신 여부를 저장할 포인터
 * @return STATUS_CODE 수신 상태 코드
 */
STATUS_CODE QT_USB_Get_ID_Data(uint16_t *COB_ID, uint8_t data[], bool *is_new_message) {
    // USB 인터페이스가 초기화되지 않았으면 초기화 시도
    if (!usb_interface_initialized) {
        if (!Initialize_USB_Interface()) {
            return STATUS_CODE_ERROR_INITIALIZATION;
        }
    }

    // USB로부터 데이터 수신 로직 구현
    // 실제 USB 수신 로직에 맞게 수정해야 함
    // 예시로 가상의 데이터 수신
    // 이 부분을 실제 데이터 수신 로직으로 대체해야 합니다
    
    // 수신 성공 가정
    *COB_ID = 0x123; // 예시 COB-ID
    for (uint8_t i = 0; i < 8; i++) {
        data[i] = i; // 예시 데이터
    }
    *is_new_message = true;

    return STATUS_CODE_SUCCESSFUL;
}

/**
 * @brief CAN 인터페이스를 종료하는 함수
 */
void Close_CAN_Interface() {
    if (can_socket >= 0) {
        close(can_socket);
        can_socket = -1;
        usb_interface_initialized = false;
    }
}
