#include "Bluetooth.hpp"


void BluetoothManager::init() {
    TM_USART_Init(USART6, TM_USART_PinsPack_1, 9600);

}


extern "C" void TM_USART6_ReceiveHandler(uint8_t ch) {
    BluetoothManager::get().pushRx(ch);
}
