#include <iostream>

#include <AdsClient/AdsClient.h>

int main() {

    AdsClient client(AMSPORT_R0_PLC_TC3);

    auto symbol = client.getSymbolByName<double>("MAIN.mydouble");

    std::cout << "Initial value: " << symbol.read() << std::endl;

    symbol.write(4.20);

    std::cout << "New value: " << symbol.read() << std::endl;


    typedef std::array<float, 5> List;
    auto symbol_list = client.getSymbolByName<List>("MAIN.mylist");

    symbol_list.write({1,2,3,4,5});

    auto value_list = symbol_list.read();

    std::cout << "List: ";
    for (const auto& item : value_list) {
        std::cout << item << ", ";
    }
    std::cout << std::endl;

    symbol_list.value().at(4) = 3.14;
    symbol_list.write();

    return 0;
}
