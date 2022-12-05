#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

struct Stack {
    std::vector<char> crates;

    char take() noexcept
    {
        assert(not crates.empty());
        const char crate = crates.back();
        crates.pop_back();
        return crate;
    }

    void place(char crate) noexcept
    {
        crates.emplace_back(crate);
    }

    char top() const noexcept
    {
        assert(not crates.empty());
        return crates.back();
    }

    void transferCratesTo(Stack& destination, ptrdiff_t count) noexcept
    {
        auto moveBeginIt = crates.end() - count;
        destination.crates.insert(destination.crates.end(), moveBeginIt, crates.end());
        crates.erase(moveBeginIt, crates.end());
    }
};

struct Command {
    size_t source;
    size_t destination;
    std::ptrdiff_t count;
};

using Stacks = std::array<Stack, 9>;
using Commands = std::vector<Command>;

std::pair<Stacks, Commands> parseStacksAndCommands(std::string_view filepath) noexcept
{
    Stacks stacks;

    std::ifstream file(filepath.data());
    {
        std::regex baseRegex(" 1   2   3   4   5   6   7   8   9 ");

        std::vector<std::string> lines;
        lines.reserve(16);
        lines.emplace_back();

        while (std::getline(file, lines.back())) {
            // find index line
            if(std::regex_match(lines.back(), baseRegex)) {
                std::getline(file, lines.back());
                lines.pop_back();
                break;
            }
            else {
                lines.emplace_back();
            }
        }
        // we need to insert from the bottom up
        std::ranges::reverse(lines);

        std::string_view crateRegex = "((   )|(\\[[A-Z]\\]))";
        std::regex crateLineRegex(std::format("{0} {0} {0} {0} {0} {0} {0} {0} {0}", crateRegex));
        std::smatch subMatches;
        for(const auto& line : lines) {
            const bool matched = std::regex_match(line, subMatches, crateLineRegex);
            assert(matched && "Expected match failed");
            assert(subMatches.size() == 28);
            for(int i = 0; i < 9; ++i) {
                const auto subMatchIndex = i * 3
                    + 1     // phrase match
                    + 2;    // third submatch in submatch block
                if(subMatches[subMatchIndex].matched) {
                    stacks[i].place(subMatches[subMatchIndex].str()[1]);
                }
            }
        }
    }

    Commands commands;
    commands.reserve(1024);

    {
        std::regex commandRegex("move ([1-9][0-9]*) from ([1-9]) to ([1-9])");
        std::smatch subMatches;

        std::string line;
        while (std::getline(file, line)) {
            const auto matched = std::regex_match(line, subMatches, commandRegex);
            assert(matched);
            assert(subMatches.size() == 4);
            commands.emplace_back(std::stoull(subMatches[2].str()) - 1,
                                  std::stoull(subMatches[3].str()) - 1,
                                  std::stoll(subMatches[1].str()));
        }
    }

    return { stacks, commands };
}

void executeCommandCrateMover9000(const Command& command, Stacks& stacks) noexcept
{
    auto& sourceStack = stacks[command.source];
    auto& destinationStack = stacks[command.destination];

    for(std::ptrdiff_t i = 0; i < command.count; ++i) {
        destinationStack.place(sourceStack.take());
    }
}

void executeCommandCrateMover9001(const Command& command, Stacks& stacks) noexcept
{
    auto& sourceStack = stacks[command.source];
    auto& destinationStack = stacks[command.destination];

    sourceStack.transferCratesTo(destinationStack, command.count);
}

int main()
{
    const auto [ stacks, commands ] = parseStacksAndCommands("cratestacking_input");

    {
        auto stacks9000 = stacks;

        for (const auto& command : commands) {
            executeCommandCrateMover9000(command, stacks9000);
        }

        std::cout << "The top crates are (CrateMover 9000): ";
        for (const auto& stack : stacks9000) {
            std::cout << stack.top();
        }
        std::cout << "\n";
    }

    {
        auto stacks9001 = stacks;

        for (const auto& command : commands) {
            executeCommandCrateMover9001(command, stacks9001);
        }

        std::cout << "The top crates are (CrateMover 9001): ";
        for (const auto& stack : stacks9001) {
            std::cout << stack.top();
        }
        std::cout << "\n";
    }

    return 0;
}
