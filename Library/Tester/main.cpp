#include "MdfFile.h"
#include "DummyStreamBuffer.h"

#include <fstream>
#include <memory>

#include <windows.h>
#include <psapi.h>


int loop(std::string const& fileName) {
    // Create wrapper.
    std::shared_ptr<std::basic_filebuf<char>> streamParent = std::make_shared<std::basic_filebuf<char>>();
    auto ptr = streamParent->open(fileName, std::ios::in | std::ios::binary);

    std::shared_ptr<mdf::DummyStreamBuffer> stream = std::make_shared<mdf::DummyStreamBuffer>(streamParent, 1 * 1024 * 1024);

    // Attempt to open test file.
    auto mdfFile = mdf::MdfFile::Create(stream);

    if(!mdfFile) {
        return 1;
    }

    auto iter = mdfFile->getCANIterator();

    //mdfFile->loadFileInfo();

    int ctr = 0;
    for (auto const &record: iter) {
        ctr++;
    }

    return ctr;
}

int main(int argc, char*  argv[]) {
    std::string const fileName1 = "finalized.mf4";
    std::string const fileName2 = "unfinalized2.mf4";
    std::string const fileName3 = "C:\\Projects\\Resources\\Datasets\\OBD2\\4674\\00000004_fin_file.mf4";

    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | SYNCHRONIZE,
        FALSE,
        GetCurrentProcessId());

    int loopCount = 10;
    PROCESS_MEMORY_COUNTERS_EX pmc[loopCount + 1];
    int ctr = 0;

    GetProcessMemoryInfo( hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc[0], sizeof(pmc[0]) );

    for(int i = 1; i < (loopCount + 1); ++i) {
        std::cout << "Start of loop " << i << std::endl;
        ctr += loop(fileName3);
        GetProcessMemoryInfo( hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc[i], sizeof(pmc[i]) );
    }

    std::cout << pmc[0].PrivateUsage << std::endl;

    for(int i = 1; i < (loopCount + 1); ++i) {
        unsigned previous = pmc[i - 1].PrivateUsage;
        unsigned current = pmc[i].PrivateUsage;

        std::cout << current << " - " << static_cast<long long>(current) - static_cast<long long>(previous) << std::endl;
    }

    return 0;
}
