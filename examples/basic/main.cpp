#include <iostream>

#include <AdsClient/AdsClient.h>

int main() {

    AdsClient client(851);

    const auto handle = client.getVariableByName("MAIN.MyFloat");

    float data;

    bool success = client.read_by_handle(handle, &data, sizeof(float));

    if (!success) {

        std::cerr << "Failed to read_by_handle from ADS server..." << std::endl;
        return 1;
    }

    std::cout << "New value: " << data;

    return 0;
}
