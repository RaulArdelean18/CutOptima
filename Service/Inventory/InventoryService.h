#pragma once

#include "Domain/CutRequest/CutRequestItem.h"
#include "Domain/Stock/StockBar.h"
#include "Repository/Interfaces/IStockBarRepository.h"
#include <string>
#include <vector>

class InventoryService {
  private:
    IStockBarRepository &stockBarRepository;

  public:
    explicit InventoryService(IStockBarRepository &stockBarRepository);

    std::vector<StockBar> getAllBars() const;
    std::vector<StockBar> getAvailableBars() const;
    std::vector<StockBar>
    getAvailableBarsByMaterial(const std::string &materialId) const;
    std::vector<StockBar>
    getCompatibleBarsForRequestItem(const CutRequestItem &item) const;

    void addStockBar(const StockBar &stockBar);
    void consumeBarLength(const std::string &barId, double usedLength);
};
