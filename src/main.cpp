#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <windows.h>
#include <intrin.h>

void QueryCPU() {
    int cpuInfo[4] = {0};
    __cpuid(cpuInfo, 0);
    int nIds = cpuInfo[0];

    char vendor[13] = {0};
    *reinterpret_cast<int*>(vendor) = cpuInfo[1];
    *reinterpret_cast<int*>(vendor + 4) = cpuInfo[3];
    *reinterpret_cast<int*>(vendor + 8) = cpuInfo[2];

    char brand[49] = {0};
    __cpuid(cpuInfo, 0x80000000);
    unsigned int nExIds = cpuInfo[0];
    if (nExIds >= 0x80000004) {
        __cpuid(reinterpret_cast<int*>(brand), 0x80000002);
        __cpuid(reinterpret_cast<int*>(brand + 16), 0x80000003);
        __cpuid(reinterpret_cast<int*>(brand + 32), 0x80000004);
    }

    std::cout << "[ CPU INFORMATION ]\n";
    std::cout << "  Vendor         : " << vendor << "\n";
    std::cout << "  Processor      : " << (brand[0] ? brand : "Unknown") << "\n";

    SYSTEM_INFO sysInfo;
    GetNativeSystemInfo(&sysInfo);
    std::cout << "  Logical Cores  : " << sysInfo.dwNumberOfProcessors << "\n";
    std::cout << "  Architecture   : " << (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? "x86_64" : "x86") << "\n";

    if (nIds >= 1) {
        __cpuid(cpuInfo, 1);
        int ecx = cpuInfo[2];
        int edx = cpuInfo[3];

        std::cout << "\n[ INSTRUCTION SET EXTENSIONS ]\n";
        auto check = [](const char* name, bool supported) {
            std::cout << "  " << std::left << std::setw(12) << name << ": " 
                      << (supported ? "[+] YES" : "[-] NO") << "\n";
        };
        check("SSE", (edx & (1 << 25)) != 0);
        check("SSE2", (edx & (1 << 26)) != 0);
        check("SSE3", (ecx & (1 << 0)) != 0);
        check("SSSE3", (ecx & (1 << 9)) != 0);
        check("SSE4.1", (ecx & (1 << 19)) != 0);
        check("SSE4.2", (ecx & (1 << 20)) != 0);
        check("AVX", (ecx & (1 << 28)) != 0);
        check("AES-NI", (ecx & (1 << 25)) != 0);
        check("RDRAND", (ecx & (1 << 30)) != 0);

        if (nIds >= 7) {
            __cpuidex(cpuInfo, 7, 0);
            int ebx7 = cpuInfo[1];
            check("AVX2", (ebx7 & (1 << 5)) != 0);
            check("AVX-512F", (ebx7 & (1 << 16)) != 0);
            check("SHA-NI", (ebx7 & (1 << 29)) != 0);
        }
    }
}

void QueryMemory() {
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    if (GlobalMemoryStatusEx(&mem)) {
        double totalGB = mem.ullTotalPhys / (1024.0 * 1024.0 * 1024.0);
        double availGB = mem.ullAvailPhys / (1024.0 * 1024.0 * 1024.0);
        std::cout << "\n[ MEMORY STATUS ]\n";
        std::cout << "  Total Physical : " << std::fixed << std::setprecision(2) << totalGB << " GB\n";
        std::cout << "  Available      : " << availGB << " GB\n";
        std::cout << "  Memory Load    : " << mem.dwMemoryLoad << "%\n";
    }
}

void QueryDisks() {
    DWORD drives = GetLogicalDrives();
    std::cout << "\n[ STORAGE DRIVES ]\n";
    std::cout << std::left << std::setw(8) << "Drive" 
              << std::setw(15) << "Type" 
              << std::setw(15) << "Total Space" 
              << "Free Space\n";
    std::cout << std::string(55, '-') << "\n";

    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        if (drives & (1 << (letter - 'A'))) {
            std::string rootPath = std::string(1, letter) + ":\\";
            UINT type = GetDriveTypeA(rootPath.c_str());
            const char* typeStr = "Unknown";
            switch (type) {
                case DRIVE_FIXED: typeStr = "Fixed (HDD/SSD)"; break;
                case DRIVE_REMOVABLE: typeStr = "Removable (USB)"; break;
                case DRIVE_REMOTE: typeStr = "Network Drive"; break;
                case DRIVE_CDROM: typeStr = "CD/DVD-ROM"; break;
                case DRIVE_RAMDISK: typeStr = "RAM Disk"; break;
            }

            ULARGE_INTEGER freeBytes, totalBytes, totalFree;
            if (GetDiskFreeSpaceExA(rootPath.c_str(), &freeBytes, &totalBytes, &totalFree)) {
                double totalGB = totalBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
                double freeGB = freeBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
                std::cout << std::left << std::setw(8) << rootPath
                          << std::setw(15) << typeStr
                          << std::fixed << std::setprecision(1) << totalGB << " GB        "
                          << freeGB << " GB\n";
            }
        }
    }
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "          HARDWARE & SYSTEM DIAGNOSTIC TOOL             \n";
    std::cout << "========================================================\n\n";
    QueryCPU();
    QueryMemory();
    QueryDisks();
    std::cout << "\n========================================================\n";
    return 0;
}
