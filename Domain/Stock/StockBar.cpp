#include "StockBar.h"
#include <stdexcept>
#include <utility>

StockBar::StockBar()
    : originalLength(0.0), remainingLength(0.0),
      status(StockBarStatus::AVAILABLE) {}

StockBar::StockBar(std::string id, std::string uniqueBarcode,
                   const BarProfile &profile, double originalLength,
                   double remainingLength, StockBarStatus status)
    : id(std::move(id)), uniqueBarcode(std::move(uniqueBarcode)),
      profile(profile), originalLength(originalLength),
      remainingLength(remainingLength), status(status) {
    if (this->id.empty()) {
        throw std::invalid_argument("StockBar id cannot be empty.");
    }
    if (this->uniqueBarcode.empty()) {
        throw std::invalid_argument("StockBar barcode cannot be empty.");
    }
    if (this->originalLength <= 0) {
        throw std::invalid_argument("Original length must be positive.");
    }
    if (this->remainingLength < 0 ||
        this->remainingLength > this->originalLength) {
        throw std::invalid_argument(
            "Remaining length must be in [0, originalLength].");
    }
}

const std::string &StockBar::getId() const { return id; }

const std::string &StockBar::getUniqueBarcode() const { return uniqueBarcode; }

const BarProfile &StockBar::getProfile() const { return profile; }

double StockBar::getOriginalLength() const { return originalLength; }

double StockBar::getRemainingLength() const { return remainingLength; }

StockBarStatus StockBar::getStatus() const { return status; }

bool StockBar::canFit(double requiredLength) const {
    return status == StockBarStatus::AVAILABLE &&
           requiredLength <= remainingLength;
}

bool StockBar::consume(double usedLength) {
    if (usedLength <= 0) {
        return false;
    }
    if (!canFit(usedLength)) {
        return false;
    }

    remainingLength -= usedLength;

    if (remainingLength == 0) {
        status = StockBarStatus::USED;
    }

    return true;
}

bool StockBar::isAvailable() const {
    return status == StockBarStatus::AVAILABLE;
}

double StockBar::getUsedLength() const {
    return originalLength - remainingLength;
}

void StockBar::setStatus(StockBarStatus newStatus) { status = newStatus; }