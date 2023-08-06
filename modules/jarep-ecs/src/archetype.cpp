//
// Created by Sebastian Borsch on 01.07.23.
//

#include "archetype.hpp"

Archetype::Archetype(){
     componentTypeMap = std::unordered_map<std::type_index, size_t>();
     componentCollections = std::vector<std::unique_ptr<ComponentInstanceCollection>>();
}

Archetype::~Archetype() {
    componentTypeMap.clear();
}

std::unique_ptr<Archetype> Archetype::createEmpty() {
    return std::make_unique<Archetype>();
}

void Archetype::removeComponentsAtEntityIndex(size_t entityIndex) {

    for (const auto &componentCollection: componentCollections) {
        componentCollection->removeAt(entityIndex);
    }
}

std::optional<size_t> Archetype::migrateEntity(std::unique_ptr<Archetype> &from, const size_t &entityIndex) {

    size_t newEntityIndex = componentCollections[0]->getCollectionLength();


    for (const auto &element: from->componentTypeMap) {

        if (!componentTypeMap.contains(element.first)) {
            continue;
        }
        if (componentCollections[element.second]->getCollectionLength() != newEntityIndex) return std::nullopt;

        from->componentCollections[element.second]->migrate(entityIndex, *componentCollections[element.second]);
    }


    return std::make_optional(newEntityIndex);
}



