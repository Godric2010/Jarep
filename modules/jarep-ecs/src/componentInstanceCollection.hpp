//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_COMPONENTINSTANCECOLLECTION_HPP
#define JAREP_COMPONENTINSTANCECOLLECTION_HPP

#include <vector>
#include <functional>

class ComponentInstanceCollection {
    public:
        virtual ~ComponentInstanceCollection() = default;

        // This is called "Curiously recurring template pattern" (CRTP) which basically allows to return the deriving type from the base type.
         [[nodiscard]] virtual ComponentInstanceCollection* createNewAndEmpty() const = 0;
};

template<class T>
class InstanceCollection : public ComponentInstanceCollection {

    public:
        InstanceCollection();

        ~InstanceCollection() override;

        const InstanceCollection* createNewAndEmpty() const override;

        void add(T component);

        std::vector<T> &getInstances();

        void removeAt(size_t index);

        size_t getHash() { return hashValue; }


    private:
        std::vector<T> componentList;
        size_t hashValue;

};

template<class T>
InstanceCollection<T>::InstanceCollection() {
    componentList.clear();
    hashValue = new std::hash<InstanceCollection<T>>;
}

template<class T>
const InstanceCollection<T>* InstanceCollection<T>::createNewAndEmpty() const {
    return new InstanceCollection();
}

template<class T>
InstanceCollection<T>::~InstanceCollection() {
    componentList.clear();
}

template<class T>
void InstanceCollection<T>::add(T component) {
    componentList.push_back(component);
}

template<class T>
std::vector<T> &InstanceCollection<T>::getInstances() {
    return &componentList;
}

template<class T>
void InstanceCollection<T>::removeAt(size_t index) {
    componentList.erase(componentList.begin() + index);
}


template<typename T>
struct std::hash<InstanceCollection<T>> {
    std::size_t operator()(const InstanceCollection<T> &obj) const {
        std::hash<T> elementHasher;
        std::size_t hashValue = 0;
        for (const T &component: obj.componentList) {
            hashValue ^= elementHasher(component);
        }
        return hashValue;
    }
};

#endif //JAREP_COMPONENTINSTANCECOLLECTION_HPP
