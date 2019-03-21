/*****************************************************************************
#                                                                            #
#    uStreamer - Lightweight and fast MJPG-HTTP streamer.                    #
#                                                                            #
#    Copyright (C) 2018  Maxim Devaev <mdevaev@gmail.com>                    #
#                                                                            #
#    This program is free software: you can redistribute it and/or modify    #
#    it under the terms of the GNU General Public License as published by    #
#    the Free Software Foundation, either version 3 of the License, or       #
#    (at your option) any later version.                                     #
#                                                                            #
#    This program is distributed in the hope that it will be useful,         #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of          #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           #
#    GNU General Public License for more details.                            #
#                                                                            #
#    You should have received a copy of the GNU General Public License       #
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.  #
#                                                                            #
*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../tools.h"
#include "../logging.h"

#include "path.h"
#include "static.h"


char *find_static_file_path(const char *root_path, const char *request_path) {
	char *simplified_path;
	char *path = NULL;
	struct stat st;

	simplified_path = simplify_request_path(request_path);
	if (simplified_path[0] == '\0') {
		goto error;
	}

	A_CALLOC(path, strlen(root_path) + strlen(simplified_path) + 32);
	sprintf(path, "%s/%s", root_path, simplified_path);

#	define LOAD_STAT { \
			if (lstat(path, &st) < 0) { \
				/* LOG_PERROR("Can't stat() file %s", path); */ \
				goto error; \
			} \
		}

	LOAD_STAT;
	if (S_ISDIR(st.st_mode)) {
		strcat(path, "/index.html");
		LOAD_STAT;
	}

#	undef LOAD_STAT

	if (!S_ISREG(st.st_mode)) {
		// LOG_ERROR("Not a regulary file: %s", path);
		goto error;
	}

	if (access(path, R_OK) < 0) {
		// LOG_PERROR("Can't access() R_OK file %s", path);
		goto error;
	}

	goto ok;

	error:
		if (path) {
			free(path);
		}
		path = NULL;

	ok:
		free(simplified_path);

	return path;
}
