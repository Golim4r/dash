/** @file dart_translation.c
 *  @date 25 Aug 2014
 *  @brief Implementation for the operations on translation table.
 */

#ifndef ENABLE_LOG
#define ENABLE_LOG
#endif
#include "dart_deb_log.h"
#include "dart_translation.h"
#include "dart_mem.h"

/* Global array: the header for the global translation table. */
node_t dart_transtable_globalalloc;

MPI_Win dart_win_local_alloc;
MPI_Win dart_sharedmem_win_local_alloc;


int dart_adapt_transtable_create ()
{
	dart_transtable_globalalloc = NULL;
	return 0;
}

int dart_adapt_transtable_add (info_t item)
{
	node_t pre, q;
	node_t p = (node_t) malloc (sizeof (node_info_t));
	p -> trans.seg_id = item.seg_id;
	p -> trans.size = item.size;
	p -> trans.win = item.win;
	p -> trans.disp = item.disp;
	p -> next = NULL;

	/* The translation table is empty. */
	if (dart_transtable_globalalloc == NULL)
	{
		dart_transtable_globalalloc = p;
	}

	/* Otherwise, insert the added item into the translation table. */
	else
	{
		q = dart_transtable_globalalloc;
		while (q != NULL)
		{
			pre = q;
			q = q -> next;
		}
		pre -> next = p;
	}
	return 0;
}

int dart_adapt_transtable_remove (int16_t seg_id)
{
	node_t pre, p;
	p = dart_transtable_globalalloc;
  	if (seg_id == (p -> trans.seg_id))
	{
		dart_transtable_globalalloc = p -> next;
	}
        else
	{
	 	while ((p != NULL ) && (seg_id != (p->trans.seg_id)))
		{
			pre = p;
			p = p -> next;
		}
		if (!p)
		{
			ERROR ("Invalid seg_id: %d,can't remove the record from translation table", seg_id);
			return -1;
		}
	 	pre -> next = p -> next;
	}

	free (p->trans.disp);
	free (p);
	return 0;
}

int dart_adapt_transtable_get_win (int16_t seg_id, MPI_Win* win)
{
	node_t p;
	p = dart_transtable_globalalloc;

	while ((p != NULL) && (seg_id > ((p -> trans).seg_id)))
	{
		p = p -> next;
	}
	if ((!p) || (seg_id != ((p -> trans).seg_id)))
	{
		
		ERROR ("Invalid seg_id: %d, can not get the related window object", seg_id);
		return -1;
	}

	*win = (p -> trans).win;
	return 0;
}

/*
int dart_adapt_transtable_get_addr (uin16_t index, int offset, int* base, void **addr)
{
	node_t p, pre;
	p = transtable_globalalloc [index];

	while ((p != NULL) && (offset >= (p -> trans.offset)))
	{
		pre = p;
		p = p -> next;
	}

	if (pre -> trans.offset + pre -> trans.size <= offset)
	{
		return -1;
	}


	*base = (pre -> trans).offset;
	*addr = (pre -> trans).addr;
	return 0;
}
*/

int dart_adapt_transtable_get_disp (int16_t seg_id, dart_unit_t rel_unitid, MPI_Aint *disp_s)
{
	node_t p;
	p = dart_transtable_globalalloc;

	while ((p != NULL) && (seg_id > ((p -> trans).seg_id)))
	{
		p = p -> next;
	}

	if ((!p) || (seg_id != (p -> trans).seg_id))
	{
		ERROR ("Invalid seg_id: %d, can not get the related displacement", seg_id);
		return -1;
	}

	*disp_s = (p -> trans).disp[rel_unitid];
	return 0;
}

int dart_adapt_transtable_get_size (int16_t seg_id, size_t *size)
{
	node_t p;
	p = dart_transtable_globalalloc;

	while ((p != NULL) && (seg_id > ((p -> trans).seg_id)))
	{
		p = p -> next;
	}

	if ((!p) || (seg_id) != (p -> trans).seg_id)
	{
		ERROR ("Invalid seg_id: %d, can not get the related memory size", seg_id);
		return -1;
	}
	*size = (p -> trans).size;
	return 0;
}

int dart_adapt_transtable_destroy ()
{
	node_t p, pre;
	p = dart_transtable_globalalloc;
	if (p != NULL)
	{
		LOG ("Free up the translation table forcibly as there are still global memory blocks functioning on this team.");
	}
	while (p != NULL)
	{
		pre = p;
		p = p -> next;
		free (pre->trans.disp);
		free (pre);
	}
	dart_transtable_globalalloc = NULL;
	return 0;
}