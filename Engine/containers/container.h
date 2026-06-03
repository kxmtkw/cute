

#ifndef CONTAINERS_CONTAINER_H
#define CONTAINERS_CONTAINER_H

#include <stdint.h>

#include "CuteAtom.h"
#include "CuteConfig.h"
#include "CuteInstr.h"
#include "engine/error.h"


typedef struct {
	uint32_t      internal_id;
	uint32_t      container_count;
	ctContainer*  containers[10];
} ctContainerManager;


void
ct_containers_init(ctContainerManager* manager);

void
ct_containers_end(ctContainerManager* manager);

ctContainer*
ct_containers_newContainer(ctContainerManager* manager, uint32_t size);

void
ct_containers_delContainer(ctContainerManager* manager, ctContainer* container);

void
ct_containers_incRef(ctContainerManager* manager, ctContainer* container);

inline void
ct_containers_decRef(ctContainerManager* manager, ctContainer* container);


// Get an atom in the container. Will return ctConManagerCode_OutOfBounds if index is out of bounds
ctTypedAtom
ct_containers_conGet(ctContainerManager* manager, ctContainer* container, uint32_t index, ctError* error);

// Set an atom in the container. Will return ctConManagerCode_OutOfBounds if index is out of bounds.
void
ct_containers_conSet(ctContainerManager* manager, ctContainer* container, uint32_t index, ctTypedAtom atom, ctError* error);


// Makes a clone of the container with the same atoms and types but different id
ctContainer*
ct_containers_conClone(ctContainerManager* manager, ctContainer* src, ctError* error);


// Resize the container
void
ct_containers_conResize(ctContainerManager* manager, ctContainer* con, uint32_t new_size, ctError* error);

#endif // CONTAINERS_CONTAINER_H