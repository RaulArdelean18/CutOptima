#include "Domain/BarProfile/BarProfile.h"
#include "Domain/CutRequest/CutRequestItem.h"
#include "Domain/Material/Material.h"
#include "Domain/Stock/StockBar.h"
#include "Repository/InMemory/InMemoryStockBarRepository.h"
#include "Service/Inventory/InventoryService.h"
#include "Service/Optimization/CutPlanApplier.h"
#include "Service/Optimization/OptimizationPreparationService.h"
#include "Service/Optimization/OptimizationService.h"
#include <iostream>
#include <vector>

int main() {
    Material aluminum("mat1", "Aluminum", 3.0, true);

    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", aluminum, 6000.0,
                       120.0);

    InMemoryStockBarRepository stockRepository;
    stockRepository.save(
        StockBar("stock1", "BARCODE-0001", profile, 6000.0, 6000.0));
    stockRepository.save(
        StockBar("stock2", "BARCODE-0002", profile, 6000.0, 6000.0));

    std::vector<CutRequestItem> requestItems = {
        CutRequestItem("req1", 1500.0, 2, {"mat1"}, {"prof1"}),
        CutRequestItem("req2", 1200.0, 3, {"mat1"}, {"prof1"}),
        CutRequestItem("req3", 900.0, 1, {"mat1"}, {"prof1"}),
    };

    InventoryService inventory(stockRepository);
    OptimizationPreparationService preparation(inventory);
    OptimizationService optimizer;
    CutPlanApplier planApplier(inventory);

    std::vector<OptimizationProblem> problems =
        preparation.prepare(requestItems);

    for (const auto &problem : problems) {
        CutPlan plan = optimizer.optimize(problem);
        planApplier.apply(plan);

        std::cout << "Cut plan for material: " << problem.materialId << '\n';

        for (const auto &barPlan : plan.bars) {
            std::cout << "Bar " << barPlan.stockBar.getId() << ":\n";
            double cutLoss =
                barPlan.stockBar.getProfile().getMaterial().getDefaultCutLoss();

            for (const auto &piece : barPlan.pieces) {
                std::cout << "  piece " << piece.getId() << ": "
                          << piece.getLength() << " + cut loss " << cutLoss
                          << '\n';
            }

            std::cout << "  original: " << barPlan.stockBar.getOriginalLength()
                      << '\n';
            std::cout << "  used including cut loss: " << barPlan.usedLength
                      << '\n';
            std::cout << "  remaining: " << barPlan.remainingLength << '\n';
        }

        std::cout << "Total waste: " << plan.totalWaste << '\n';
        std::cout << "Unassigned pieces: " << plan.unassignedPieces.size();

        if (!plan.unassignedPieces.empty()) {
            std::cout << '\n';
            for (const auto &piece : plan.unassignedPieces) {
                std::cout << "  piece " << piece.getId() << ": "
                          << piece.getLength() << '\n';
            }
        } else {
            std::cout << '\n';
        }
    }

    std::cout << "\nInventory after applying plan:\n";
    for (const auto &bar : inventory.getAllBars()) {
        std::cout << "Bar " << bar.getId()
                  << " remaining: " << bar.getRemainingLength() << '\n';
    }

    return 0;
}
