#include "RequestPartitioner.h"
#include <stdexcept>

bool RequestPartitioner::canPartitionByMaterial(
    const std::vector<CutRequestItem> &items) const {
    for (const auto &item : items) {
        if (!item.hasFixedMaterial()) {
            return false;
        }
    }
    return true;
}

std::unordered_map<std::string, std::vector<CutRequestItem>>
RequestPartitioner::partitionByMaterial(
    const std::vector<CutRequestItem> &items) const {
    if (!canPartitionByMaterial(items)) {
        throw std::runtime_error("Cannot partition request by material because "
                                 "not all items have a fixed material.");
    }

    std::unordered_map<std::string, std::vector<CutRequestItem>> groups;

    for (const auto &item : items) {
        const std::string &materialId = item.getAllowedMaterialIds()[0];
        groups[materialId].push_back(item);
    }

    return groups;
}