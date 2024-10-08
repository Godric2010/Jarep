//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_COMPONENTINSTANCECOLLECTION_HPP
#define JAREP_COMPONENTINSTANCECOLLECTION_HPP

#include <vector>
#include <functional>
#include <memory>
#include <any>


/// This pattern is called "Curiously recurring template pattern" (CRTP). It allows the compiler to
/// work with generic classes which shall be accessible by a non-generic interface like class.
/// In Rust a trait would be used for this, C++ mirrors this in most of the way, but checks types at runtime and
/// not at compile time like rust.

class ComponentInstanceCollection {
    public:
        virtual ~ComponentInstanceCollection() = default;

        /// Create a new collection based of the generic value given to a similar collection.
        /// \return Unique pointer to the new collection.
        virtual std::unique_ptr<ComponentInstanceCollection> createNewAndEmpty() = 0;

        /// Get the instance of this collection immutable.
        /// \return Reference to this generic collection.
        virtual const std::any as_any_const() const = 0;

        /// Get the instance of this collection mutable.
        /// \return Reference to this generic collection.
        virtual std::any as_any() = 0;

        /// Remove and item at the given entity index.
        /// \param index -> The index of the entity to which this item belongs.
        virtual void removeAt(size_t index) = 0;

        /// Migrate entries from this collection to another collection.
        /// \param index -> The entity index of the element that should be migrated.
        /// \param target -> The target collection to which this element shall migrate.
        virtual void migrate(size_t index, ComponentInstanceCollection &other) = 0;

        /// Fetch the amount of entries in this collection.
        virtual size_t getCollectionLength() = 0;

        /// Gets the hash value of this collection instance.
        /// \return The hash valur of this collection.
        virtual size_t getHashValue() = 0;
};

template<class T>
class InstanceCollection : public ComponentInstanceCollection {

        // This is needed so the hash struct can read the componentList field without exposing it publicly
        friend struct std::hash<InstanceCollection<T>>;

    public:
        /// Create a new collection based of the generic value given to a similar collection.
        /// \return Unique pointer to the new collection.
        std::unique_ptr<ComponentInstanceCollection> createNewAndEmpty() override {
            return std::make_unique<InstanceCollection<T>>();
        }

        /// Fetch the amount of entries inside this collection.
        size_t getCollectionLength() override{
            return componentList.size();
        }

        /// Get the instance of this collection immutable.
        const std::any as_any_const() const override {
            return std::any(std::reference_wrapper(componentList));
        }

        /// Get the instance of this collection mutable.
        std::any as_any() override {
            return std::any(std::reference_wrapper(componentList));
        }

        /// Remove and item at the given entity index.
        /// \param index -> The index of the entity to which this item belongs.
        void removeAt(size_t index) override {
            if (index >= componentList.size()) return;
            componentList.erase(componentList.begin() + index);
        }

        /// Migrate entries from this collection to another collection.
        /// \param index -> The entity index of the element that should be migrated.
        /// \param target -> The target collection to which this element shall migrate.
        void migrate(size_t index, ComponentInstanceCollection &target) override {
           std::shared_ptr<T> value = std::move(componentList[index]);
            static_cast<InstanceCollection<T> &>(target).componentList.push_back(std::move(value));
        }

        /// Gets the hash value of this collection instance.
        /// \return The hash valur of this collection.
        size_t getHashValue() override {
            std::hash<InstanceCollection<T>> hasher;
            return hasher(*this);
        }

    private:
        std::vector<std::shared_ptr<T>> componentList;

};


template<typename T>
struct std::hash<InstanceCollection<T>> {
    std::size_t operator()(const InstanceCollection<T> &obj) const {
        std::hash<std::type_index> elementHasher;
        std::size_t hashValue = 0;
        for (const std::shared_ptr<T> &component: obj.componentList) {
            hashValue ^= elementHasher(typeid(*component));
        }
        return hashValue;
    }
};

#endif //JAREP_COMPONENTINSTANCECOLLECTION_HPP
