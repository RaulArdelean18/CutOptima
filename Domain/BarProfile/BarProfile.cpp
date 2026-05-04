#include "BarProfile.h"
#include <stdexcept>
#include <utility>

BarProfile::BarProfile() : standardLength(0.0), purchasePrice(0.0) {}

BarProfile::BarProfile(std::string id, std::string name, std::string barcode,
                       const Material &material, double standardLength,
                       double purchasePrice)
    : id(std::move(id)), name(std::move(name)), barcode(std::move(barcode)),
      material(material), standardLength(standardLength),
      purchasePrice(purchasePrice) {
    if (this->id.empty()) {
        throw std::invalid_argument("BarProfile id cannot be empty.");
    }
    if (this->name.empty()) {
        throw std::invalid_argument("BarProfile name cannot be empty.");
    }
    if (this->barcode.empty()) {
        throw std::invalid_argument("BarProfile barcode cannot be empty.");
    }
    if (this->standardLength <= 0) {
        throw std::invalid_argument("Standard length must be positive.");
    }
    if (this->purchasePrice < 0) {
        throw std::invalid_argument("Purchase price cannot be negative.");
    }
}

const std::string &BarProfile::getId() const { return id; }

const std::string &BarProfile::getName() const { return name; }

const std::string &BarProfile::getBarcode() const { return barcode; }

const Material &BarProfile::getMaterial() const { return material; }

double BarProfile::getStandardLength() const { return standardLength; }

double BarProfile::getPurchasePrice() const { return purchasePrice; }

void BarProfile::setName(const std::string &newName) {
    if (newName.empty()) {
        throw std::invalid_argument("BarProfile name cannot be empty.");
    }
    name = newName;
}

void BarProfile::setBarcode(const std::string &newBarcode) {
    if (newBarcode.empty()) {
        throw std::invalid_argument("Barcode cannot be empty.");
    }
    barcode = newBarcode;
}

void BarProfile::setStandardLength(double newLength) {
    if (newLength <= 0) {
        throw std::invalid_argument("Standard length must be positive.");
    }
    standardLength = newLength;
}

void BarProfile::setPurchasePrice(double newPrice) {
    if (newPrice < 0) {
        throw std::invalid_argument("Purchase price cannot be negative.");
    }
    purchasePrice = newPrice;
}