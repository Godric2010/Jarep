# TODOs for the ECS

## World
-[ ] define the add entity function
-[ ] define the remove entity function
-[ ] define the addComponent fucntion
-[ ] define the removeComponent function
-[ ] ensure that only deriving classes of component can be used for component actions 
-[ ] define the registerSystem function
-[ ] define the unregisterSystem function
-[ ] ensure that only deriving classes of system can be used for system actions 

## EntityManager
-[x] create a new class entity manager
-[x] the entity manager creates, deletes and keeps track of all entities 
-[x] it provides an array of unused entities for recycling
-[x] move the entity storing from the archetype into the entity manager so each entity knows its signature 
-[x] ensure functionality through unit tests 

## ComponentManager
-[x] create a new class component manager
-[x] each component should be registered on creation in the component manager
-[x] each component has to get an identifier value based on the bitwise iteration 
-[x] the component manager keeps track of all the archetypes and their respected signature 
-[x] define the getComponent function
-[x] define the getComponents function
-[x] ensure functionality through unit tests
 
## Signature 
-[x] create a typedef for a signature

## SystemManager
-[ ] create a new class system manager
-[ ] each system should be registered on creation in the system manager
-[ ] each system can be assigned with a signature to use 
-[ ] the system manager updates the entities available for each system by checking the entities signature
-[ ] ensure functionality through unit tests

