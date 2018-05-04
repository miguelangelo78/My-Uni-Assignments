#include "Bluetooth.hpp"
#include "DebugHelpers.hpp"
#include "stm32f4xx_usart.h"
#include "tm_stm32f4_usart.h"
#include <string.h>

#define DEFAULT_BAUDRATE 115200
#define USART USART3

/* This callback gets called whenever a packet is ready to be handled: */
void usart_packet_cback(struct BluetoothData * packet) {
    BluetoothManager& bt = BluetoothManager::get();

    memcpy(bt.getData(), packet, sizeof(BluetoothData));
    bt.countReceived++;
}

/* Variables necessary for the USART IRQ: */
static struct BluetoothData rxDataCarrier; /* This is the packet buffer */
uint8_t * rx_carrier_ptr; /* Points to the packet buffer */
uint8_t rx_carrier_ptr_index = 0;
uint8_t start_flag = 0;

void TM_USART3_ReceiveHandler(uint8_t data) {
    if(start_flag == 2) {
        rx_carrier_ptr[rx_carrier_ptr_index] = data;
        if(++rx_carrier_ptr_index >= sizeof(BluetoothData)) {
            usart_packet_cback(&rxDataCarrier);
            start_flag = 0; /* Restart start flag */
            memset(rx_carrier_ptr, 0, sizeof(BluetoothData));
            rx_carrier_ptr_index = 0;
        }
    }

    /* Start condition: */
    for(int i = 0; i < 2;i++)
        if(start_flag == i && data == 255-start_flag) start_flag++;
}

void BluetoothManager::init() {
    rx_carrier_ptr = (uint8_t*) &rxDataCarrier;
    TM_USART_Init(USART, TM_USART_PinsPack_2, DEFAULT_BAUDRATE);

#if 0
    for(;;) /* Echo packet back */
        if(received()) {
            sendPacket(getData());
            flush_data();
        }
#endif
}

void BluetoothManager::rx_timeout(void) {
    /* The transmitter */
    start_flag = 0;
    flush_data();
}

void BluetoothManager::sendPacket(BluetoothData * packetSend) {
    TM_USART_Send(USART, (uint8_t*)packetSend, sizeof(BluetoothData));
    countSent++;
}
