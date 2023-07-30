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

void Archetype::migrateEntity(std::unique_ptr<Archetype> &from, const Entity& entity) {

    auto it = std::find(from->entities.begin(), from->entities.end(), entity);

    if(it == from->entities.end()) return;
    size_t entityIndex = std::distance(from->entities.begin(), it);

    for (size_t i = 0; i < from->componentCollections.size(); ++i) {
        from->componentCollections[i]->migrate(entityIndex, *componentCollections[i]);
    }
    from->entities.erase(from->entities.begin() + entityIndex);
    entities.push_back(entity);
}



