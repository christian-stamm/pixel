#pragma once
#include "pixutils/logger.hpp"

#include <any>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using Registry = std::unordered_map<std::string, std::any>;

class Config {
  public:
    bool hasKey(const std::string& key) const
    {
        return registry.contains(key);
    }

    template <typename T> void set(const std::string& key, const T& value)
    {
        registry[key] = value;
    }

    template <typename T> const T& get(const std::string& key, const std::optional<T>& fallback = std::nullopt) const
    {
        if (!hasKey(key)) {
            if (fallback.has_value()) {
                set<const T&>(key, fallback.value());
            }
            else {
                throw std::runtime_error("Key not found");
            }
        }

        return get<const T&>(key);
    }

    const std::vector<std::string> parameters() const
    {
        std::vector<std::string> keys;
        keys.reserve(registry.size());

        for (const auto& pair : registry) {
            keys.emplace_back(pair.first);
        }

        return keys;
    }

    friend std::ostream& operator<<(std::ostream& os, const Config& obj)
    {
        os << "Config(" << std::endl;

        for (const std::string& key : obj.parameters()) {
            os << key << std::endl;
        }

        os << ")";
        return os;
    }

  private:
    Registry      registry;
    static Logger logger;
};

inline Logger Config::logger = Logger::getLogger("Config");