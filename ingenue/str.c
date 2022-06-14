/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pch.h"
#include <stdarg.h>

InAllocator* gInDefaultMallocator = NULL;
extern InStr fmt_translate(InStrView seq, va_list* va);


InStr in_str_alloc(size_t capacity, InAllocator* allocator)
{
	InStr s = { 0 };
	s.length    = 0;
	s.mutable   = true;
	s.ownMemory = true;
	s.capacity  = capacity;
	if(allocator != NULL) {
		s.allocator = allocator;
	} else {
		s.allocator = gInDefaultMallocator;
	}

	s.data = s.allocator->memalloc(capacity);
	return s;
}

InStr in_str_alloc_from_literal(char* literal, InAllocator* alloc)
{
	size_t len = strlen(literal);
	InStr str = in_str_alloc(len, alloc);
	str = in_str_set_from_literal(str, literal);
	return str;
}

InStr in_str_alloc_from_view(InStrView v)
{
	InStr s = { 0 };
	s.length    = v.length;
	s.mutable   = true;
	s.ownMemory = true;
	s.capacity  = v.length;
	s.allocator = v.str.allocator;
	s.data      = s.allocator->memalloc(v.length);

	if(s.data != NULL) {
		memcpy(s.data, &v.str.data[v.start], v.length);
	}
	return s;
}

InStr in_str_immut_from_literal(char* literal)
{
	InStr s = { 0 };
	s.length    = strlen(literal);
	s.mutable   = false;
	s.ownMemory = false;
	s.capacity  = s.length;
	s.data      = literal;
	return s;
}


InStr in_str_emplace_into(size_t capacity, void* mem)
{
	InStr s = { 0 };
	s.length    = 0;
	s.mutable   = true;
	s.ownMemory = false;
	s.capacity  = capacity;
	s.data      = mem;
	return s;
}

void in_str_free(InStr str)
{
	if(str.ownMemory) {
		str.allocator->memfree(str.data);
	}
}

char* in_str_alloc_cstr(InStr str)
{
	char* cstr = str.allocator->memalloc((str.length + 1) * sizeof(char));
	if(cstr != NULL) {
		memcpy(cstr, str.data, str.length);
		cstr[str.length] = 0;
	}
	return cstr;
}

// C++ eat ur heart out
bool in_str_isnull(InStr s)
{
	return (s.data == NULL);
}

bool in_str_eq(InStr a, InStr b)
{
	size_t cmp = strncmp(a.data, b.data, in_min(a.length, b.length));
	return (a.length == b.length) && (cmp == 0);
}

bool in_str_eq_strview(InStr a, InStrView b)
{
	char* bview = &b.str.data[b.start];
	size_t cmp = strncmp(a.data, bview, in_min(a.length, b.length));
	return (a.length == b.length) && (cmp == 0);
}

void in_str_puts(InStr str, FILE* stream)
{
	fwrite(str.data, sizeof(char), str.length, stream);
}

void in_str_putv(InStrView v, FILE* stream)
{
	fwrite(&v.str.data[v.start], sizeof(char), v.length, stream);
}

InStr in_str_set_from_literal(InStr str, char* data)
{
	IN_ASSERT(str.mutable);
	InStr src = in_str_immut_from_literal(data);
	if(str.capacity >= src.length) {
		InStr s = str;
		s.length = src.length;
		memset(s.data, 0, str.capacity);
		memcpy(str.data, src.data, src.length);
		return s;
	} else {
		return gInNullStr;
	}
}

InStr in_str_copy(InStr dst, InStr src, size_t len)
{
	IN_ASSERT(dst.mutable);
	size_t cpylen = len;
	if(len == 0 || len > src.length) {
		cpylen = src.length;
	}

	size_t newlen = dst.length + cpylen;
	if(dst.capacity >= newlen) {
		InStr s = dst;
		s.length = newlen;
		memcpy(&s.data[dst.length], src.data, cpylen);
		return s;
	} else {
		IN_ASSERT(false);
		return gInNullStr;
	}
}

/**
 * Uses dst string allocator
 */
InStr in_str_copy_realloc(InStr dst, InStr src, size_t len)
{
	IN_ASSERT(dst.mutable && dst.ownMemory);
	size_t cpylen = len;
	if(len == 0 || len > src.length) {
		cpylen = src.length;
	}

	size_t newlen = dst.length + cpylen;
	if(dst.capacity < newlen) {
		dst.capacity = newlen;
		dst.data = dst.allocator->memrealloc(dst.data, newlen);
		if(dst.data == NULL) {
			return gInNullStr;
		}
	}

	InStr s = dst;
	s.length = newlen;
	memcpy(&s.data[dst.length], src.data, cpylen);
	return s;
}

InStr in_str_copy_from_view(InStr dst, InStrView v)
{
	v.str.data += v.start; // Jesus fuck
	InStr s = in_str_copy(dst, v.str, v.length);
	v.str.data -= v.start;
	return s;
}

InStr in_str_copy_literal(InStr dst, char* data)
{
	InStr src = in_str_immut_from_literal(data);
	return in_str_copy(dst, src, src.length);
}

/**
 * Uses dst string allocator
 */
InStr in_str_copy_from_view_realloc(InStr dst, InStrView v)
{
	IN_ASSERT(dst.mutable && dst.ownMemory);
	size_t newlen = dst.length + v.length;
	if(dst.capacity < newlen) {
		dst.capacity = newlen;
		dst.data = dst.allocator->memrealloc(dst.data, newlen);
		if(dst.data == NULL) {
			return gInNullStr;
		}
	}

	InStr s = dst;
	s.length = newlen;
	memcpy(&s.data[dst.length], &v.str.data[v.start], v.length);
	return s;
}

InStrView in_str_subview_at_first(InStr str, char from)
{
	InStrView v = { 0 };
	v.str = str;
	for(size_t s = 0; s < str.length; s++) {
		if(str.data[s] == from) {
			v.start = s + 1;
			v.length = str.length - v.start;
			return v;
		}
	}

	v.str = gInNullStr;
	return v;
}

InStrView in_str_subview_at_first_v(InStrView v, char from)
{
	char* dataoff = &v.str.data[v.start];
	InStr offstr = in_str_emplace_into(v.length, dataoff);
	offstr.length = v.length;

	InStrView r = in_str_subview_at_first(offstr, from);
	if(!in_str_isnull(r.str)) {
		r.str = v.str;
	}

	r.start += v.start;
	return r;
}

InStrRangedView in_str_subview_between(InStr s, char open, char close)
{
	InStrRangedView r = { 0 };
	InStrView first = in_str_subview_at_first(s, open);
	if(in_str_isnull(first.str)) {
		return r;
	}

	InStrView second = in_str_subview_at_first_v(first, close);
	if(in_str_isnull(second.str)) {
		return r;
	}

	first.length -= second.length + 1;
	r.snipped = first;
	r.remaining = second;
	return r;
}

InStrRangedView in_str_subview_between_v(InStrView v, char open, char close)
{

	InStrRangedView r = { 0 };
	InStrView first = in_str_subview_at_first_v(v, open);
	if(in_str_isnull(first.str)) {
		return r;
	}

	InStrView second = in_str_subview_at_first_v(first, close);
	if(in_str_isnull(second.str)) {
		return r;
	}

	first.length -= second.length + 1;
	r.snipped = first;
	r.remaining = second;
	return r;
}

InStr in_str_format(InStr fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	InStr s = in_str_format_va(fmt, va);
	va_end(va);
	return s;
}

InStr in_str_format_va(InStr fmt, va_list va)
{
	InStr result = in_str_alloc(fmt.capacity * 2, fmt.allocator);
	InStrView last = { .str = fmt };
	last.start = 0;
	last.length = 0;
	InStrRangedView sub = in_str_subview_between(fmt, '{', '}');

	while(!in_str_isnull(sub.snipped.str)) {
		InStrView skipped = last;
		skipped.start = last.start + last.length;
		skipped.length = sub.snipped.start - skipped.start - 1;
		if(skipped.str.data[skipped.start] == '}') {
			skipped.start += 1;
			skipped.length -= 1;
		}

		InStr fmtTranslate = fmt_translate(sub.snipped, &va);
		result = in_str_copy_from_view_realloc(result, skipped);
		result = in_str_copy_realloc(result, fmtTranslate, 0);
		in_str_free(fmtTranslate);

		last = sub.snipped;
		sub = in_str_subview_between_v(sub.remaining, '{', '}');
	}

	InStrView rem = last;
	rem.start = last.start + last.length + 1;
	rem.length = fmt.length - rem.start;
	result = in_str_copy_from_view_realloc(result, rem);
	return result;
}
