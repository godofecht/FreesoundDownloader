#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "freesound_downloader.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>

std::string loadApiKeyFromEnvFile() {
    // Construct the path to .env.local file
    std::filesystem::path env_path = 
        std::filesystem::current_path().parent_path() / ".env.local";
    
    // Open the file
    std::ifstream env_file(env_path);
    if (!env_file.is_open()) {
        std::cerr << "Failed to open .env.local file at: " << env_path << std::endl;
        return "";
    }

    // Read the file line by line
    std::string line;
    while (std::getline(env_file, line)) {
        // Find the FREESOUND_API_KEY line
        if (line.find("FREESOUND_API_KEY=") == 0) {
            // Extract the API key value
            std::string api_key = line.substr(std::string("FREESOUND_API_KEY=").length());
            
            // Remove any whitespace or quotes
            api_key.erase(0, api_key.find_first_not_of(" \t\"'"));
            api_key.erase(api_key.find_last_not_of(" \t\"'") + 1);
            
            return api_key;
        }
    }

    std::cerr << "FREESOUND_API_KEY not found in .env.local" << std::endl;
    return "";
}

TEST_CASE("Downloader Initialization") {
    // Preserve the original API key environment variable state
    const char* original_api_key = std::getenv("FREESOUND_API_KEY");
    
    // Temporarily set a test API key for validation
    #ifdef _WIN32
    _putenv_s("FREESOUND_API_KEY", "test_api_key");
    #else
    setenv("FREESOUND_API_KEY", "test_api_key", 1);
    #endif
    
    try {
        // Validate Downloader constructor with environment variable configuration
        FreesoundDownloader::Downloader downloader;
        CHECK(true);
    } 
    catch (const std::exception& e) {
        // Fail the test if constructor throws an unexpected exception
        CHECK_FALSE(true);
    }
    
    // Restore the original environment variable configuration
    if (original_api_key) {
        #ifdef _WIN32
        _putenv_s("FREESOUND_API_KEY", original_api_key);
        #else
        setenv("FREESOUND_API_KEY", original_api_key, 1);
        #endif
    }
}

TEST_CASE("Search Impulse Response Sounds") {
    // Try to load API key from .env.local file
    std::string api_key = loadApiKeyFromEnvFile();
    
    // Extremely detailed API key validation
    REQUIRE_MESSAGE(!api_key.empty(), "FREESOUND_API_KEY MUST be set in .env.local");
    
    // Trim and validate API key
    std::string trimmed_key = api_key;
    trimmed_key.erase(0, trimmed_key.find_first_not_of(" \t\n\r"));
    trimmed_key.erase(trimmed_key.find_last_not_of(" \t\n\r") + 1);
    
    // Print detailed API key information
    std::cout << "API Key Diagnostic Information:" << std::endl;
    std::cout << "  Raw Key Length: " << api_key.length() << std::endl;
    std::cout << "  Trimmed Key Length: " << trimmed_key.length() << std::endl;
    std::cout << "  First 3 chars: " << trimmed_key.substr(0, 3) << "..." << std::endl;
    std::cout << "  Last 3 chars: ..." << trimmed_key.substr(trimmed_key.length() - 3) << std::endl;
    
    // Initialize Downloader with authenticated API credentials
    FreesoundDownloader::Downloader downloader(trimmed_key);
    
    // Perform search query for acoustic impulse response samples
    auto search_result = downloader.searchSounds(
        "piano", 
        "duration:[0 TO 30]",  // Sounds between 0-30 seconds
        "score",                // Sort by relevance score
        1,                      // First page
        15                      // 15 results per page
    );

    // Validate search results
    REQUIRE(search_result.has_value());

    // Parse JSON response
    nlohmann::json search_json;
    try {
        search_json = nlohmann::json::parse(*search_result);
    } catch (const std::exception& e) {
        FAIL("Failed to parse JSON response: " << e.what());
    }

    // Validate JSON structure
    REQUIRE(search_json.contains("count"));
    REQUIRE(search_json.contains("results"));
    REQUIRE(search_json["results"].is_array());

    // Check for at least one result
    CHECK(search_json["count"].get<int>() > 0);
    CHECK(search_json["results"].size() > 0);

    // Optional: Validate first result structure
    if (!search_json["results"].empty()) {
        auto first_result = search_json["results"][0];
        CHECK(first_result.contains("id"));
        CHECK(first_result.contains("name"));
        CHECK(first_result.contains("username"));
        CHECK(first_result.contains("duration"));
    }
}

TEST_CASE("Advanced Sound Search") {
    // Try to load API key from .env.local file
    std::string api_key = loadApiKeyFromEnvFile();
    
    // Extremely detailed API key validation
    REQUIRE_MESSAGE(!api_key.empty(), "FREESOUND_API_KEY MUST be set in .env.local");
    
    // Trim and validate API key
    std::string trimmed_key = api_key;
    trimmed_key.erase(0, trimmed_key.find_first_not_of(" \t\n\r"));
    trimmed_key.erase(trimmed_key.find_last_not_of(" \t\n\r") + 1);
    
    // Use the trimmed key
    FreesoundDownloader::Downloader downloader(trimmed_key);

    // Basic advanced search
    auto advancedSearchResults = downloader.searchSounds(
        "guitar", 
        "type:wav duration:[10 TO 60]",  // WAV files between 10-60 seconds
        "num_downloads_desc",            // Sort by most downloaded
        1,                               // First page
        20,                              // 20 results per page
        true,                            // Group by pack
        "tag:4,description:3"            // Custom field weights
    );

    // Validate search results
    REQUIRE(advancedSearchResults.has_value());

    // Parse JSON response
    nlohmann::json search_json;
    try {
        search_json = nlohmann::json::parse(*advancedSearchResults);
    } catch (const std::exception& e) {
        FAIL("Failed to parse JSON response: " << e.what());
    }

    // Validate JSON structure
    REQUIRE(search_json.contains("count"));
    REQUIRE(search_json.contains("results"));
    REQUIRE(search_json["results"].is_array());

    // Check for at least one result
    CHECK(search_json["count"].get<int>() > 0);
    CHECK(search_json["results"].size() > 0);

    // Optional: Validate first result structure
    if (!search_json["results"].empty()) {
        auto first_result = search_json["results"][0];
        CHECK(first_result.contains("id"));
        CHECK(first_result.contains("name"));
        CHECK(first_result.contains("username"));
        CHECK(first_result.contains("duration"));
        
        // Verify WAV file constraints
        if (first_result.contains("duration")) {
            double duration = first_result["duration"].get<double>();
            CHECK(duration >= 10.0);
            CHECK(duration <= 60.0);
        }
    }
}
