//
// Created by Sebastian Borsch on 01.07.23.
//

#include "archetype.hpp"

Archetype::Archetype(){
    typeHash = 0;
    componentCollectionsLength = 0;
}

Archetype::~Archetype() {
    componentTypeMap.clear();
}

std::unique_ptr<Archetype> Archetype::createEmpty() {
    return std::make_unique<Archetype>();
}

//template std::optional<std::unique_ptr<Archetype>> Archetype::createFromAdd<Component>(const std::unique_ptr<Archetype>&);
template<typename T>
std::optional<std::unique_ptr<Archetype>> Archetype::createFromAdd(const std::unique_ptr<Archetype> &fromArchetype) {

    auto instance = std::make_unique<Archetype>();

     //Take the existing archetype and create a new component instance collection with only empty vectors.
    size_t fromColumnLength = fromArchetype->componentCollections.size();
    for (int i = 0; i < fromColumnLength; ++i) {
        auto newAndEmpty = fromArchetype->componentCollections[i]->createNewAndEmpty();
        instance->componentCollections.push_back(std::move(newAndEmpty));
    }
    instance->componentCollections.push_back(std::make_unique<InstanceCollection<T>>());
    instance->componentCollectionsLength += 1;

    // Iterate over the existing type-map and copy it, as well as collecting all types from the old archetype.
    instance->componentTypeMap = std::unordered_map<std::type_index, size_t>();
    auto typesInArchetype = new std::vector<std::type_index>();
    for (const auto typeEntry: fromArchetype->componentTypeMap) {
        instance->componentTypeMap.insert_or_assign(typeEntry.first, typeEntry.second);
        typesInArchetype->push_back(typeEntry.first);
    }

    // Assign the new generic component to all maps and lists so the new archetype will be different from the old one.
    instance->componentTypeMap.insert_or_assign(typeid(T), instance->componentCollectionsLength);
    typesInArchetype->push_back(typeid(T));

    instance->generate_hash(typesInArchetype);

    return std::make_optional<std::unique_ptr<Archetype>>(std::move(instance));
}

template<class T>
std::optional<std::unique_ptr<Archetype>> Archetype::createFromRemove(const std::unique_ptr<Archetype> &fromArchetype) {

//    // Check if the old archetype contains the requested type to remove and return nullopt if not.
//    if (!fromArchetype.containsType<T>()) {
//        return std::nullopt;
//    }
//
    auto instance = new Archetype();
//
//    // Start with coping the existing types and filter out the index of the type to remove in the process.
//    instance->componentTypeMap = std::unordered_map<std::type_index, size_t>();
//    auto typesInArchetype = new std::vector<std::type_index>();
//    size_t targetIndexToRemove = 0;
//    for (const auto typeEntry: fromArchetype.componentTypeMap) {
//
//        // If the typeid is the one to remove we do not copy it memorize the index of that type in the archetype lists.
//        if (typeEntry.first == typeid(T)) {
//            targetIndexToRemove = typeEntry.second;
//            continue;
//        }
//
//        instance->componentTypeMap.insert_or_assign(typeEntry.first, typeEntry.second);
//        typesInArchetype->push_back(typeEntry.first);
//    }
//    instance->generate_hash(typesInArchetype);
//
//    // Copy the component lists and instantiate them empty except for the collection at the memorized index.
//    size_t newComponentCollectionsLength = fromArchetype.componentCollectionsLength--;
//    for (int i = 0; i < newComponentCollectionsLength; ++i) {
//        if (i == targetIndexToRemove) continue;
//        instance->componentCollections[i] = fromArchetype.componentCollections[i]->createNewAndEmpty();
//    }
//
    return std::make_optional<std::unique_ptr<Archetype>>(instance);
}

void Archetype::generate_hash(std::vector<std::type_index> *componentTypes) {
    // Create a unique hash from the component types in the archetype.
    size_t seed = componentTypes->size();
    for (const auto type: *componentTypes) {
        // ChatGPT suggested these constants to generate more secure and unique hash values.
        seed ^= type.hash_code() * 0x9e3779b9 + (seed << 6) + (seed << 2);
    }
    typeHash = seed;
}

void Archetype::removeEntity(Entity &entity) {

//    auto it = std::find(entities.begin(), entities.end(), entity);
//    if(it == entities.end()) return;
//    int entityIndex = std::distance(entities.begin(), it);
//
//    for (int i = 0; i < componentCollectionsLength; ++i) {
//        componentCollections[i]->removeAt(entityIndex);
//    }
//    entities.erase(entities.begin() + entityIndex);
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

template<class T>
std::optional<std::vector<std::tuple<T *, Entity>>> Archetype::getComponentsWithEntities() {
    return std::optional<std::vector<std::tuple<T *, Entity>>>();
}

template<class T>
std::optional<T *> Archetype::getComponent(size_t index) {
    return std::optional<T *>();
}

template<class T>
void Archetype::setComponentInstance(T componentInstance) {

}

template<class T>
bool Archetype::containsType() {
    auto result = componentTypeMap.find(typeid(T));
    if (componentTypeMap.end() != result) {
        return false;
    }
    return true;
}

