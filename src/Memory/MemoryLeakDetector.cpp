#include "Memory/MemoryLeakDetector.h"
#include <ctime>
#include <iomanip>

#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif

MemoryLeakDetector& MemoryLeakDetector::getInstance() {
    static MemoryLeakDetector instance;
    return instance;
}

MemoryLeakDetector::MemoryLeakDetector() {
    std::cout << "[MemoryLeakDetector] Initialized" << std::endl;
}

MemoryLeakDetector::~MemoryLeakDetector() {
    // Check for leaks on destruction
    if (!m_allocations.empty()) {
        std::cerr << "[WARNING] Memory leaks detected at program exit!" << std::endl;
        checkForLeaks(true, "exit_memory_leaks.txt");
    }
}

void MemoryLeakDetector::trackAllocation(void* ptr, size_t size, 
                                         const std::string& type,
                                         const char* file, int line) {
    if (!m_enabled || !ptr) return;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Create allocation info
    AllocationInfo info;
    info.size = size;
    info.type = type;
    info.file = file;
    info.line = line;
    info.stackTrace = captureStackTrace();
    
    // Track allocation
    m_allocations[ptr] = info;
    
    // Update type statistics
    m_typeStats[type] += size;
}

void MemoryLeakDetector::trackDeallocation(void* ptr) {
    if (!m_enabled || !ptr) return;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Find the allocation
    auto it = m_allocations.find(ptr);
    if (it != m_allocations.end()) {
        // Update type statistics
        m_typeStats[it->second.type] -= it->second.size;
        
        // Remove the allocation from tracking
        m_allocations.erase(it);
    }
}

size_t MemoryLeakDetector::getAllocationCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_allocations.size();
}

bool MemoryLeakDetector::checkForLeaks(bool outputToFile, const std::string& filename) {
    if (!m_enabled) return true;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_allocations.empty()) {
        std::cout << "[MemoryLeakDetector] No memory leaks detected" << std::endl;
        return true;
    }
    
    std::cout << "[MemoryLeakDetector] " << m_allocations.size() 
              << " memory leaks detected!" << std::endl;
    
    if (outputToFile) {
        std::ofstream file(filename);
        if (file.is_open()) {
            dumpLeaks(file);
            std::cout << "[MemoryLeakDetector] Leak report written to " << filename << std::endl;
        }
        else {
            std::cerr << "[MemoryLeakDetector] Failed to open file: " << filename << std::endl;
            dumpLeaks(std::cerr);
        }
    }
    else {
        dumpLeaks(std::cout);
    }
    
    return false;
}

void MemoryLeakDetector::printMemorySummary() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    size_t totalSize = 0;
    for (const auto& [type, size] : m_typeStats) {
        totalSize += size;
    }
    
    std::cout << "===== Memory Usage Summary =====" << std::endl;
    std::cout << "Total allocations: " << m_allocations.size() << std::endl;
    std::cout << "Total memory: " << totalSize << " bytes" << std::endl;
    
    // Sort types by memory usage
    std::vector<std::pair<std::string, size_t>> sortedTypes(m_typeStats.begin(), m_typeStats.end());
    std::sort(sortedTypes.begin(), sortedTypes.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::cout << "\n--- Memory Usage by Type ---" << std::endl;
    std::cout << std::left << std::setw(30) << "Type"
              << std::right << std::setw(15) << "Bytes"
              << std::setw(10) << "%" << std::endl;
    
    for (const auto& [type, size] : sortedTypes) {
        float percentage = totalSize > 0 ? (100.0f * size / totalSize) : 0.0f;
        std::cout << std::left << std::setw(30) << type
                  << std::right << std::setw(15) << size
                  << std::setw(9) << std::fixed << std::setprecision(1) << percentage << "%" << std::endl;
    }
    
    std::cout << "===============================" << std::endl;
}

void MemoryLeakDetector::reset() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_allocations.clear();
    m_typeStats.clear();
    std::cout << "[MemoryLeakDetector] Tracking data reset" << std::endl;
}

void MemoryLeakDetector::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_enabled = enabled;
    std::cout << "[MemoryLeakDetector] " << (enabled ? "Enabled" : "Disabled") << std::endl;
}

bool MemoryLeakDetector::isEnabled() const {
    return m_enabled;
}

void MemoryLeakDetector::dumpLeaks(std::ostream& stream) {
    // Output header with timestamp
    auto now = std::time(nullptr);
    stream << "Memory Leak Report - " << std::ctime(&now) << std::endl;
    stream << "Total leaks: " << m_allocations.size() << std::endl;
    stream << std::endl;
    
    // Calculate total leaked memory
    size_t totalSize = 0;
    for (const auto& [ptr, info] : m_allocations) {
        totalSize += info.size;
    }
    
    stream << "Total leaked memory: " << totalSize << " bytes" << std::endl;
    stream << std::endl;
    
    // Group by type for summary
    std::unordered_map<std::string, std::pair<size_t, size_t>> typeCount; // type -> (count, totalSize)
    
    for (const auto& [ptr, info] : m_allocations) {
        auto& entry = typeCount[info.type];
        entry.first++;
        entry.second += info.size;
    }
    
    // Output summary by type
    stream << "===== Leak Summary by Type =====" << std::endl;
    stream << std::left << std::setw(30) << "Type"
           << std::right << std::setw(10) << "Count"
           << std::setw(15) << "Total Size"
           << std::setw(10) << "%" << std::endl;
    
    std::vector<std::pair<std::string, std::pair<size_t, size_t>>> sortedTypes(
        typeCount.begin(), typeCount.end());
    
    std::sort(sortedTypes.begin(), sortedTypes.end(),
              [](const auto& a, const auto& b) { return a.second.second > b.second.second; });
    
    for (const auto& [type, data] : sortedTypes) {
        float percentage = totalSize > 0 ? (100.0f * data.second / totalSize) : 0.0f;
        
        stream << std::left << std::setw(30) << type
               << std::right << std::setw(10) << data.first
               << std::setw(15) << data.second
               << std::setw(9) << std::fixed << std::setprecision(1) << percentage << "%" << std::endl;
    }
    
    stream << std::endl;
    
    // Output detailed leak info
    stream << "===== Detailed Leak Information =====" << std::endl;
    int leakIndex = 1;
    
    for (const auto& [ptr, info] : m_allocations) {
        stream << "Leak #" << leakIndex++ << ":" << std::endl;
        stream << "  Address: " << ptr << std::endl;
        stream << "  Type: " << info.type << std::endl;
        stream << "  Size: " << info.size << " bytes" << std::endl;
        stream << "  Allocated at: " << info.file << ":" << info.line << std::endl;
        
        if (!info.stackTrace.empty()) {
            stream << "  Stack trace: " << std::endl;
            stream << info.stackTrace << std::endl;
        }
        
        stream << std::endl;
    }
}

std::string MemoryLeakDetector::captureStackTrace() {
#ifdef _WIN32
    // Windows stack trace implementation
    constexpr int MAX_FRAMES = 32;
    void* callstack[MAX_FRAMES];
    
    // Initialize symbol handler for process
    SymInitialize(GetCurrentProcess(), nullptr, TRUE);
    
    // Capture stack trace
    WORD frames = CaptureStackBackTrace(1, MAX_FRAMES, callstack, nullptr);
    
    std::stringstream stackTrace;
    
    for (int i = 0; i < frames; i++) {
        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
        
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        
        DWORD64 displacement = 0;
        
        if (SymFromAddr(GetCurrentProcess(), (DWORD64)callstack[i], &displacement, symbol)) {
            IMAGEHLP_LINE64 line;
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            
            DWORD displacement = 0;
            if (SymGetLineFromAddr64(GetCurrentProcess(), (DWORD64)callstack[i], &displacement, &line)) {
                stackTrace << "    " << symbol->Name << " at " 
                           << line.FileName << ":" << line.LineNumber << std::endl;
            }
            else {
                stackTrace << "    " << symbol->Name << std::endl;
            }
        }
        else {
            stackTrace << "    [Unknown function]" << std::endl;
        }
    }
    
    return stackTrace.str();
#else
    // Simple implementation for non-Windows platforms
    return "Stack trace not available on this platform";
#endif
}