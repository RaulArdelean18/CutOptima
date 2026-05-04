#include "InventoryService.h"
#include <stdexcept>

InventoryService::InventoryService(IStockBarRepository &stockBarRepository)
    : stockBarRepository(stockBarRepository) {}

std::vector<StockBar> InventoryService::getAllBars() const {
    return stockBarRepository.findAll();
}

std::vector<StockBar> InventoryService::getAvailableBars() const {
    return stockBarRepository.findAvailable();
}

std::vector<StockBar> InventoryService::getAvailableBarsByMaterial(
    const std::string &materialId) const {
    return stockBarRepository.findAvailableByMaterialId(materialId);
}

std::vector<StockBar> InventoryService::getCompatibleBarsForRequestItem(
    const CutRequestItem &item) const {
    std::vector<StockBar> result;
    auto availableBars = stockBarRepository.findAvailable();

    for (const auto &bar : availableBars) {
        const std::string &materialId = bar.getProfile().getMaterial().getId();
        const std::string &profileId = bar.getProfile().getId();

        if (item.isMaterialAllowed(materialId) &&
            item.isProfileAllowed(profileId) &&
            bar.canFit(item.getRequiredLength())) {
            result.push_back(bar);
        }
    }

    return result;
}

void InventoryService::addStockBar(const StockBar &stockBar) {
    stockBarRepository.save(stockBar);
}

void InventoryService::consumeBarLength(const std::string &barId,
                                        double usedLength) {
    auto foundBar = stockBarRepository.findById(barId);
    if (!foundBar.has_value()) {
        throw std::runtime_error("StockBar not found.");
    }

    StockBar updatedBar = foundBar.value();
    bool ok = updatedBar.consume(usedLength);

    if (!ok) {
        throw std::runtime_error(
            "Cannot consume requested length from stock bar.");
    }

    stockBarRepository.update(updatedBar);
}