#include "Plugin/DynamicPluginLoader.h"
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
    #define LIBRARY_LOAD(path) LoadLibrary(path.c_str())
    #define LIBRARY_UNLOAD(handle) FreeLibrary((HMODULE)handle)
    #define LIBRARY_GETFUNCTION(handle, name) GetProcAddress((HMODULE)handle, name.c_str())
    #define LIBRARY_GETERROR() getWindowsErrorMessage()
    #define LIBRARY_EXTENSION ".dll"
#else
    #include <dlfcn.h>
    #define LIBRARY_LOAD(path) dlopen(path.c_str(), RTLD_NOW)
    #define LIBRARY_UNLOAD(handle) dlclose(handle)
    #define LIBRARY_GETFUNCTION(handle, name) dlsym(handle, name.c_str())
    #define LIBRARY_GETERROR() dlerror()
    #ifdef __APPLE__
        #define LIBRARY_EXTENSION ".dylib"
    #else
        #define LIBRARY_EXTENSION ".so"
    #endif
#endif

#ifdef _WIN32
std::string getWindowsErrorMessage() {
    DWORD error = GetLastError();
    if (error == 0) {
        return "No error";
    }

    LPSTR buffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, nullptr);
    
    std::string message(buffer, size);
    LocalFree(buffer);
    
    return message;
}
#endif

bool DynamicLibrary::load(const std::string& path) {
    if (isLoaded()) {
        unload();
    }
    
    m_handle = LIBRARY_LOAD(path);
    
    if (!m_handle) {
        m_lastError = LIBRARY_GETERROR();
        return false;
    }
    
    m_path = path;
    return true;
}

void DynamicLibrary::unload() {
    if (isLoaded()) {
        LIBRARY_UNLOAD(m_handle);
        m_handle = nullptr;
        m_path.clear();
    }
}

void* DynamicLibrary::getFunction(const std::string& name) {
    if (!isLoaded()) {
        m_lastError = "Library not loaded";
        return nullptr;
    }
    
    void* func = LIBRARY_GETFUNCTION(m_handle, name);
    
    if (!func) {
        m_lastError = LIBRARY_GETERROR();
    }
    
    return func;
}

bool DynamicLibrary::isLoaded() const {
    return m_handle != nullptr;
}

std::string DynamicLibrary::getLastError() const {
    return m_lastError;
}

DynamicLibrary::~DynamicLibrary() {
    unload();
}

std::unique_ptr<IPluginFactory> DynamicPluginLoader::loadPluginFactory(const std::string& path) {
    DynamicLibrary library;
    
    if (!library.load(path)) {
        std::cerr << "Failed to load plugin library: " << path << std::endl;
        std::cerr << "Error: " << library.getLastError() << std::endl;
        return nullptr;
    }
    
    // Get create factory function
    auto createFunc = reinterpret_cast<CreatePluginFactoryFunc>(library.getFunction("CreatePluginFactory"));
    
    if (!createFunc) {
        std::cerr << "Failed to get CreatePluginFactory function from: " << path << std::endl;
        std::cerr << "Error: " << library.getLastError() << std::endl;
        return nullptr;
    }
    
    // Create factory
    IPluginFactory* factory = createFunc();
    
    if (!factory) {
        std::cerr << "CreatePluginFactory returned nullptr for: " << path << std::endl;
        return nullptr;
    }
    
    // Get destroy factory function for proper cleanup
    auto destroyFunc = reinterpret_cast<DestroyPluginFactoryFunc>(library.getFunction("DestroyPluginFactory"));
    
    if (!destroyFunc) {
        std::cerr << "Failed to get DestroyPluginFactory function from: " << path << std::endl;
        std::cerr << "Error: " << library.getLastError() << std::endl;
        delete factory; // Fallback cleanup
        return nullptr;
    }
    
    // Create deleter that uses destroy function
    auto deleter = [destroyFunc](IPluginFactory* f) {
        if (f) destroyFunc(f);
    };
    
    // Return unique pointer with custom deleter
    return std::unique_ptr<IPluginFactory, decltype(deleter)>(factory, deleter);
}

std::vector<std::string> DynamicPluginLoader::discoverPluginLibraries(
    const std::filesystem::path& directory,
    const std::string& extension) {
    
    std::vector<std::string> libraryPaths;
    
    // Check if directory exists
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        std::cerr << "Plugin directory does not exist: " << directory << std::endl;
        return libraryPaths;
    }
    
    // Get file extension to search for
    std::string fileExtension = extension.empty() ? getDefaultLibraryExtension() : extension;
    
    // Iterate over directory contents
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == fileExtension) {
            libraryPaths.push_back(entry.path().string());
        }
    }
    
    return libraryPaths;
}

std::string DynamicPluginLoader::getDefaultLibraryExtension() {
    return LIBRARY_EXTENSION;
}