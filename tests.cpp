#include "Domain/BarProfile/BarProfile.h"
#include "Domain/CutRequest/CutRequestItem.h"
#include "Domain/Material/Material.h"
#include "Domain/Piece/CutPiece.h"
#include "Domain/Stock/StockBar.h"
#include "Repository/InMemory/InMemoryBarProfileRepository.h"
#include "Repository/InMemory/InMemoryMaterialRepository.h"
#include "Repository/InMemory/InMemoryStockBarRepository.h"
#include "Service/Inventory/InventoryService.h"
#include "Service/Optimization/OptimizationPreparationService.h"
#include "Service/Optimization/OptimizationService.h"
#include "Service/Request/CutPieceFactory.h"
#include "Service/Request/RequestPartitioner.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

void require(bool condition, const std::string &message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void requireDouble(double actual, double expected, const std::string &message) {
    if (std::fabs(actual - expected) > 0.000001) {
        throw std::runtime_error(message);
    }
}

template <typename Function>
void requireThrows(Function function, const std::string &message) {
    try {
        function();
    } catch (const std::exception &) {
        return;
    }

    throw std::runtime_error(message);
}

void testCutPieceFromRequestItem() {
    CutRequestItem item("req1", 1200.0, 3, {"mat1"}, {"prof1"});

    CutPiece piece = CutPiece::fromRequestItem(item, 0);

    require(piece.getId() == "req1-1", "CutPiece id is wrong.");
    require(piece.getRequestItemId() == "req1",
            "CutPiece request item id is wrong.");
    requireDouble(piece.getLength(), 1200.0, "CutPiece length is wrong.");
}

void testDomainValidationRejectsInvalidValues() {
    requireThrows([] { Material("", "Aluminum", 3.0, true); },
                  "Material should reject empty id.");
    requireThrows([] { Material("mat1", "", 3.0, true); },
                  "Material should reject empty name.");
    requireThrows([] { Material("mat1", "Aluminum", -1.0, true); },
                  "Material should reject negative cut loss.");

    Material material("mat1", "Aluminum", 3.0, true);
    requireThrows(
        [&] { BarProfile("prof1", "Profile", "P1", material, 0.0, 10.0); },
        "BarProfile should reject zero standard length.");
    requireThrows(
        [&] { BarProfile("prof1", "Profile", "P1", material, 1000.0, -1.0); },
        "BarProfile should reject negative purchase price.");

    BarProfile profile("prof1", "Profile", "P1", material, 1000.0, 10.0);
    requireThrows(
        [&] { StockBar("stock1", "B1", profile, 1000.0, 1200.0); },
        "StockBar should reject remaining length above original length.");
    requireThrows([] { CutRequestItem("req1", 0.0, 1, {}, {}); },
                  "CutRequestItem should reject zero required length.");
    requireThrows([] { CutRequestItem("req1", 100.0, 0, {}, {}); },
                  "CutRequestItem should reject zero quantity.");
    requireThrows([] { CutPiece("piece1", "req1", 0.0); },
                  "CutPiece should reject zero length.");
}

void testMaterialAndBarProfileSetters() {
    Material material("mat1", "Aluminum", 3.0, true);

    material.setName("Steel");
    material.setDefaultCutLoss(4.5);
    material.setReusable(false);

    require(material.getName() == "Steel", "Material name setter failed.");
    requireDouble(material.getDefaultCutLoss(), 4.5,
                  "Material cut loss setter failed.");
    require(!material.isReusable(), "Material reusable setter failed.");
    requireThrows([&] { material.setName(""); },
                  "Material should reject empty name in setter.");
    requireThrows([&] { material.setDefaultCutLoss(-1.0); },
                  "Material should reject negative cut loss in setter.");

    BarProfile profile("prof1", "Profile", "P1", material, 1000.0, 10.0);

    profile.setName("Updated Profile");
    profile.setBarcode("P2");
    profile.setStandardLength(2000.0);
    profile.setPurchasePrice(25.0);

    require(profile.getName() == "Updated Profile",
            "BarProfile name setter failed.");
    require(profile.getBarcode() == "P2", "BarProfile barcode setter failed.");
    requireDouble(profile.getStandardLength(), 2000.0,
                  "BarProfile standard length setter failed.");
    requireDouble(profile.getPurchasePrice(), 25.0,
                  "BarProfile purchase price setter failed.");
    requireThrows([&] { profile.setName(""); },
                  "BarProfile should reject empty name in setter.");
    requireThrows([&] { profile.setBarcode(""); },
                  "BarProfile should reject empty barcode in setter.");
    requireThrows([&] { profile.setStandardLength(0.0); },
                  "BarProfile should reject zero standard length in setter.");
    requireThrows(
        [&] { profile.setPurchasePrice(-1.0); },
        "BarProfile should reject negative purchase price in setter.");
}

void testCutRequestAllowedMaterialAndProfileRules() {
    CutRequestItem unrestricted("req1", 1000.0, 1, {}, {});
    CutRequestItem restricted("req2", 1000.0, 1, {"mat1", "mat2"},
                              {"prof1", "prof2"});
    CutRequestItem fixedMaterial("req3", 1000.0, 1, {"mat1"}, {});

    require(unrestricted.isMaterialAllowed("anything"),
            "Empty material list should allow any material.");
    require(unrestricted.isProfileAllowed("anything"),
            "Empty profile list should allow any profile.");
    require(restricted.isMaterialAllowed("mat1"),
            "Allowed material should be accepted.");
    require(!restricted.isMaterialAllowed("mat3"),
            "Unknown material should be rejected.");
    require(restricted.isProfileAllowed("prof2"),
            "Allowed profile should be accepted.");
    require(!restricted.isProfileAllowed("prof3"),
            "Unknown profile should be rejected.");
    require(fixedMaterial.hasFixedMaterial(),
            "Single allowed material should mean fixed material.");
    require(!restricted.hasFixedMaterial(),
            "Multiple allowed materials should not mean fixed material.");
}

void testCutPieceFromRequestItemRejectsInvalidIndex() {
    CutRequestItem item("req1", 1200.0, 3, {"mat1"}, {"prof1"});

    requireThrows([&] { CutPiece::fromRequestItem(item, -1); },
                  "CutPiece::fromRequestItem should reject negative index.");
    requireThrows(
        [&] { CutPiece::fromRequestItem(item, 3); },
        "CutPiece::fromRequestItem should reject index equal to quantity.");
}

void testCutPieceFactoryExpandsQuantity() {
    CutRequestItem item("req1", 1200.0, 3, {"mat1"}, {"prof1"});
    CutPieceFactory factory;

    std::vector<CutPiece> pieces = factory.expand(item);

    require(pieces.size() == 3, "CutPieceFactory should create 3 pieces.");
    require(pieces[0].getId() == "req1-1", "First piece id is wrong.");
    require(pieces[2].getId() == "req1-3", "Last piece id is wrong.");
}

void testStockBarConsume() {
    Material material("mat1", "Aluminum", 3.0, true);
    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", material, 6000.0,
                       120.0);
    StockBar bar("stock1", "BARCODE-0001", profile, 6000.0, 6000.0);

    require(bar.consume(1200.0), "StockBar should consume valid length.");
    requireDouble(bar.getRemainingLength(), 4800.0,
                  "StockBar remaining length is wrong.");
    requireDouble(bar.getUsedLength(), 1200.0,
                  "StockBar used length is wrong.");
    require(!bar.consume(7000.0), "StockBar should reject oversized consume.");
    require(!bar.consume(0.0), "StockBar should reject zero consume.");
    require(!bar.consume(-1.0), "StockBar should reject negative consume.");
}

void testStockBarConsumeExactLengthMarksUsed() {
    Material material("mat1", "Aluminum", 3.0, true);
    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", material, 6000.0,
                       120.0);
    StockBar bar("stock1", "BARCODE-0001", profile, 6000.0, 1200.0);

    require(bar.consume(1200.0), "StockBar should consume exact length.");
    requireDouble(bar.getRemainingLength(), 0.0,
                  "StockBar should have no remaining length.");
    require(bar.getStatus() == StockBarStatus::USED,
            "StockBar should be marked as used.");
    require(!bar.isAvailable(), "Used StockBar should not be available.");
}

void testStockBarRejectsUnavailableConsume() {
    Material material("mat1", "Aluminum", 3.0, true);
    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", material, 6000.0,
                       120.0);
    StockBar bar("stock1", "BARCODE-0001", profile, 6000.0, 6000.0,
                 StockBarStatus::RESERVED);

    require(!bar.canFit(1200.0), "Reserved StockBar should not fit pieces.");
    require(!bar.consume(1200.0), "Reserved StockBar should not be consumed.");
    requireDouble(bar.getRemainingLength(), 6000.0,
                  "Rejected consume should not change remaining length.");
}

void testMaterialRepository() {
    InMemoryMaterialRepository repository;
    Material material("mat1", "Aluminum", 3.0, true);
    Material updated("mat1", "Updated Aluminum", 4.0, false);

    repository.save(material);

    require(repository.exists("mat1"),
            "MaterialRepository should find saved id.");
    require(repository.findAll().size() == 1,
            "MaterialRepository should contain one material.");
    require(repository.findById("mat1").has_value(),
            "MaterialRepository should find saved material.");
    require(repository.findById("mat1")->getName() == "Aluminum",
            "MaterialRepository returned wrong material.");

    repository.update(updated);

    require(repository.findById("mat1")->getName() == "Updated Aluminum",
            "MaterialRepository update failed.");
    requireThrows([&] { repository.save(updated); },
                  "MaterialRepository should reject duplicate ids.");
    requireThrows(
        [&] { repository.update(Material("missing", "Missing", 1.0, true)); },
        "MaterialRepository should reject update for missing id.");
    require(!repository.findById("missing").has_value(),
            "MaterialRepository should return nullopt for missing id.");
}

void testBarProfileRepository() {
    Material aluminum("mat1", "Aluminum", 3.0, true);
    Material steel("mat2", "Steel", 4.0, true);
    BarProfile aluminumProfile("prof1", "Aluminum 40x20", "ALU-40-20", aluminum,
                               6000.0, 120.0);
    BarProfile steelProfile("prof2", "Steel 40x20", "STEEL-40-20", steel,
                            6000.0, 160.0);
    BarProfile updated("prof1", "Updated Aluminum", "ALU-UPDATED", aluminum,
                       5000.0, 100.0);

    InMemoryBarProfileRepository repository;
    repository.save(aluminumProfile);
    repository.save(steelProfile);

    require(repository.exists("prof1"),
            "BarProfileRepository should find saved id.");
    require(repository.findAll().size() == 2,
            "BarProfileRepository should contain two profiles.");
    require(repository.findById("prof1").has_value(),
            "BarProfileRepository should find saved profile.");
    require(repository.findByMaterialId("mat1").size() == 1,
            "BarProfileRepository should filter by material.");
    require(repository.findByMaterialId("mat1")[0].getId() == "prof1",
            "BarProfileRepository returned wrong material profile.");

    repository.update(updated);

    require(repository.findById("prof1")->getName() == "Updated Aluminum",
            "BarProfileRepository update failed.");
    requireThrows([&] { repository.save(updated); },
                  "BarProfileRepository should reject duplicate ids.");
    requireThrows(
        [&] {
            repository.update(BarProfile("missing", "Missing", "MISSING",
                                         aluminum, 1000.0, 10.0));
        },
        "BarProfileRepository should reject update for missing id.");
    require(!repository.findById("missing").has_value(),
            "BarProfileRepository should return nullopt for missing id.");
}

void testInventoryServiceFindsCompatibleBars() {
    Material aluminum("mat1", "Aluminum", 3.0, true);
    Material steel("mat2", "Steel", 4.0, true);
    BarProfile aluminumProfile("prof1", "Aluminum 40x20", "ALU-40-20", aluminum,
                               6000.0, 120.0);
    BarProfile steelProfile("prof2", "Steel 40x20", "STEEL-40-20", steel,
                            6000.0, 160.0);

    InMemoryStockBarRepository repository;
    repository.save(
        StockBar("stock1", "BARCODE-0001", aluminumProfile, 6000.0, 6000.0));
    repository.save(
        StockBar("stock2", "BARCODE-0002", steelProfile, 6000.0, 6000.0));

    InventoryService inventory(repository);
    CutRequestItem item("req1", 1200.0, 1, {"mat1"}, {"prof1"});

    std::vector<StockBar> bars =
        inventory.getCompatibleBarsForRequestItem(item);

    require(bars.size() == 1,
            "InventoryService should find one compatible bar.");
    require(bars[0].getId() == "stock1",
            "InventoryService returned the wrong bar.");
}

void testInventoryServiceReturnsEmptyWhenNoCompatibleBarsExist() {
    Material material("mat1", "Aluminum", 3.0, true);
    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", material, 6000.0,
                       120.0);

    InMemoryStockBarRepository repository;
    repository.save(
        StockBar("stock1", "BARCODE-0001", profile, 6000.0, 1000.0));

    InventoryService inventory(repository);
    CutRequestItem item("req1", 1200.0, 1, {"mat1"}, {"prof1"});

    require(inventory.getCompatibleBarsForRequestItem(item).empty(),
            "InventoryService should return empty when no bar can fit.");
}

void testRepositoryRejectsDuplicatesAndMissingUpdates() {
    Material material("mat1", "Aluminum", 3.0, true);
    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", material, 6000.0,
                       120.0);
    StockBar bar("stock1", "BARCODE-0001", profile, 6000.0, 6000.0);

    InMemoryStockBarRepository repository;
    repository.save(bar);

    requireThrows([&] { repository.save(bar); },
                  "Repository should reject duplicate ids.");
    requireThrows(
        [&] {
            repository.update(
                StockBar("missing", "BARCODE-0002", profile, 6000.0, 6000.0));
        },
        "Repository should reject update for missing id.");
    require(!repository.findById("missing").has_value(),
            "Repository should return nullopt for missing id.");
}

void testStockBarRepositoryFilters() {
    Material aluminum("mat1", "Aluminum", 3.0, true);
    Material steel("mat2", "Steel", 4.0, true);
    BarProfile aluminumProfile("prof1", "Aluminum 40x20", "ALU-40-20", aluminum,
                               6000.0, 120.0);
    BarProfile steelProfile("prof2", "Steel 40x20", "STEEL-40-20", steel,
                            6000.0, 160.0);

    InMemoryStockBarRepository repository;
    repository.save(
        StockBar("stock1", "BARCODE-0001", aluminumProfile, 6000.0, 6000.0));
    repository.save(StockBar("stock2", "BARCODE-0002", aluminumProfile, 6000.0,
                             6000.0, StockBarStatus::RESERVED));
    repository.save(
        StockBar("stock3", "BARCODE-0003", steelProfile, 6000.0, 6000.0));

    require(repository.findAll().size() == 3,
            "StockBarRepository should contain three bars.");
    require(repository.findAvailable().size() == 2,
            "StockBarRepository should return only available bars.");
    require(repository.findByMaterialId("mat1").size() == 2,
            "StockBarRepository should filter by material.");
    require(repository.findAvailableByMaterialId("mat1").size() == 1,
            "StockBarRepository should filter available bars by material.");
    require(repository.findByProfileId("prof1").size() == 2,
            "StockBarRepository should filter by profile.");

    StockBar updated("stock1", "BARCODE-0001", aluminumProfile, 6000.0, 3000.0);
    repository.update(updated);
    requireDouble(repository.findById("stock1")->getRemainingLength(), 3000.0,
                  "StockBarRepository update failed.");
}

void testInventoryServiceConsumeBarLength() {
    Material material("mat1", "Aluminum", 3.0, true);
    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", material, 6000.0,
                       120.0);

    InMemoryStockBarRepository repository;
    repository.save(
        StockBar("stock1", "BARCODE-0001", profile, 6000.0, 6000.0));

    InventoryService inventory(repository);

    inventory.consumeBarLength("stock1", 1200.0);
    requireDouble(repository.findById("stock1")->getRemainingLength(), 4800.0,
                  "InventoryService consume should update repository.");
    requireThrows([&] { inventory.consumeBarLength("missing", 100.0); },
                  "InventoryService should reject missing bar id.");
    requireThrows([&] { inventory.consumeBarLength("stock1", 7000.0); },
                  "InventoryService should reject oversized consume.");
}

void testRequestPartitionerGroupsByMaterial() {
    std::vector<CutRequestItem> items = {
        CutRequestItem("req1", 1200.0, 2, {"mat1"}, {}),
        CutRequestItem("req2", 900.0, 1, {"mat2"}, {}),
        CutRequestItem("req3", 600.0, 1, {"mat1"}, {}),
    };
    RequestPartitioner partitioner;

    auto groups = partitioner.partitionByMaterial(items);

    require(groups.size() == 2, "RequestPartitioner should create 2 groups.");
    require(groups["mat1"].size() == 2, "mat1 group should contain 2 items.");
    require(groups["mat2"].size() == 1, "mat2 group should contain 1 item.");
}

void testRequestPartitionerRejectsFlexibleMaterialRequests() {
    std::vector<CutRequestItem> items = {
        CutRequestItem("req1", 1200.0, 2, {}, {}),
    };
    RequestPartitioner partitioner;

    require(
        !partitioner.canPartitionByMaterial(items),
        "RequestPartitioner should reject requests without fixed material.");
    requireThrows(
        [&] { partitioner.partitionByMaterial(items); },
        "RequestPartitioner should throw for flexible material requests.");
}

void testOptimizationPreparationService() {
    Material material("mat1", "Aluminum", 3.0, true);
    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", material, 6000.0,
                       120.0);

    InMemoryStockBarRepository repository;
    repository.save(
        StockBar("stock1", "BARCODE-0001", profile, 6000.0, 6000.0));

    InventoryService inventory(repository);
    OptimizationPreparationService service(inventory);

    std::vector<CutRequestItem> items = {
        CutRequestItem("req1", 1200.0, 3, {"mat1"}, {"prof1"}),
    };

    std::vector<OptimizationProblem> problems = service.prepare(items);

    require(problems.size() == 1,
            "OptimizationPreparationService should create one problem.");
    require(problems[0].materialId == "mat1",
            "Optimization problem material id is wrong.");
    require(problems[0].stockBars.size() == 1,
            "Optimization problem should contain one stock bar.");
    require(problems[0].pieces.size() == 3,
            "Optimization problem should contain 3 pieces.");
}

void testOptimizationPreparationFallsBackToGlobalProblem() {
    Material material("mat1", "Aluminum", 3.0, true);
    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", material, 6000.0,
                       120.0);

    InMemoryStockBarRepository repository;
    repository.save(
        StockBar("stock1", "BARCODE-0001", profile, 6000.0, 6000.0));

    InventoryService inventory(repository);
    OptimizationPreparationService service(inventory);

    std::vector<CutRequestItem> items = {
        CutRequestItem("req1", 1200.0, 2, {}, {}),
    };

    std::vector<OptimizationProblem> problems = service.prepare(items);

    require(problems.size() == 1,
            "Global optimization preparation should create one problem.");
    require(problems[0].materialId.empty(),
            "Global optimization problem should not have a material id.");
    require(problems[0].stockBars.size() == 1,
            "Global optimization problem should include available bars.");
    require(problems[0].pieces.size() == 2,
            "Global optimization problem should include expanded pieces.");
}

void testOptimizationServiceFitsPiecesIntoOneBar() {
    Material material("mat1", "Aluminum", 3.0, true);
    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", material, 6000.0,
                       120.0);

    OptimizationProblem problem;
    problem.materialId = "mat1";
    problem.stockBars = {
        StockBar("stock1", "BARCODE-0001", profile, 6000.0, 6000.0),
    };
    problem.pieces = {
        CutPiece("piece1", "req1", 1200.0),
        CutPiece("piece2", "req1", 1500.0),
        CutPiece("piece3", "req1", 1000.0),
    };

    OptimizationService service;
    CutPlan plan = service.optimize(problem);

    require(plan.bars.size() == 1,
            "OptimizationService should use one bar for fitting pieces.");
    require(plan.bars[0].pieces.size() == 3,
            "OptimizationService should assign all pieces to the bar.");
    require(plan.unassignedPieces.empty(),
            "OptimizationService should not leave fitting pieces unassigned.");
    requireDouble(plan.bars[0].remainingLength, 2300.0,
                  "OptimizationService remaining length is wrong.");
    requireDouble(plan.totalWaste, 2300.0,
                  "OptimizationService total waste is wrong.");
}

void testOptimizationServiceUsesMultipleBars() {
    Material material("mat1", "Aluminum", 3.0, true);
    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", material, 5000.0,
                       120.0);

    OptimizationProblem problem;
    problem.materialId = "mat1";
    problem.stockBars = {
        StockBar("stock1", "BARCODE-0001", profile, 5000.0, 5000.0),
        StockBar("stock2", "BARCODE-0002", profile, 5000.0, 5000.0),
    };
    problem.pieces = {
        CutPiece("piece1", "req1", 3000.0),
        CutPiece("piece2", "req1", 3000.0),
        CutPiece("piece3", "req1", 2000.0),
    };

    OptimizationService service;
    CutPlan plan = service.optimize(problem);

    require(plan.bars.size() == 2,
            "OptimizationService should use two bars when needed.");
    require(plan.unassignedPieces.empty(),
            "OptimizationService should assign all fitting pieces.");
    requireDouble(plan.totalWaste, 2000.0,
                  "OptimizationService total waste for two bars is wrong.");
}

void testOptimizationServiceLeavesOversizedPiecesUnassigned() {
    Material material("mat1", "Aluminum", 3.0, true);
    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", material, 5000.0,
                       120.0);

    OptimizationProblem problem;
    problem.materialId = "mat1";
    problem.stockBars = {
        StockBar("stock1", "BARCODE-0001", profile, 5000.0, 5000.0),
    };
    problem.pieces = {
        CutPiece("piece1", "req1", 6000.0),
        CutPiece("piece2", "req1", 2000.0),
    };

    OptimizationService service;
    CutPlan plan = service.optimize(problem);

    require(plan.bars.size() == 1,
            "OptimizationService should still use a bar for fitting pieces.");
    require(plan.bars[0].pieces.size() == 1,
            "OptimizationService should assign only fitting pieces.");
    require(plan.unassignedPieces.size() == 1,
            "OptimizationService should leave oversized piece unassigned.");
    require(plan.unassignedPieces[0].getId() == "piece1",
            "OptimizationService unassigned piece is wrong.");
    requireDouble(
        plan.totalWaste, 3000.0,
        "OptimizationService total waste with unassigned piece is wrong.");
}

int main() {
    testDomainValidationRejectsInvalidValues();
    testMaterialAndBarProfileSetters();
    testCutRequestAllowedMaterialAndProfileRules();
    testCutPieceFromRequestItem();
    testCutPieceFromRequestItemRejectsInvalidIndex();
    testCutPieceFactoryExpandsQuantity();
    testStockBarConsume();
    testStockBarConsumeExactLengthMarksUsed();
    testStockBarRejectsUnavailableConsume();
    testMaterialRepository();
    testBarProfileRepository();
    testInventoryServiceFindsCompatibleBars();
    testInventoryServiceReturnsEmptyWhenNoCompatibleBarsExist();
    testRepositoryRejectsDuplicatesAndMissingUpdates();
    testStockBarRepositoryFilters();
    testInventoryServiceConsumeBarLength();
    testRequestPartitionerGroupsByMaterial();
    testRequestPartitionerRejectsFlexibleMaterialRequests();
    testOptimizationPreparationService();
    testOptimizationPreparationFallsBackToGlobalProblem();
    testOptimizationServiceFitsPiecesIntoOneBar();
    testOptimizationServiceUsesMultipleBars();
    testOptimizationServiceLeavesOversizedPiecesUnassigned();

    std::cout << "All tests passed.\n";
    return 0;
}
