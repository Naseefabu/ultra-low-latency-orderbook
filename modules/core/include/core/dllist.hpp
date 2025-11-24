#pragma once

namespace hft::core {

/*Intrusive Double linked list container.*/

struct ci_dllink_s
{
	struct ci_dllink_s *prev;
	struct ci_dllink_s *next;
};

using ci_dllink = ci_dllink_s;

struct ci_dllist
{
	ci_dllink l;
};

// Macros from CI Tools (adapted)
#define CI_DLLINK_INITIALISER(prev, next) { (prev), (next) }
#define CI_DLLIST_INITIALISER(name) \
	{                               \
		{                           \
			&(name).l, &(name).l    \
		}                           \
	}
#define CI_DLLIST_DECLARE(name) ci_dllist name = CI_DLLIST_INITIALISER(name)

// Commented out asserts for Compiler Explorer compatibility
// #define CI_DLLIST_LINK_ASSERT_VALID(l) \
// do{ \
// ci_assert_equal((((l)->prev))->next, l); \
// ci_assert_equal((((l)->next))->prev, l); \
//   }while(0)

inline void ci_dllist_init(ci_dllist *list)
{
	list->l.prev = list->l.next = &list->l;
}

inline int ci_dllist_is_empty(const ci_dllist *list)
{
	return list->l.next == &list->l;
}

inline int ci_dllist_not_empty(const ci_dllist *list)
{
	return list->l.next != &list->l;
}

inline void ci_dllist_insert_after(ci_dllink *before, ci_dllink *after)
{
	// CI_DLLIST_LINK_ASSERT_VALID(before);
	after->next = before->next;
	after->prev = before;
	before->next->prev = after;
	before->next = after;
}

inline void ci_dllist_insert_before(ci_dllink *after, ci_dllink *insert)
{
	// CI_DLLIST_LINK_ASSERT_VALID(after);
	insert->next = after;
	insert->prev = after->prev;
	after->prev->next = insert;
	after->prev = insert;
}

inline void ci_dllist_remove(ci_dllink *link)
{
	// CI_DLLIST_LINK_ASSERT_VALID(link);
	link->prev->next = link->next;
	link->next->prev = link->prev;
}

inline void ci_dllist_remove_safe(ci_dllink *link)
{
	// CI_DLLIST_LINK_ASSERT_VALID(link);
	link->prev->next = link->next;
	link->next->prev = link->prev;
	link->next = link->prev = link;
}

inline ci_dllink *ci_dllist_head(ci_dllist *list)
{
	return list->l.next;
}

inline const ci_dllink *ci_dllist_head(const ci_dllist *list)
{
	return list->l.next;
}

inline ci_dllink *ci_dllist_tail(ci_dllist *list)
{
	return list->l.prev;
}

inline const ci_dllink *ci_dllist_tail(const ci_dllist *list)
{
	return list->l.prev;
}

inline int ci_dllist_is_head(ci_dllist *list, ci_dllink *link)
{
	return link == list->l.next;
}

inline int ci_dllist_is_tail(ci_dllist *list, ci_dllink *link)
{
	return link == list->l.prev;
}

inline int ci_dllist_is_anchor(ci_dllist *list, ci_dllink *link)
{
	return link == &list->l;
}

inline void ci_dllist_push(ci_dllist *list, ci_dllink *link)
{
	// CI_DLLIST_LINK_ASSERT_VALID(&list->l);
	link->next = list->l.next;
	link->prev = &list->l;
	list->l.next = link->next->prev = link;
}

inline void ci_dllist_push_tail(ci_dllist *list, ci_dllink *link)
{
	// CI_DLLIST_LINK_ASSERT_VALID(&list->l);
	link->next = &list->l;
	link->prev = list->l.prev;
	list->l.prev = link->prev->next = link;
}

inline ci_dllink *ci_dllist_pop(ci_dllist *list)
{
	ci_dllink *l = list->l.next;
	ci_dllist_remove(l);
	return l;
}

inline ci_dllink *ci_dllist_pop_tail(ci_dllist *list)
{
	ci_dllink *l = list->l.prev;
	ci_dllist_remove(l);
	return l;
}

inline ci_dllink *ci_dllist_try_pop(ci_dllist *list)
{
	return ci_dllist_is_empty(list) ? nullptr : ci_dllist_pop(list);
}

inline ci_dllink *ci_dllist_try_pop_tail(ci_dllist *list)
{
	return ci_dllist_is_empty(list) ? nullptr : ci_dllist_pop_tail(list);
}

#define ci_dllist_put      ci_dllist_push_tail
#define ci_dllist_put_back ci_dllist_push
#define ci_dllist_get      ci_dllist_pop
#define ci_dllist_try_get  ci_dllist_try_pop

inline void ci_dllist_rehome(ci_dllist *to, ci_dllist *from)
{
	if (ci_dllist_is_empty(from))
	{
		ci_dllist_init(to);
	}
	else
	{
		to->l.next = from->l.next;
		to->l.prev = from->l.prev;
		to->l.next->prev = to->l.prev->next = &to->l;
		ci_dllist_init(from);
	}
}

inline void ci_dllist_join(ci_dllist *list, ci_dllist *from)
{
	if (ci_dllist_not_empty(from))
	{
		list->l.prev->next = from->l.next;
		from->l.next->prev = list->l.prev;
		list->l.prev = from->l.prev;
		from->l.prev->next = &list->l;
		ci_dllist_init(from);
	}
}

inline ci_dllink *ci_dllist_start(ci_dllist *list)
{
	return list->l.next;
}

inline const ci_dllink *ci_dllist_start(const ci_dllist *list)
{
	return list->l.next;
}

inline ci_dllink *ci_dllist_last(ci_dllist *list)
{
	return list->l.prev;
}

inline const ci_dllink *ci_dllist_last(const ci_dllist *list)
{
	return list->l.prev;
}

inline ci_dllink *ci_dllist_end(ci_dllist *list)
{
	return &list->l;
}

inline const ci_dllink *ci_dllist_end(const ci_dllist *list)
{
	return &list->l;
}

#define ci_dllist_iter(l) ((l) = (l)->next)

#define CI_DLLIST_FOR_EACH(p_lnk, p_list)                                  \
	for ((p_lnk) = ci_dllist_start(p_list);                                \
	     ((p_lnk) != ci_dllist_end(p_list)) || ((p_lnk) = nullptr, false); \
	     ci_dllist_iter(p_lnk))

#define CI_DLLIST_FOR_EACH_CONST(p_lnk, p_list)                            \
	for ((p_lnk) = ci_dllist_start(p_list);                                \
	     ((p_lnk) != ci_dllist_end(p_list)) || ((p_lnk) = nullptr, false); \
	     ci_dllist_iter(p_lnk))

#define container_of(ptr, type, member)                      \
	({                                                       \
		const decltype(((type *)0)->member) *__mptr = (ptr); \
		(type *)((char *)__mptr - offsetof(type, member));   \
	})

inline int ci_dllist_count(ci_dllist *list)
{
	ci_dllink *l;
	int count = 0;
	CI_DLLIST_FOR_EACH(l, list)
	++count;
	return count;
}

inline int ci_dllist_is_member(ci_dllist *list, ci_dllink *link)
{
	ci_dllink *l;
	CI_DLLIST_FOR_EACH(l, list)
	if (l == link)
		return 1;
	return 0;
}

inline void ci_dllink_self_link(ci_dllink *link)
{
	link->next = link->prev = link;
}

inline int ci_dllink_is_self_linked(ci_dllink *link)
{
	return link == link->next;
}

inline void ci_dllink_mark_free(ci_dllink *link)
{
	link->next = nullptr;
}

inline int ci_dllink_is_free(ci_dllink *link)
{
	return link->next == nullptr;
}

} // namespace hft::core