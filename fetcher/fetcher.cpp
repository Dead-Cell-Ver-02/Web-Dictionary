#include "fetcher.h" // Assuming the header is in the same directory

WordData fetchWordData(const std::string &wordToSearch) {
    WordData data;

    // Set default values for error cases
    data.word = "Not Found";
    data.phonetic = "/not_found/";

    std::string url = "https://api.dictionaryapi.dev/api/v2/entries/en/" + wordToSearch;
    cpr::Response r = cpr::Get(cpr::Url{url});

    if (r.status_code != 200) {
        std::cerr << "Error fetching data: " << r.status_code << std::endl;
        data.definitionList.push_back("Failed to fetch data from the API.");
        return data;
    }

    try {
        // FIX 1: Corrected library name 'nlohmann'
        // FIX 2: Consistent variable name 'jsonData'
        auto jsonData = nlohmann::json::parse(r.text);

        if (!jsonData.empty() && jsonData.is_array()) {
            auto entry = jsonData[0];

            // fetching word
            if (entry.contains("word") && entry["word"].is_string()) {
                data.word = entry["word"].get<std::string>();
            }

            // fetching phonetics
            if (entry.contains("phonetic") && entry["phonetic"].is_string()) {
                data.phonetic = entry["phonetic"].get<std::string>();
            } else if (entry.contains("phonetics") && !entry["phonetics"].empty()) {
                for (const auto &phonetic_obj : entry["phonetics"]) {
                    if (phonetic_obj.contains("text") && phonetic_obj["text"].is_string()) {
                        data.phonetic = phonetic_obj["text"].get<std::string>();
                        if (!data.phonetic.empty()) break;
                    }
                }
            }

            if (data.phonetic.empty())
                data.phonetic = "-";

            // fetching parts of speech and definitions
            if (entry.contains("meanings") && entry["meanings"].is_array()) {
                std::set<std::string> uniquePosSet;

                for (const auto &meaning : entry["meanings"]) {
                    if (meaning.contains("partOfSpeech") && meaning["partOfSpeech"].is_string()) {
                        // FIX 3: Declared the 'pos' variable
                        std::string pos = "_" + meaning["partOfSpeech"].get<std::string>();
                        uniquePosSet.insert(pos);
                    }

                    if (meaning.contains("definitions") && meaning["definitions"].is_array()) {
                        for (const auto &defObject : meaning["definitions"]) {
                            if (defObject.contains("definition") && defObject["definition"].is_string()) {
                                std::string definitionText = defObject["definition"].get<std::string>();
                                data.definitionList.push_back(definitionText);
                            }
                        }
                    }
                }
                
                // FIX 4 (Logical Error): Moved this line outside the loop
                data.posList = std::vector<std::string>(uniquePosSet.begin(), uniquePosSet.end());
            }
        }
    }
    // FIX 5: Added a specific 'catch' block for JSON errors
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        data.definitionList.push_back("Failed to parse the response from the API.");
    }

    // FIX 6: Added the final 'return' statement for the success path
    return data;
}
