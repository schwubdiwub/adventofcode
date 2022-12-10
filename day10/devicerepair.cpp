#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

struct Command {
    enum class Operation {
        noop,
        addx
    };
    Operation operation;
    int64_t parameter;

    [[nodiscard]] uint64_t numRequiredCycles() const noexcept;
    int64_t generateResult(int64_t registerX) const noexcept;
};

uint64_t Command::numRequiredCycles() const noexcept
{
    switch (operation) {
        case Operation::noop: return 1;
        case Operation::addx: return 2;
    }
}

int64_t Command::generateResult(int64_t registerX) const noexcept
{
    switch (operation) {
        case Operation::noop: return registerX;
        case Operation::addx: return registerX + parameter;
    }
}

struct CPU {
    uint64_t cycleNumber = 1;
    size_t currentCommandIndex = 0;
    uint64_t commandStartCycle = 1;
    int64_t registerX = 1;

    std::vector<Command> commandBuffer;

    void resetExecution() noexcept;
    void tick() noexcept;
    [[nodiscard]] int64_t signalStrength() const noexcept { return static_cast<int64_t>(cycleNumber) * registerX; }
    [[nodiscard]] bool executionFinished() const noexcept { return currentCommandIndex >= commandBuffer.size(); }
};

void CPU::resetExecution() noexcept
{
    cycleNumber = 1;
    currentCommandIndex = 0;
    commandStartCycle = 1;
    registerX = 1;
}

void CPU::tick() noexcept
{
    ++cycleNumber;

    const auto& currentCommand = commandBuffer[currentCommandIndex];
    if(cycleNumber - commandStartCycle == currentCommand.numRequiredCycles()) {
        registerX = currentCommand.generateResult(registerX);
        ++currentCommandIndex;;
        commandStartCycle = cycleNumber;
    }
}

struct CRT {
    CPU& cpu;

    bool colHasSprite(uint64_t col) const noexcept;
    void draw() const noexcept;
};

bool CRT::colHasSprite(uint64_t col) const noexcept
{
    return cpu.registerX - 1 <= col & col <= cpu.registerX + 1;
}

void CRT::draw() const noexcept
{
    uint64_t currentCol = 0;
    while (not cpu.executionFinished()) {
        if(currentCol == 40) {
            currentCol = 0;
            std::cout << "\n";
        }
        if(colHasSprite(currentCol)) {
            std::cout << "#";
        }
        else {
//            std::cout << ".";
            std::cout << " ";
        }

        cpu.tick();
        ++currentCol;
    }
}

CPU parseCommands(std::string_view filepath) noexcept
{
    CPU cpu;
    cpu.commandBuffer.reserve(256);

    std::ifstream file(filepath.data());
    std::string line;

    std::regex commandRegex("^(noop|addx)( (-?\\d+))?$");
    std::smatch subMatches;

    while (std::getline(file, line)) {
        const bool matched = std::regex_match(line, subMatches, commandRegex);
        assert(matched);

        if(subMatches[1].str() == "noop") {
            cpu.commandBuffer.emplace_back(Command::Operation::noop, 0);
        }
        else {
            assert(subMatches[1].str() == "addx");
            cpu.commandBuffer.emplace_back(Command::Operation::addx, std::stoll(subMatches[3].str()));
        }
    }

    return cpu;
}

int main()
{
    CPU cpu = parseCommands("devicerepair_input");

    int64_t signalStrengthSum = 0;

    const std::vector<uint64_t> signalStrengthPositions = { 20, 60, 100, 140, 180, 220 };
    while (not cpu.executionFinished()) {
        if (std::ranges::find(signalStrengthPositions, cpu.cycleNumber) != signalStrengthPositions.end()) [[unlikely]] {
            const auto nowSignalStrength = cpu.signalStrength();
            signalStrengthSum += nowSignalStrength;
            std::cout << std::format("The signal strength at cycle {:4} is {}.\n", cpu.cycleNumber, nowSignalStrength);
        }

        cpu.tick();
    }

    std::cout << std::format("The sum of the signal strengths is {}.\n", signalStrengthSum);

    std::cout << "\nPart 2:\n\n";

    cpu.resetExecution();
    CRT crt(cpu);
    crt.draw();

    return 0;
}
