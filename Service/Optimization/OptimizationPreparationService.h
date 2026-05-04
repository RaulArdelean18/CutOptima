#pragma once

#include "Domain/Piece/CutPiece.h"
#include "Domain/CutRequest/CutRequestItem.h"
#include "Domain/Stock/StockBar.h"
#include "Service/Request/CutPieceFactory.h"
#include "Service/Inventory/InventoryService.h"
#include "Service/Request/RequestPartitioner.h"
#include <string>
#include <vector>

struct OptimizationProblem {
    std::string materialId;
    std::vector<StockBar> stockBars;
    std::vector<CutPiece> pieces;
};

class OptimizationPreparationService {
  private:
    InventoryService &inventoryService;
    CutPieceFactory cutPieceFactory;
    RequestPartitioner requestPartitioner;

  public:
    explicit OptimizationPreparationService(InventoryService &inventoryService);

    bool canPartitionByMaterial(const std::vector<CutRequestItem> &items) const;

    std::vector<OptimizationProblem>
    prepare(const std::vector<CutRequestItem> &items) const;
    OptimizationProblem
    prepareGlobal(const std::vector<CutRequestItem> &items) const;
};
