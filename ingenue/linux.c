/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ingenue.h"
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

struct InFileHandle_
{
	int fd;
};

InFile in_file_open(char *path, InAllocator *alloc, InFileMode mode)
{
	int posixMode = O_RDONLY;
	if(mode == IN_FILE_MODE_READWRITE) {
		posixMode = O_RDWR;
	} else if(mode == IN_FILE_MODE_WRITEONLY) {
		posixMode = O_WRONLY | O_APPEND;
	}

	size_t fileSize = 0;
	bool fileValid = false;
	InFileHandle *fh = alloc->memalloc(sizeof(*fh));
	if(fh != NULL) {
		fh->fd = open(path, posixMode);
		fileValid = (fh->fd != -1);
		if(fileValid) {
			struct stat st;
			fstat(fh->fd, &st);
			fileSize = (size_t)st.st_size;
		}
	}

	return (InFile) {
		.handle = fh,
		.mode = mode,
		.path = incstr(path),
		.cursor = 0,
		.alloc = alloc,
		.size = fileSize,
		.open = fileValid
	};
}

InFile in_file_create(char *path, InAllocator *alloc)
{
	size_t fileSize = 0;
	bool fileValid = false;
	InFileHandle *fh = alloc->memalloc(sizeof(*fh));
	if(fh != NULL) {
		fh->fd = open(path, O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		fileValid = (fh->fd != -1);
		if(fileValid) {
			struct stat st;
			fstat(fh->fd, &st);
			fileSize = (size_t)st.st_size;
		}
	}

	return (InFile) {
		.cursor = 0,
		.handle = fh,
		.mode = IN_FILE_MODE_WRITEONLY,
		.path = incstr(path),
		.alloc = alloc,
		.size = fileSize,
		.open = fileValid
	};
}

void in_file_close(InFile file)
{
	if(file.open) {
		close(file.handle->fd);
		file.alloc->memfree(file.handle);
	}
}

bool in_file_delete_from_system(const char *path)
{
	return (remove(path) == 0);
}

void in_file_flush(InFile file)
{
	fsync(file.handle->fd);
}

bool in_file_set_cursor_pos(InFile *file, size_t pos)
{
	off_t curpos = 0;
	if(pos == gInFilePositionEOF) {
		curpos = lseek(file->handle->fd, 0, SEEK_END);
	} else {
		curpos = lseek(file->handle->fd, (off_t)pos, SEEK_SET);
	}

	if(curpos != -1) {
		file->cursor = (size_t)curpos;
		return true;
	}
	return false;
}

bool in_file_write_str(InFile file, InStr str)
{
	size_t datalen = str.length * sizeof(char);
	ssize_t result = write(file.handle->fd, str.data, datalen);
	return ((size_t)result == datalen);
}

bool in_file_write_bytes(InFile file, uint8_t *bytes, size_t length)
{
	ssize_t result = write(file.handle->fd, bytes, length);
	return ((size_t)result == length);
}

InStrBuf in_file_read(InFile file, size_t length, InStrBuf buf)
{
	size_t readlen = length;
	if(readlen + file.cursor > file.size) {
		readlen = file.size - file.cursor;
	}

	if(buf.capacity < readlen) {
		return (InStrBuf) { 0 };
	}

	ssize_t len = read(file.handle->fd, buf.str.data, readlen);
	if(len == -1) {
		return (InStrBuf) { 0 };
	}

	buf.str.length = (size_t)len;
	return buf;
}

InStrBuf in_file_read_all(InFile file, InStrBuf buf)
{
	in_file_set_cursor_pos(&file, 0);
	return in_file_read(file, file.size, buf);
}


int main(int argc, char **argv)
{
	gInDefaultMallocator = &(InAllocator) {
		.memalloc = malloc,
		.memrealloc = realloc,
		.memfree = free,
	};

	return in_main(argc, argv);
}
