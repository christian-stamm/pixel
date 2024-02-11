#pragma once
#include "pixtools/logger.hpp"

#include <any>
#include <iostream>
#include <sstream>
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

    template <typename T> const T& get(const std::string& key) const
    {
        try {
            return std::any_cast<const T&>(registry.at(key));
        }
        catch (const std::out_of_range& e) {
            logger.error() << "Param" << key << ":" << e.what() << "(KeyNotFound)";
            throw std::runtime_error("KeyNotFound");
        }
        catch (const std::bad_any_cast& e) {
            logger.error() << "Param" << key << ":" << e.what() << "(TypeMismatch)";
            throw std::runtime_error("TypeMismatch");
        }
    }

    template <typename T> const T& get(const std::string& key, const T& fallback) const
    {
        if (!hasKey(key)) {
            set<const T&>(key, fallback);
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

    const std::string to_string() const
    {
        std::stringstream ss;

        for (const std::string& key : parameters()) {
            ss << key << std::endl;
        }

        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Config& obj)
    {
        os << "Config(" << std::endl;
        os << obj.to_string();
        os << ")";
        return os;
    }

  private:
    Registry      registry;
    static Logger logger;
};

inline Logger Config::logger = Logger::getLogger("Config");