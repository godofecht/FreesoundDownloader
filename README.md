# Freesound Downloader Library

## Overview
A C++ library for downloading and searching sounds from the Freesound API using CMake's FetchContent.

## Dependencies
- CMake 3.20+
- C++17 Compiler
- Freesound API Key

## Dependencies Management
To include this library in your CMake project, use FetchContent:

```cmake
# In your CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(YourProject)

# Enable FetchContent to manage dependencies
include(FetchContent)

# Fetch FreesoundDownloader library
FetchContent_Declare(
    FreesoundDownloader
    GIT_REPOSITORY https://github.com/YourUsername/FreesoundDownloader.git
    GIT_TAG main  # Or specify a specific commit/tag
)

# Make the library available
FetchContent_MakeAvailable(FreesoundDownloader)

# Add your executable and link the library
add_executable(YourApp main.cpp)
target_link_libraries(YourApp FreesoundDownloader)
```

### Required External Dependencies
The library automatically fetches and builds:
- `cpr`: C++ HTTP library
- `nlohmann/json`: JSON parsing library
- `doctest`: Unit testing framework

## Usage
```cpp
FreesoundDownloader::Downloader downloader("YOUR_API_KEY");
auto searchResults = downloader.searchSounds("bird");
downloader.downloadSound(12345, "output.wav");
```

## Setup
1. Get a Freesound API key from [Freesound.org](https://freesound.org/apiv2/)
2. Include this library in your CMake project using FetchContent
3. Link against `FreesoundDownloader`

## Building
```bash
mkdir build && cd build
cmake ..
make
```

## Note
Requires a valid Freesound API key for authentication.

## API Key Management
To use the library, you need to provide a valid Freesound API key. You can do this by creating a `.env.local` file in your project's root directory with the following content:

```bash
FREESOUND_API_KEY=your_api_key_here
# Replace 'your_api_key_here' with instructions for other developers
# Example: Get your API key from https://freesound.org/apiv2/
# Do NOT commit real API keys to version control

LOG_LEVEL=info
MAX_DOWNLOAD_RETRY=3

# Finally, rename the file to .env.local
```
