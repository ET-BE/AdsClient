#include <iostream>

#include <AdsClient/AdsClient.h>

int main() {

    AdsClient client(350);

    const auto handle = client.getVariableByName("MAIN.MyFloat");

    float data;

    bool success = client.read(handle, &data, sizeof(float));

    if (!success) {

        std::cerr << "Failed to read from ADS server..." << std::endl;
        return 1;
    }

    std::cout << "New value: " << data;

    return 0;
}
