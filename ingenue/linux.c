/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ingenue.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

struct InFileHandle_
{
	int fd;
};

#if 0
InFile in_file_open(InStr path, InAllocator alloc, InFileMode mode)
{
	uint32_t posixMode = O_RDONLY;
	if(mode == IN_FILE_MODE_READWRITE) {
		posixMode = O_RDWR;
	} else if(mode == IN_FILE_MODE_WRITEONLY) {
		posixMode = O_WRONLY | O_APPEND;
	}

	size_t fileSize = 0;
	bool fileValid = false;
	InFileHandle* fh = alloc.memalloc(sizeof(*fh));
	if(fh != NULL) {
		fh->fd = open(pathCstr, posixMode);
		fileValid = (fh->fd != -1);
		if(fileValid) {
			struct stat st;
			fstat(fh->fd, &st);
			fileSize = st.st_size;
		}
	}

	path.allocator->memfree(pathCstr);
	return (InFile){
		.handle = fh,
		.mode = mode,
		.path = path,
		.alloc = alloc,
		.size = fileSize,
		.open = fileValid
	};
}

InFile in_file_open_and_create(InStr path, InAllocator alloc, bool clear)
{
	char* pathCstr = in_str_alloc_cstr(path);
	size_t fileSize = 0;
	bool fileValid = false;
	InFileHandle* fh = alloc.memalloc(sizeof(*fh));
	if(fh != NULL) {
		int flags = O_WRONLY | O_CREAT;
		if(clear) {
			flags |= O_TRUNC;
		}

		fh->fd = open(pathCstr, flags);
		fileValid = (fh->fd != -1);
		if(fileValid) {
			struct stat st;
			fstat(fh->fd, &st);
			fileSize = st.st_size;
		}
	}

	path.allocator->memfree(pathCstr);
	return (InFile){
		.handle = fh,
		.mode = IN_FILE_MODE_WRITEONLY,
		.path = path,
		.alloc = alloc,
		.size = fileSize,
		.open = fileValid
	};
}

void in_file_close(InFile file)
{
	if(file.open) {
		close(file.handle->fd);
		file.alloc.memfree(file.handle);
	}
}

bool in_file_delete_from_system_path(const char* path)
{
	return (remove(path) == 0);
}

InStr in_file_read(InFile file, size_t from, size_t length)
{
}

bool in_file_write_str(InFile file, InStr str)
{
}

bool in_file_write_bytes(InFile file, uint8_t* bytes, size_t length)
{
}

void in_file_flush(InFile file)
{
	fsync(file.handle->fd);
}
#endif


int main(int argc, char **argv)
{
	gInDefaultMallocator = &(InAllocator) {
		.memalloc = malloc,
		.memrealloc = realloc,
		.memfree = free,
	};

	return in_main(argc, argv);
}
