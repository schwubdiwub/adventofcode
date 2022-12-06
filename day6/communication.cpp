#include <array>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

int findFirstConsecutiveUniqueByteChain(const std::string& data, int consecutiveCount) noexcept
{
    const int consecutiveMinusOne = consecutiveCount - 1;

    for(int i = consecutiveMinusOne; i < data.size(); ++i) {
        std::string_view subData{ &data[i - consecutiveMinusOne], static_cast<size_t>(consecutiveCount) };
        bool beginningFound = true;

        for(int j = 0; (j < subData.size() - 1) & beginningFound; ++j) {
            for(int k = j + 1; k < subData.size(); ++k) {
                if(subData[j] == subData[k]) {
                    beginningFound = false;
                    break;
                }
            }
        }

        if(beginningFound) {
            return i + 1;
        }
    }

    return -1;
}

int main()
{

    std::string data;
    data.reserve(1024);

    std::fstream file("communication_input");
    std::getline(file, data);

    std::cout << std::format("The start of the packet marker is: {}\n", findFirstConsecutiveUniqueByteChain(data, 4));
    std::cout << std::format("Part 2:\nThe start of the message marker is: {}\n", findFirstConsecutiveUniqueByteChain(data, 14));

    return 0;
}
