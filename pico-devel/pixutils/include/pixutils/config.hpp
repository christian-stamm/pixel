#pragma once
#include <any>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Config {
  public:
    Config merge(const Config& other)
    {
        Config merged = *this;

        for (const auto& [key, value] : other.registry) {
            merged.set(key, value);
        }

        return merged;
    }

    bool hasKey(const std::string& key) const
    {
        return registry.contains(key);
    }

    template <typename T> void set(const std::string& param, T value)
    {
        registry[param] = value;
    }

    template <typename T> T get(const std::string& param, const std::optional<T>& fallback = std::nullopt) const
    {
        if (!hasKey(param) && fallback.has_value()) {
            return fallback.value();
        }

        try {
            return std::any_cast<T>(registry.at(param));
        }
        catch (const std::out_of_range& e) {
            throw std::runtime_error(std::format("KeyNotFound for param '{}'", param));
        }
        catch (const std::bad_any_cast& e) {
            throw std::runtime_error(std::format("TypeMismatch for param '{}'", param));
        }
    }

    const std::vector<std::string> getParams() const
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
        for (const std::string& key : obj.getParams()) {
            os << '"' << key << '"' << std::endl;
        }
        os << ")";
        return os;
    }

  private:
    std::unordered_map<std::string, std::any> registry;
};