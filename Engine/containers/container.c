#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "CuteAtom.h"
#include "CuteConfig.h"
#include "CuteInstr.h"
#include "utils/utils.h"

#include "container.h"



void
ct_containers_init(ctContainerManager* manager) {
	manager->internal_id = 0;
	manager->bucket_count = 0;
	manager->bucket_capacity = 0;
	manager->buckets = NULL;

	ct_containers_newBucket(manager);
}


void
ct_containers_end(ctContainerManager* manager) {

	CUTE_LOG("containers", "Starting cleanup.\n");

	for (uint32_t i = 0; i < manager->bucket_count; i++) {
		ctContainerBucket* bucket = manager->buckets[i];

		for (uint32_t j = 0; j < sizeof(bucket->containers)/sizeof(bucket->containers[0]); j++) {
			if (ct_utils_isBitSet(bucket->bitmask, j)) {
				free(bucket->containers[j].atoms);
			}
		}
		free(bucket);
	}

	free(manager->buckets);
	manager->buckets = NULL;
	manager->bucket_capacity = 0;
	manager->bucket_count = 0;

	CUTE_LOG("containers", "All containers deallocated.\n");
}


uint32_t
ct_containers_newBucket(ctContainerManager* manager) {
	
	if (manager->bucket_count == manager->bucket_capacity) {
		manager->bucket_capacity = manager->bucket_capacity == 0 ? 1 : manager->bucket_capacity * 2;
		manager->buckets = realloc(manager->buckets, sizeof(ctContainerBucket*) * manager->bucket_capacity);
	}

	ctContainerBucket* bucket = malloc(sizeof(ctContainerBucket));

	if (bucket == NULL) {
		CUTE_LOG("containers", "Failed to allocate memory for new bucket.\n");
		return UINT32_MAX;
	}

	bucket->id = manager->bucket_count;
	memset(bucket->containers, 0, sizeof(bucket->containers));

	manager->buckets[manager->bucket_count] = bucket;
	manager->bucket_count++;

	CUTE_LOG("containers", "New bucket (%u) [%p] allocated.\n", bucket->id, bucket);
	return bucket->id;
};



ctContainer*
ct_containers_newContainer(ctContainerManager* manager, uint32_t size) {

	ctContainerBucket* assigned_bucket = NULL;
	ctContainer* assigned_con = NULL;

	// Looking for an not-filled bucket
	for (uint32_t i = 0; i < manager->bucket_count; i++) {

		ctContainerBucket* bucket = manager->buckets[i];

		if (bucket->bitmask == 0xFFFFFFFFFFFFFFFF) {
			continue;
		}

		assigned_bucket = bucket;
		break;
	}

	// Have to allocate a new bucket if no bucket was found
	if (!assigned_bucket) {
		uint32_t new_bucket_id = ct_containers_newBucket(manager);
		assigned_bucket = manager->buckets[new_bucket_id];
	}

	// Looking for a valid index to assign the container to in the bucket
	for (uint32_t j = 0; j < sizeof(assigned_bucket->containers)/sizeof(assigned_bucket->containers[0]); j++) {

		if (ct_utils_isBitSet(assigned_bucket->bitmask, j)) {
			continue;
		}

		assigned_con = &assigned_bucket->containers[j];
		assigned_con->bucket_id = assigned_bucket->id;
		assigned_con->bucket_index = j;

		ct_utils_setBit(&assigned_bucket->bitmask, j);

		break;
	};

	if (!assigned_con) {
		CUTE_LOG("containers", "Failed to allocate new container. No available slots in bucket (%u)\n", assigned_bucket->id);
		return NULL;
	}


	// Allocating atoms
	uint8_t* ptr = malloc(
		sizeof(ctAtom) * size +
		sizeof(ctAtomTypeSize) * size
	);

	if (ptr == NULL) {
		ct_utils_clearBit(&assigned_bucket->bitmask, assigned_con->bucket_index);
		CUTE_LOG("containers", "Failed to allocate memory for container (%u:%u) [%p] of size %u\n", assigned_con->bucket_id, assigned_con->bucket_index, assigned_con, size);
		return NULL;
	}

	assigned_con->atoms = (ctAtom*) (ptr);
	assigned_con->types = (ctAtomTypeSize*) (ptr + sizeof(ctAtom) * size);

	
	assigned_con->ref_count = 0;
	assigned_con->size = size;
	assigned_con->sub_containers = 0;
	memset(assigned_con->types, ctAtomType_NoneType, size);
	
	CUTE_LOG("containers", "New container (%u:%u) [%p] allocated to bucket (%u)\n", assigned_con->bucket_id, assigned_con->bucket_index, assigned_con, assigned_bucket->id);
	return assigned_con;
}


void
ct_containers_delContainer(ctContainerManager* manager, ctContainer* con) {

	uint32_t j = 0;
	
	for (uint32_t i = 0; i < con->size && j < con->sub_containers; i++) {
		if (con->types[i] == ctAtomType_Container) {
			ct_containers_decRef(manager, con->atoms[i].as_container);
			j++;
		}
	}
	
	ctContainerBucket* bucket = manager->buckets[con->bucket_id];
	ct_utils_clearBit(&bucket->bitmask, con->bucket_index);

	free(con->atoms);

	CUTE_LOG("containers", "Container (%u:%u) [%p] deallocated.\n", con->bucket_id, con->bucket_index, con);
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

	CUTE_LOG("containers", "Resized container (%u:%u) [%p] from %u to %u\n", con->bucket_id, con->bucket_index, con, temp.size, new_size);
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
	
	CUTE_LOG("containers", "Copied container (%u:%u) [%p] from container (%u:%u) [%p]\n", copy->bucket_id, copy->bucket_index, copy, src->bucket_id, src->bucket_index, src);

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
	
	CUTE_LOG("containers", "Cloned container (%u:%u) [%p] from container (%u:%u) [%p]\n", clone->bucket_id, clone->bucket_index, clone, src->bucket_id, src->bucket_index, src);

	return clone;
}