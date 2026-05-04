#include "OptimizationPreparationService.h"

OptimizationPreparationService::OptimizationPreparationService(
    InventoryService &inventoryService)
    : inventoryService(inventoryService) {}

bool OptimizationPreparationService::canPartitionByMaterial(
    const std::vector<CutRequestItem> &items) const {
    return requestPartitioner.canPartitionByMaterial(items);
}

std::vector<OptimizationProblem> OptimizationPreparationService::prepare(
    const std::vector<CutRequestItem> &items) const {

    if (!requestPartitioner.canPartitionByMaterial(items)) {
        return {prepareGlobal(items)};
    }

    std::vector<OptimizationProblem> result;
    auto groups = requestPartitioner.partitionByMaterial(items);

    for (const auto &entry : groups) {
        const std::string &materialId = entry.first;
        const std::vector<CutRequestItem> &groupItems = entry.second;

        OptimizationProblem problem;
        problem.materialId = materialId;
        problem.stockBars =
            inventoryService.getAvailableBarsByMaterial(materialId);
        problem.pieces = cutPieceFactory.expandAll(groupItems);

        result.push_back(problem);
    }

    return result;
}

OptimizationProblem OptimizationPreparationService::prepareGlobal(
    const std::vector<CutRequestItem> &items) const {

    OptimizationProblem problem;
    problem.materialId = "";
    problem.stockBars = inventoryService.getAvailableBars();
    problem.pieces = cutPieceFactory.expandAll(items);

    return problem;
}