#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "CuteAtom.h"
#include "CuteConfig.h"
#include "CuteInstr.h"

#include "container.h"



static inline bool
ct_container_manager_pushToArray(ctContainerManager* manager, ctContainer* container) {

	if (manager->container_count > 10) {
		printf("Too many containers allocated\n");
	}
    manager->containers[manager->container_count] = container;
    manager->container_count++;
    return true;
}


static inline bool 
ct_containers_removeFromArray(ctContainerManager* manager, ctContainer* container) {

    uint32_t index = 0;
    bool found = false;

    for (uint32_t i = 0; i < manager->container_count; i++) {
        if (manager->containers[i] == container) {
            index = i;
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }

    uint32_t elements_to_move = manager->container_count - index - 1;
    
    if (elements_to_move > 0) {
        memmove(&manager->containers[index], 
                &manager->containers[index + 1], 
                elements_to_move * sizeof(ctContainer*));
    }

    manager->container_count--;
    return true;
}


void
ct_containers_init(ctContainerManager* manager) {
	manager->container_count = 0;
	manager->internal_id = 0;
}


void
ct_containers_end(ctContainerManager* manager) {

	while (manager->container_count > 0) {
		ct_containers_shallowDelContainer(manager, manager->containers[manager->container_count-1]);
	}
		
	manager->container_count = 0;
}


ctContainer*
ct_containers_newContainer(ctContainerManager* manager, uint32_t size) {

	ctContainer* con = malloc(sizeof(ctContainer));

	uint8_t* ptr = malloc(
		sizeof(ctAtom) * size +
		sizeof(ctAtomTypeSize) * size
	);

	con->atoms = (ctAtom*) (ptr);
	con->types = (ctAtomTypeSize*) (ptr + sizeof(ctAtom) * size);


	con->id = manager->internal_id;
	manager->internal_id++;
	con->ref_count = 0;
	con->size = size;
	con->sub_containers = 0;
	memset(con->types, ctAtomType_NoneType, size);
	
	ct_container_manager_pushToArray(manager, con);

	CUTE_LOG("containers", "New container (%u) (%p) allocated.\n", con->id, con);
	return con;
}


void
ct_containers_delContainer(ctContainerManager* manager, ctContainer* con) {

	ct_containers_removeFromArray(manager, con);

	uint32_t j = 0;
	
	for (uint32_t i = 0; i < con->size && j < con->sub_containers; i++) {
		if (con->types[i] == ctAtomType_Container) {
			ct_containers_decRef(manager, con->atoms[i].as_container);
			j++;
		}
	}
	CUTE_LOG("containers", "Container (%u) (%p) deallocated.\n", con->id, con);

	free(con);
}


void
ct_containers_shallowDelContainer(ctContainerManager* manager, ctContainer* con) {

	ct_containers_removeFromArray(manager, con);

	CUTE_LOG("containers", "Container (%u) (%p) deallocated.\n", con->id, con);

	free(con);
}



ctTypedAtom
ct_containers_conGet(ctContainerManager* manager, ctContainer* con, uint32_t index, ctError* error) {

	if (index >= con->size) {
		if (error) {
			*error = ct_error_make(ctErrorCode_OutOfBounds, "Can not get atom at invalid container index.");
		}
		return (ctTypedAtom){ctAtomType_NoneType, (ctAtom){0}};
	}

	return (ctTypedAtom){con->types[index], con->atoms[index]};
}


void
ct_containers_conSet(ctContainerManager* manager, ctContainer* con, uint32_t index, ctTypedAtom atom, ctError* error) {
	if (index >= con->size) {
		if (error) {
			*error = ct_error_make(ctErrorCode_OutOfBounds, "Can not set atom at invalid container index.");
		}
		return;
	}

	if (con->types[index] == ctAtomType_Container) {
		ct_containers_decRef(manager, con->atoms[index].as_container);
		if (con->sub_containers > 0) con->sub_containers--; // remove these checks later since sub_containers will always be ++
	}

	con->atoms[index] = atom.atom;
	con->types[index] = atom.type;

	if (con->types[index] == ctAtomType_Container) {
		ct_containers_incRef(manager, con->atoms[index].as_container);
		con->sub_containers++; 
	}
}


// Resize the container
void
ct_containers_conResize(ctContainerManager* manager, ctContainer* con, uint32_t new_size, ctError* error) {

	if (con->size == new_size) {
		return;
	}
	
	ctContainer temp;
	temp.size = con->size; // for logging later

	// Cannot use realloc here because of my weird hack

	uint8_t* ptr = malloc(
		sizeof(ctAtom) * new_size +
		sizeof(ctAtomTypeSize) * new_size
	);
	temp.atoms = (ctAtom*) (ptr);
	temp.types = (ctAtomTypeSize*) (ptr + sizeof(ctAtom) * new_size);

	size_t size_to_copy = new_size < temp.size ? new_size: temp.size;

	memcpy(temp.atoms, con->atoms, size_to_copy * sizeof(ctAtom));
	memcpy(temp.types, con->types, size_to_copy * sizeof(ctAtomTypeSize));

	free(con->atoms);

	con->atoms = temp.atoms;
	con->types = temp.types;
	con->size = new_size;

	CUTE_LOG("containers", "Resized container(%u) [%p] from %u to %u\n", con->id, con, temp.size, new_size);
}


ctContainer*
ct_containers_conCopy(ctContainerManager* manager, ctContainer* src, ctError* error) {
	
	ctContainer* copy = ct_containers_newContainer(manager, src->size);

	memcpy(copy->atoms, src->atoms, src->size * sizeof(ctAtom));
	memcpy(copy->types, src->types, src->size * sizeof(ctAtomTypeSize));

	copy->sub_containers = src->sub_containers;
	uint32_t j = 0;
	
	for (uint32_t i = 0; i < src->size && j < src->sub_containers; i++) {
		if (src->types[i] == ctAtomType_Container) {
			ct_containers_incRef(manager, src->atoms[i].as_container);
			j++;
		}
	}
	
	CUTE_LOG("containers", "Copied container(%u) [%p] from container(%u) [%p]\n", copy->id, copy, src->id, src);

	return copy;
}


ctContainer*
ct_containers_conClone(ctContainerManager* manager, ctContainer* src, ctError* error) {

	ctContainer* clone = ct_containers_newContainer(manager, src->size);

	memcpy(clone->atoms, src->atoms, src->size * sizeof(ctAtom));
	memcpy(clone->types, src->types, src->size * sizeof(ctAtomTypeSize));

	clone->sub_containers = src->sub_containers;
	uint32_t j = 0;
	
	for (uint32_t i = 0; i < src->size && j < src->sub_containers; i++) {
		if (src->types[i] == ctAtomType_Container) {
			clone->atoms[i].as_container = ct_containers_conClone(manager, src->atoms[i].as_container, error);
			ct_containers_incRef(manager, clone->atoms[i].as_container);
			j++;
		}
	}
	
	CUTE_LOG("containers", "Cloned container(%u) [%p] from container(%u) [%p]\n", clone->id, clone, src->id, src);
	return clone;
}