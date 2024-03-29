/*
SPDX-License-Identifier: GPL-2.0
SPDX-FileCopyrightText: 2021 Johan Söderlind Åström <johan.soderlind.astrom@gmail.com>
*/
#pragma once

#include "csc_assert.h"
#include "csc_dlist.h"
#include "csc_basic.h"


unsigned csc_hash8_buf (void const * buf, unsigned size8, unsigned max)
{
	ASSERT_ISPOW2 (max);
	unsigned key = 0;
	uint8_t const * b = buf;
	for (size_t i = 0; i < size8; i++)
	{
		key += b [i];
	}
	return key & (max - 1);
}


unsigned csc_hash8_str (char const * buf, char const * buf_end, unsigned max)
{
	ASSERT_ISPOW2 (max);
	unsigned key = 0;
	while (*buf)
	{
		if (buf[0] == '\0') {break;}
		if (buf_end && (buf >= buf_end)) {break;}
		key += (unsigned)buf [0];
		buf ++;
	}
	return key & (max - 1);
}


unsigned csc_hash8_str_ab (char const * a, char const * b, unsigned max)
{
	ASSERT_ISPOW2 (max);
	unsigned key = 0;
	while (1)
	{
		if (a[0] == '\0') {break;}
		if (a >= b) {break;}
		key += (unsigned)a[0];
		a ++;
	}
	return key & (max - 1);
}



struct csc_htablestr
{
	struct csc_dlist * slots; //Hashtable
	struct csc_dlist * node; //node+i*stride, can be inserted to the hashtable
	char * name; //name+i*stride
	unsigned htsize;
	unsigned items_max;
	unsigned name_stride;
	unsigned last;
};



void csc_htablestr_init (struct csc_htablestr * ht)
{
	ASSERT_PARAM_NOTNULL (ht);
	ASSERT_ISPOW2 (ht->htsize);
	ht->slots = calloc (ht->htsize, sizeof (struct csc_dlist));
	ht->node = calloc (ht->items_max, sizeof (struct csc_dlist));
	ht->name = calloc (ht->items_max * ht->name_stride, sizeof (char));
	csc_dlist_init_v (ht->slots, ht->htsize);
}


struct csc_dlist * csc_htablestr_get_nth_node (struct csc_htablestr * ht, unsigned index)
{
	ASSERT_PARAM_NOTNULL (ht);
	ASSERT (ht->node);
	ASSERT (index <= ht->last);
	return ht->node + index;
}


char * csc_htablestr_get_nth_name (struct csc_htablestr * ht, unsigned index)
{
	ASSERT_PARAM_NOTNULL (ht);
	ASSERT (ht->name);
	ASSERT (index <= ht->last);
	return ht->name + ht->name_stride * index;
}


int csc_htablestr_add (struct csc_htablestr * ht, char const * str, char const * str_end)
{
	ASSERT_PARAM_NOTNULL (ht);
	ASSERT_PARAM_NOTNULL (str);
	ASSERT_ISPOW2 (ht->htsize);
	ASSERT (ht->slots);
	ASSERT (ht->node);
	ASSERT (ht->name);
	ASSERT (ht->last < ht->items_max);
	struct csc_dlist * nodeslot = csc_htablestr_get_nth_node (ht, ht->last);
	char * name = csc_htablestr_get_nth_name (ht, ht->last);
	if (str_end)
	{
		ptrdiff_t l = MIN (str_end - str, ht->name_stride);
		memcpy (name, str, (size_t)l);
		ASSERT (strlen (name) <= (size_t)l);
	}
	else
	{
		snprintf (name, ht->name_stride, "%s", str);
	}
	unsigned key = csc_hash8_str (name, NULL, ht->htsize);
	csc_dlist_add_head (ht->slots + key, nodeslot);
	int index = (int)ht->last;
	ht->last++;
	return index;
}


int csc_htablestr_find (struct csc_htablestr const * ht, char const * needle, char const * needle_end)
{
	ASSERT_PARAM_NOTNULL (ht);
	ASSERT_PARAM_NOTNULL (needle);
	ASSERT_ISPOW2 (ht->htsize);
	ASSERT (ht->slots);
	ASSERT (ht->node);
	ASSERT (ht->name);
	unsigned key = csc_hash8_str (needle, needle_end, ht->htsize);
	struct csc_dlist * p = ht->slots + key;
	return csc_dlist_find_str (p, ht->node, needle, needle_end, ht->name, ht->name_stride, 0);
}


int csc_htablestr_getfind (struct csc_htablestr * ht, char const * needle, char const * needle_end)
{
	ASSERT_PARAM_NOTNULL (ht);
	ASSERT_PARAM_NOTNULL (needle);
	int index;
	index = csc_htablestr_find (ht, needle, needle_end);
	if (index < 0)
	{
		index = csc_htablestr_add (ht, needle, needle_end);
	}
	ASSERT (index >= 0);
	return index;
}






