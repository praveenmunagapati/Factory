/*
	'U', 'u' で始まる K は unq + direct

	uuid "_m" で mtx

	[tablehash]/
		record/
			00/00/00/00/[rec-hash] = { K, V, K, V, ... }

		direct/
			00/00/00/00/[key-hash]/
			00/00/00/00/[val-hash]/[rec-hash] = { }

	filingCase2.id = { uuid }
	table = { T, T, T, ... }
*/

#include "FilingCase2.h"
