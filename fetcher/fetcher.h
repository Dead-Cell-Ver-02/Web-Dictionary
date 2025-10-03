#ifndef FETCHER_H
#define FETCHER_H

#if defined(_WIN32)
#define NOGDI  // All GDI defines and routines
#define NOUSER // All USER defines and routines
#endif

#define NOMINMAX

#include <cpr/cpr.h> // or any library that uses Windows.h

#if defined(_WIN32) // raylib uses these names as function parameters
#undef near
#undef far
#endif

#ifdef PlaySound
#undef PlaySound
#endif

#include <set>
#include <iostream>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

// A struct to hold all the parsed data for a word.
struct WordData {
    std::string word;
    std::string phonetic;
    std::vector<std::string> posList;
    std::vector<std::string> definitionList;
};

// Function DECLARATION (prototype).
// The implementation is now in fetcher.cpp.
WordData fetchWordData(const std::string &wordToSearch);

#endif
