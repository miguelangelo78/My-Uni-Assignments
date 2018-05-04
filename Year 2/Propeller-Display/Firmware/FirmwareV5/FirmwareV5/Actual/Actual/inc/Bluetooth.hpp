#pragma once

#include <cstdint>
#include "tm_stm32f4_usart.h"
#include <array>

enum class RequestedDrawMode : std::uint8_t {
    clockMode,
    messageMode
};


struct alignas(32) BluetoothData {
    RequestedDrawMode requestedDrawMode;
    std::uint8_t messageLength;
    std::array<std::uint8_t, 23> message;
} __attribute__((packed));

enum class BluetoothStatus : std::uint16_t {
    fail,
    success
};

static_assert(sizeof(BluetoothData) == 32,
              "For USART to work size of blue tooth data must be 32");

class BluetoothManager {
public:
    static BluetoothManager& get() {
        static BluetoothManager instance;
        return instance;
    }


    void init();


    void pushRx(uint8_t ch) {
        if(countRecieved + 1 > 31) {
            auto status = BluetoothStatus::fail;
            return TM_USART_Send(USART6, (uint8_t *)status, 2);
        }

        else {
            recievedData.buf[countRecieved++] = ch;
            auto status = BluetoothStatus::success;
            return TM_USART_Send(USART6, (uint8_t *)status, 2);
        }
    }


    bool recieved(BluetoothData& r) {
        if(countRecieved == sizeof(BluetoothData)) {
            r = recievedData.data;
            return true;

        }
        else {
            return false;
        }
    }

private:
    int countRecieved;
    int countSent;

    union RecievedData {
        BluetoothData data;
        std::uint8_t buf[sizeof(data)];
    };

    RecievedData recievedData;
};


