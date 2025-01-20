/**
 * @file src/freesound_downloader.cpp
 * @brief Implementation of the Downloader class
 * 
 * This file contains the implementation of the Downloader class, which provides 
 * an interface for interacting with the Freesound API.
 * 
 * @author Abhishek Shivakumar
 * @date 2025-20-1
 * @see include/freesound_downloader.h
 */

#include "freesound_downloader.h"
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <iostream>

namespace FreesoundDownloader 
{
    const std::string Downloader::BASE_URL = "https://freesound.org/apiv2/";

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
    Downloader::Downloader(const std::string& api_key)
        : m_api_key(api_key)
    {
        if (m_api_key.empty()) 
        {
            const char* env_key = std::getenv("FREESOUND_API_KEY");
            if (env_key) 
            {
                m_api_key = env_key;
            }

            if (m_api_key.empty()) 
            {
                throw std::invalid_argument(
                    "Freesound API authentication failed. "
                    "Provide API key via constructor or FREESOUND_API_KEY environment variable."
                );
            }
        }
    }

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
    bool Downloader::downloadSound(
        int sound_id, 
        const std::string& output_path
    )
    {
        std::string download_url = BASE_URL + "sounds/" 
            + std::to_string(sound_id) + "/download/";
        
        auto response = cpr::Get(
            cpr::Url{download_url},
            cpr::Parameters{{"token", m_api_key}}
        );

        if (response.status_code != 200) 
        {
            return false;
        }

        std::ofstream out_file(output_path, std::ios::binary);
        if (!out_file) 
        {
            return false;
        }
        
        out_file.write(response.text.c_str(), response.text.size());
        out_file.close();

        return true;
    }

    /**
     * @brief Performs a text-based search for sound samples
     * 
     * Executes a query against the Freesound API to retrieve 
     * sound samples matching the specified search criteria.
     * 
     * @param query Text-based search term for sound discovery
     * @param page Page number of search results (default: 1)
     * @param page_size Number of results per page (default: 15)
     * @return std::optional<std::string> JSON response from the Freesound API
     */
    std::optional<std::string> Downloader::searchSounds(
        const std::string& query, 
        int page, 
        int page_size
    )
    {
        std::string search_url = BASE_URL + "search/text/";
        
        auto response = cpr::Get(
            cpr::Url{search_url},
            cpr::Parameters{
                {"query", query},
                {"token", m_api_key},
                {"page", std::to_string(page)},
                {"page_size", std::to_string(page_size)}
            }
        );

        if (response.status_code != 200) 
        {
            return std::nullopt;
        }

        return response.text;
    }

    /**
     * @brief Performs a text-based search for sound samples with advanced options
     * 
     * Executes a query against the Freesound API to retrieve 
     * sound samples matching the specified search criteria.
     * 
     * @param query Text-based search term for sound discovery
     * @param filter Optional filter for search results
     * @param sort Optional sorting criteria for search results
     * @param page Page number of search results (default: 1)
     * @param page_size Number of results per page (default: 15)
     * @param group_by_pack Whether to group results by pack
     * @param weights Optional weights for ranking search results
     * @return std::optional<std::string> JSON response from the Freesound API
     */
    std::optional<std::string> Downloader::searchSounds(
        const std::string& query,
        const std::optional<std::string>& filter,
        const std::optional<std::string>& sort,
        int page,
        int page_size,
        bool group_by_pack,
        const std::optional<std::string>& weights
    )
    {
        std::string search_url = BASE_URL + "search/text/";
        
        cpr::Parameters params{
            {"query", query},
            {"page", std::to_string(page)},
            {"page_size", std::to_string(page_size)},
            {"fields", "id,name,username,description,tags,preview-hq-mp3,duration"}
        };

        if (filter) {
            params.Add({"filter", *filter});
        }

        if (sort) {
            params.Add({"sort", *sort});
        }

        params.Add({"group_by_pack", group_by_pack ? "1" : "0"});

        if (weights) {
            params.Add({"weights", *weights});
        }

        try {
            auto response = cpr::Get(
                cpr::Url{search_url},
                cpr::Parameters{params},
                cpr::Header{
                    {"Authorization", "Token " + m_api_key},
                    {"Content-Type", "application/json"}
                },
                cpr::Timeout{10000}  // 10-second timeout
            );

            if (response.status_code == 200) {
                return response.text;
            } else {
                std::cerr << "FREESOUND API ERROR:" << std::endl;
                std::cerr << "Status Code: " << response.status_code << std::endl;
                std::cerr << "Full Response: " << response.text << std::endl;
                return std::nullopt;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "EXCEPTION DURING FREESOUND SEARCH:" << std::endl;
            std::cerr << e.what() << std::endl;
            return std::nullopt;
        }
    }
}
