#include "Material.h"
#include <stdexcept>

Material::Material() : defaultCutLoss(0.0), reusable(false) {}

Material::Material(std::string id, std::string name, double defaultCutLoss,
                   bool reusable)
    : id(std::move(id)), name(std::move(name)), defaultCutLoss(defaultCutLoss),
      reusable(reusable) {
    if (this->id.empty()) {
        throw std::invalid_argument("Material id cannot be empty.");
    }
    if (this->name.empty()) {
        throw std::invalid_argument("Material name cannot be empty.");
    }
    if (this->defaultCutLoss < 0) {
        throw std::invalid_argument("Cut loss cannot be negative.");
    }
}

const std::string &Material::getId() const { return id; }

const std::string &Material::getName() const { return name; }

double Material::getDefaultCutLoss() const { return defaultCutLoss; }

bool Material::isReusable() const { return reusable; }

void Material::setName(const std::string &newName) {
    if (newName.empty()) {
        throw std::invalid_argument("Material name cannot be empty.");
    }
    name = newName;
}

void Material::setDefaultCutLoss(double newCutLoss) {
    if (newCutLoss < 0) {
        throw std::invalid_argument("Cut loss cannot be negative.");
    }
    defaultCutLoss = newCutLoss;
}

void Material::setReusable(bool value) { reusable = value; }