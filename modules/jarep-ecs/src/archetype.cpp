//
// Created by Sebastian Borsch on 01.07.23.
//

#include "archetype.hpp"

Archetype::Archetype(){
    typeHash = 0;
}

Archetype::~Archetype() {
    componentTypeMap.clear();
}

std::unique_ptr<Archetype> Archetype::createEmpty() {
    return std::make_unique<Archetype>();
}

std::size_t Archetype::generate_hash(std::vector<std::type_index> &componentTypes) {
    // Create a unique hash from the component types in the archetype.
    size_t seed = componentTypes.size();
    for (const auto type: componentTypes) {
        // ChatGPT suggested these constants to generate more secure and unique hash values.
        seed ^= type.hash_code() * 0x9e3779b9 + (seed << 6) + (seed << 2);
    }
    return seed;
}

void Archetype::removeEntity(Entity &entity) {

    auto it = std::find(entities.begin(), entities.end(), entity);
    if(it == entities.end()) return;
    int entityIndex = std::distance(entities.begin(), it);

    for (const auto & componentCollection : componentCollections) {
        componentCollection->removeAt(entityIndex);
    }
    entities.erase(entities.begin() + entityIndex);
}

std::optional<size_t> Archetype::migrateEntity(std::unique_ptr<Archetype> &from, const size_t & entityIndex) {

    size_t newEntityIndex = componentCollections[0]->getCollectionLength();


    for(const auto& element : from->componentTypeMap){

        if(!componentTypeMap.contains(element.first))
        {
            from->componentCollections.at(element.second)->removeAt(entityIndex);
            continue;
        }
        if(componentCollections[element.second]->getCollectionLength() != newEntityIndex) return std::nullopt;

        from->componentCollections[element.second]->migrate(entityIndex, *componentCollections[element.second]);
    }


    return std::make_optional(newEntityIndex);
}



