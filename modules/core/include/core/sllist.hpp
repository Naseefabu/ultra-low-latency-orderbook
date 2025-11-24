#pragma once

namespace hft::core {

/*Intrusive Single linked list container.*/

typedef struct ci_sllink_s
{
	struct ci_sllink_s *next;
} ci_sllink;


typedef struct
{
	ci_sllink *head;
} ci_sllist;

/**********************************************************************/

static inline int ci_sllink_busy(ci_sllink *link)
{
	return link->next != NULL;
}

/* It is needed to make ci_sllink_busy() works correctly if this link is
 * the last in the list.  We use even number here, so it can't accidentaly
 * match with any valid value, because all valid values are aligned.
 */
#define CI_SLLIST_TAIL ((ci_sllink *)(uintptr_t)0xdeaddead)

static inline void ci_sllist_init(ci_sllist *list)
{
	list->head = CI_SLLIST_TAIL;
}


static inline int ci_sllist_is_empty(ci_sllist *list)
{
	return list->head == CI_SLLIST_TAIL;
}

static inline int ci_sllist_not_empty(ci_sllist *list)
{
	return list->head != CI_SLLIST_TAIL;
}


static inline void ci_sllist_push(ci_sllist *list, ci_sllink *link)
{
	link->next = list->head;
	list->head = link;
}


static inline ci_sllink *ci_sllist_pop(ci_sllist *list)
{
	ci_sllink *link;
	link = list->head;
	list->head = link->next;
	link->next = NULL;
	return link;
}


static inline ci_sllink *ci_sllist_try_pop(ci_sllist *list)
{
	ci_sllink *link;
	link = list->head;
	if (link == CI_SLLIST_TAIL)
		return NULL;
	list->head = link->next;
	link->next = NULL; /* not busy any more */
	return link;
}

} // namespace hft::core