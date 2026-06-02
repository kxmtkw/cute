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
		ct_containers_delContainer(manager, manager->containers[manager->container_count-1]);
	}
		
	
	manager->container_count = 0;
}


ctContainer*
ct_containers_newContainer(ctContainerManager* manager, uint32_t size) {

	ctContainer* con;

	uint8_t* ptr = malloc(
		sizeof(ctContainer) +
		sizeof(ctAtom) * size +
		sizeof(ctAtomTypeSize) * size
	);

	con = (ctContainer*) ptr;
	con->atoms = (ctAtom*) (ptr + sizeof(ctContainer));
	con->types = (ctAtomTypeSize*) (ptr + sizeof(ctContainer) + sizeof(ctAtom)*size);


	con->id = manager->internal_id;
	manager->internal_id++;
	con->ref_count = 0;
	con->size = size;
	memset(con->types, ctAtomType_NoneType, size);
	
	ct_container_manager_pushToArray(manager, con);
	return con;
}


void
ct_containers_delContainer(ctContainerManager* manager, ctContainer* con) {
	ct_containers_removeFromArray(manager, con);
	free(con);
}

void
ct_containers_incRef(ctContainerManager* manager, ctContainer* con) {
	con->ref_count++;
}

void
ct_containers_decRef(ctContainerManager* manager, ctContainer* con) {

	if (con->ref_count == 0) {
		ct_containers_delContainer(manager, con);
		return;
	}

	con->ref_count--;
}

ctTypedAtom
ct_containers_conGet(ctContainerManager* manager, ctContainer* con, uint32_t index) {

	if (index >= con->size) {
		printf("OutOfBounds\n");
		exit(1);
	}

	return ct_atom_pack(con->atoms[index], con->types[index]);
}

void
ct_containers_conSet(ctContainerManager* manager, ctContainer* con, uint32_t index, ctTypedAtom atom) {
	if (index >= con->size) {
		printf("OutOfBounds\n");
		exit(1);
	}

	con->atoms[index] = atom.atom;
	con->types[index] = atom.type;
}