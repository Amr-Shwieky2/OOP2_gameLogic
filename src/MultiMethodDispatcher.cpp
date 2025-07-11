#include "MultiMethodDispatcher.h"
#include <iostream>

// Debug function to print all registered handlers for a domain
void MultiMethodDispatcher::debugPrintHandlers(const std::string& domain) const {
    std::cout << "Registered handlers for domain '" << domain << "':" << std::endl;
    
    auto domainIt = m_domains.find(domain);
    if (domainIt == m_domains.end()) {
        std::cout << "  No handlers registered for this domain." << std::endl;
        return;
    }
    
    for (const auto& key : domainIt->second) {
        std::cout << "  " << key.type1.name() << " <-> " << key.type2.name() << std::endl;
    }
    
    // Check for result handlers too
    std::string resultDomain = domain + "_result";
    auto resultDomainIt = m_resultHandlers.find(resultDomain);
    
    if (resultDomainIt != m_resultHandlers.end()) {
        std::cout << "Result handlers for domain '" << domain << "':" << std::endl;
        for (const auto& [typeKey, wrapper] : resultDomainIt->second) {
            std::cout << "  " << typeKey.first.name() 
                      << " <-> " << typeKey.second.name() 
                      << " -> " << wrapper.resultType.name() << std::endl;
        }
    }
}