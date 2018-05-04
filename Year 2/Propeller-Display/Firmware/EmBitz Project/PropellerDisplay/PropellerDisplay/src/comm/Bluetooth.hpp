#pragma once
#include <cstdint>
#include <string.h>

/*********** PACKET DESCRIPTION STRUCT **************/
#include <array>

enum class RequestedDrawMode : std::uint8_t {
    clockMode,
    messageMode
};

// data that is sent by the computer
struct alignas(32) BluetoothData {
    RequestedDrawMode requestedDrawMode;
    std::uint8_t messageLength;
    std::array<std::uint8_t, 23> message;
} __attribute__((packed));
/***************************************************/

// enum that shows whether packet has succeeded
enum class BluetoothStatus : std::uint16_t {
    fail,
    success
};

static_assert(sizeof(BluetoothData) == 32,
              "For USART to work size of blue tooth data must be 32");

class BluetoothManager {
public:
    // singleton
    static BluetoothManager& get() {
        static BluetoothManager instance;
        return instance;
    }

    BluetoothData * getData(void) {
        return &receivedData.data;
    }

    // initialises bluetooth
    void init();

    void sendPacket(BluetoothData * packetSend);

    // retrieves the data passed retrieved from the bluetooth
    // returns true if data is complete and it is safe to read the
    // bluetooth data otherwise it returns false
    bool received(BluetoothData * r) {
        // if we finished getting all the data from
        // bluetooth give the caller the data
        if(countReceived > 0) {
            memcpy(r, receivedData.buf, sizeof(BluetoothData));
            flush_data();/* Flush the data */
            return true;
        }
        // otherwise tell the caller that the data
        // has not been received
        else {
            return false;
        }
    }

    bool received(void) {
        return countReceived > 0;
    }

    void flush_data(void) {
        memset(receivedData.buf, 0, sizeof(BluetoothData));
        countReceived = 0;
    }

    void rx_timeout(void);

    int countReceived;
    int countSent;

private:
    BluetoothManager() : countReceived{0}, countSent{0} {}

    BluetoothManager(const BluetoothManager&) =delete;
    BluetoothManager(BluetoothManager&&) =delete;

    union ReceivedData {
        BluetoothData data;
        std::uint8_t buf[sizeof(data)];
    };

    ReceivedData receivedData;
};


