#include "InMemoryBarProfileRepository.h"
#include <stdexcept>

int InMemoryBarProfileRepository::findIndexById(const std::string &id) const {
    for (int i = 0; i < static_cast<int>(profiles.size()); i++) {
        if (profiles[i].getId() == id) {
            return i;
        }
    }
    return -1;
}

void InMemoryBarProfileRepository::save(const BarProfile &profile) {
    if (exists(profile.getId())) {
        throw std::runtime_error("BarProfile with the same id already exists.");
    }
    profiles.push_back(profile);
}

void InMemoryBarProfileRepository::update(const BarProfile &profile) {
    int index = findIndexById(profile.getId());
    if (index == -1) {
        throw std::runtime_error("BarProfile not found.");
    }
    profiles[index] = profile;
}

std::optional<BarProfile>
InMemoryBarProfileRepository::findById(const std::string &id) const {
    int index = findIndexById(id);
    if (index == -1) {
        return std::nullopt;
    }
    return profiles[index];
}

std::vector<BarProfile> InMemoryBarProfileRepository::findAll() const {
    return profiles;
}

std::vector<BarProfile> InMemoryBarProfileRepository::findByMaterialId(
    const std::string &materialId) const {
    std::vector<BarProfile> result;

    for (const auto &profile : profiles) {
        if (profile.getMaterial().getId() == materialId) {
            result.push_back(profile);
        }
    }

    return result;
}

bool InMemoryBarProfileRepository::exists(const std::string &id) const {
    return findIndexById(id) != -1;
}