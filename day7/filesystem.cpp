#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace std::string_literals;

struct File {
    std::string name;
    size_t size;
};

struct Directory {
    std::string name;
    std::vector<Directory> subDirectories;
    std::vector<File> files;
    size_t size{ 0 };

    explicit Directory(std::string&& name) noexcept : name(name) { subDirectories.reserve(8); }
    size_t calculateSize() noexcept;
    [[nodiscard]] size_t sumSizesUpTo(size_t limit) const noexcept;
    [[nodiscard]] size_t findSmallestDirectoryAboveSize(size_t minimumRequiredSize) const noexcept;
};

size_t Directory::calculateSize() noexcept
{
    const size_t directSize = std::accumulate(files.begin(), files.end(), 0ULL, [](size_t s, const File& f) { return s + f.size; });
    const size_t indirectSize = std::accumulate(subDirectories.begin(), subDirectories.end(), 0ULL, [](size_t s, Directory& d) { return s + d.calculateSize(); });
    size = directSize + indirectSize;
    return size;
}

size_t Directory::sumSizesUpTo(size_t limit) const noexcept
{
    size_t sizeSum = size <= limit ? size : 0;
    const size_t indirectSum = std::accumulate(subDirectories.begin(), subDirectories.end(), 0ULL, [&limit](size_t s, const Directory& d) { return s + d.sumSizesUpTo(limit); });
    return sizeSum + indirectSum;
}

size_t Directory::findSmallestDirectoryAboveSize(size_t minimumRequiredSize) const noexcept
{
    if(size < minimumRequiredSize) {
        return std::numeric_limits<size_t>::max();
    }
    size_t minimumSize = size;
    for(const auto& dir : subDirectories) {
        const auto subDirMinimum = dir.findSmallestDirectoryAboveSize(minimumRequiredSize);
        if(subDirMinimum >= minimumRequiredSize & subDirMinimum < minimumSize) {
            minimumSize = subDirMinimum;
        }
    }
    return minimumSize;
}

struct Filesystem {
    Directory rootDirectory;
    constexpr static size_t TotalSpace = 70000000;
    constexpr static size_t SpaceRequiredByUpdate = 30000000;

    Filesystem() noexcept : rootDirectory("/"s) {}
    size_t freeSpace() const noexcept { return TotalSpace - rootDirectory.size; }
};

struct Shell {
    Filesystem filesystem;
    std::vector<uint64_t> currentDirIndex;
    Directory* currentDirectory = &filesystem.rootDirectory;

    void parseFilesystem(std::string_view filepath) noexcept;
    void updateCurrentDirPointer() noexcept;

    void commandCd(std::string_view parameters) noexcept;
    void discoverDirectory(std::string&& name) noexcept;
    void discoverFile(std::string&& name, size_t size) noexcept;
};

void Shell::parseFilesystem(std::string_view filepath) noexcept
{
    std::fstream file(filepath.data());
    std::string line;
    // ignore first line
    std::getline(file, line);

    std::regex dirRegex("^dir ([A-Za-z.]+)$");
    std::regex fileRegex("^([1-9][0-9]*) ([A-Za-z.]+)$");
    std::regex lsRegex("^\\$ ls$");
    std::regex cdRegex("^\\$ cd (..|[A-Za-z]+)$");
    std::smatch subMatches;

    bool skipRead = false;
    while(skipRead || std::getline(file, line)) {
        skipRead = false;

        // check for cd command
        if(std::regex_match(line, subMatches, cdRegex)) {
            assert(subMatches[1].matched);
            commandCd(subMatches[1].str());
        }
        else if(std::regex_match(line, lsRegex)) {
            skipRead = true;

            while(std::getline(file, line)) {
                // check for file
                if(std::regex_match(line, subMatches, fileRegex)) {
                    discoverFile(subMatches[2].str(), std::stoull(subMatches[1].str()));
                }
                // check for dir
                else if(std::regex_match(line, subMatches, dirRegex)) {
                    discoverDirectory(subMatches[1].str());
                }
                else {
                    // cd command or end of file
                    break;
                }
            }
        }
    }
}

void Shell::updateCurrentDirPointer() noexcept
{
    Directory* dir = &filesystem.rootDirectory;
    for(const auto& i : currentDirIndex) {
        dir = &dir->subDirectories[i];
    }
    currentDirectory = dir;
}

void Shell::commandCd(std::string_view parameters) noexcept
{
    if(parameters == "..") {
        currentDirIndex.pop_back();
    }
    else {
        auto foundIt = std::ranges::find(currentDirectory->subDirectories, parameters, &Directory::name);
        assert(foundIt != currentDirectory->subDirectories.end());
        currentDirIndex.emplace_back(std::distance(currentDirectory->subDirectories.begin(), foundIt));
    }
    updateCurrentDirPointer();
}

void Shell::discoverDirectory(std::string&& name) noexcept
{
    currentDirectory->subDirectories.emplace_back(std::move(name));
}

void Shell::discoverFile(std::string&& name, size_t size) noexcept
{
    currentDirectory->files.emplace_back(std::move(name), size);
}

int main()
{
    Shell shell;
    shell.parseFilesystem("filesystem_input");
//    shell.parseFilesystem("filesystem_input_test");

    const auto totalSize = shell.filesystem.rootDirectory.calculateSize();
    std::cout << std::format("The total size is {} ElfBytes.\n", totalSize);

    const auto sizeUpTo100000 = shell.filesystem.rootDirectory.sumSizesUpTo(100000);
    std::cout << std::format("The total size of all directories with a size up to 100000 is {} ElfBytes.\n", sizeUpTo100000);

    const auto sizeToDelete = Filesystem::SpaceRequiredByUpdate - shell.filesystem.freeSpace();
    std::cout << std::format("Used space            : {:12} ElfBytes\n"
                             "Free space            : {:12} ElfBytes\n"
                             "Missing required space: {:12} ElfBytes\n",
                             shell.filesystem.rootDirectory.size, shell.filesystem.freeSpace(), sizeToDelete);
    const auto minimumDeleteDirectorySize = shell.filesystem.rootDirectory.findSmallestDirectoryAboveSize(sizeToDelete);
    std::cout << std::format("The directory that is big enough to clear enough space but is the\n"
                             "smallest one of all the available has a size of {} ElfBytes.\n", minimumDeleteDirectorySize);

    return 0;
}
