#include "InMemoryMaterialRepository.h"
#include <stdexcept>

int InMemoryMaterialRepository::findIndexById(const std::string &id) const {
    for (int i = 0; i < static_cast<int>(materials.size()); i++) {
        if (materials[i].getId() == id) {
            return i;
        }
    }
    return -1;
}

void InMemoryMaterialRepository::save(const Material &material) {
    if (exists(material.getId())) {
        throw std::runtime_error("Material with the same id already exists.");
    }
    materials.push_back(material);
}

void InMemoryMaterialRepository::update(const Material &material) {
    int index = findIndexById(material.getId());
    if (index == -1) {
        throw std::runtime_error("Material not found.");
    }
    materials[index] = material;
}

std::optional<Material>
InMemoryMaterialRepository::findById(const std::string &id) const {
    int index = findIndexById(id);
    if (index == -1) {
        return std::nullopt;
    }
    return materials[index];
}

std::vector<Material> InMemoryMaterialRepository::findAll() const {
    return materials;
}

bool InMemoryMaterialRepository::exists(const std::string &id) const {
    return findIndexById(id) != -1;
}