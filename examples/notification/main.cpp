#include <iostream>
#include <chrono>
#include <thread>

#include <AdsClient/AdsClient.h>

void valueChanged(short new_value) {
    std::cout << "New value: " << new_value << std::endl;
}

void valueChanged2(float new_value) {
    std::cout << "New value 2: " << new_value << std::endl;
}

int main() {

    AdsClient client(851);

    auto symbol = client.getSymbolByName<short>("MAIN.counter");
    symbol.registerDeviceNotification(&valueChanged);

    auto symbol2 = client.getSymbolByName<float>("MAIN.MyFloat");
    symbol2.registerDeviceNotification(&valueChanged2);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    return 0;
}
