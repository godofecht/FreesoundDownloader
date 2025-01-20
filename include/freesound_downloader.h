#pragma once

#include <string>
#include <optional>
#include <cstdlib>

namespace FreesoundDownloader 
{
    /**
     * @class Downloader
     * @brief Provides an interface for interacting with the Freesound API
     * 
     * This class encapsulates functionality for searching and downloading 
     * sound samples from the Freesound.org audio repository. It manages 
     * authentication, API communication, and sound resource retrieval.
     * 
     * @note Requires a valid Freesound API key for authentication
     */
    class Downloader 
    {
    public:
        /**
         * @brief Constructs a Downloader instance with API authentication
         * 
         * Initializes the Downloader by extracting the API key from:
         * 1. Explicitly provided key
         * 2. FREESOUND_API_KEY environment variable
         * 
         * @param api_key Optional API key for Freesound authentication
         * @throws std::invalid_argument If no valid API key is found
         */
        explicit Downloader(
            const std::string& api_key = 
                std::getenv("FREESOUND_API_KEY") ? 
                std::getenv("FREESOUND_API_KEY") : ""
        );

        /**
         * @brief Downloads a sound file by its unique identifier
         * 
         * Retrieves and saves a sound sample from Freesound using 
         * its specific sound ID.
         * 
         * @param sound_id Unique identifier of the sound to download
         * @param output_path Filesystem path where the sound will be saved
         * @return bool Indicates successful download operation
         */
        bool downloadSound(
            int sound_id, 
            const std::string& output_path
        );

        /**
         * @brief Performs a text-based search for sound samples
         * 
         * Executes a query against the Freesound API to retrieve 
         * sound samples matching the specified search criteria.
         * 
         * @param query Text-based search term for sound discovery
         * @param page Page number of search results (default: 1)
         * @param page_size Number of results per page (default: 15)
         * @return std::optional<std::string> JSON-formatted search results
         */
        std::optional<std::string> searchSounds(
            const std::string& query, 
            int page = 1, 
            int page_size = 15
        );

        /**
         * @brief Advanced search for sounds with multiple filtering and sorting options
         * 
         * @param query Base search query string
         * @param filter Advanced filter string (e.g., "duration:[0 TO 30] type:wav")
         * @param sort Sorting criteria (e.g., "score" or "duration_desc")
         * @param page Result pagination index
         * @param page_size Maximum number of results per request
         * @param group_by_pack Group results by sound pack
         * @param weights Custom field weights for query matching
         * @return std::optional<std::string> Parsed search result payload
         */
        std::optional<std::string> searchSounds(
            const std::string& query,
            const std::optional<std::string>& filter = std::nullopt,
            const std::optional<std::string>& sort = std::nullopt,
            int page = 1,
            int page_size = 15,
            bool group_by_pack = false,
            const std::optional<std::string>& weights = std::nullopt
        );

    private:
        /// Stores the authenticated API key for Freesound requests
        std::string m_api_key;

        /// Base URL for Freesound API endpoints
        static const std::string BASE_URL;
    };
}
