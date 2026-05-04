#include "InMemoryStockBarRepository.h"
#include <stdexcept>

int InMemoryStockBarRepository::findIndexById(const std::string &id) const {
    for (int i = 0; i < static_cast<int>(stockBars.size()); i++) {
        if (stockBars[i].getId() == id) {
            return i;
        }
    }
    return -1;
}

void InMemoryStockBarRepository::save(const StockBar &stockBar) {
    if (exists(stockBar.getId())) {
        throw std::runtime_error("StockBar with the same id already exists.");
    }
    stockBars.push_back(stockBar);
}

void InMemoryStockBarRepository::update(const StockBar &stockBar) {
    int index = findIndexById(stockBar.getId());
    if (index == -1) {
        throw std::runtime_error("StockBar not found.");
    }
    stockBars[index] = stockBar;
}

std::optional<StockBar>
InMemoryStockBarRepository::findById(const std::string &id) const {
    int index = findIndexById(id);
    if (index == -1) {
        return std::nullopt;
    }
    return stockBars[index];
}

std::vector<StockBar> InMemoryStockBarRepository::findAll() const {
    return stockBars;
}

std::vector<StockBar> InMemoryStockBarRepository::findAvailable() const {
    std::vector<StockBar> result;

    for (const auto &bar : stockBars) {
        if (bar.isAvailable()) {
            result.push_back(bar);
        }
    }

    return result;
}

std::vector<StockBar> InMemoryStockBarRepository::findByMaterialId(
    const std::string &materialId) const {
    std::vector<StockBar> result;

    for (const auto &bar : stockBars) {
        if (bar.getProfile().getMaterial().getId() == materialId) {
            result.push_back(bar);
        }
    }

    return result;
}

std::vector<StockBar> InMemoryStockBarRepository::findAvailableByMaterialId(
    const std::string &materialId) const {
    std::vector<StockBar> result;

    for (const auto &bar : stockBars) {
        if (bar.isAvailable() &&
            bar.getProfile().getMaterial().getId() == materialId) {
            result.push_back(bar);
        }
    }

    return result;
}

std::vector<StockBar> InMemoryStockBarRepository::findByProfileId(
    const std::string &profileId) const {
    std::vector<StockBar> result;

    for (const auto &bar : stockBars) {
        if (bar.getProfile().getId() == profileId) {
            result.push_back(bar);
        }
    }

    return result;
}

bool InMemoryStockBarRepository::exists(const std::string &id) const {
    return findIndexById(id) != -1;
}