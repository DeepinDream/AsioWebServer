#include "LogFileSlicer.h"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

LogFileSlicer::LogFileSlicer(const std::string& filename,
                             const std::size_t MaxSize)
    : kMaxSize_(MaxSize)
    , count_(0)
    , basename_(filename)
{
}

std::string LogFileSlicer::getLogName()
{
    fs::path p(basename_);
    auto size = fs::file_size(p);
    if (size >= kMaxSize_) {
        count_++;
        std::string reName = p.stem().native() + "_" + std::to_string(count_) +
                             p.extension().native();
        p.replace_filename(reName);
        // p.filename();
    }

    return p;
}