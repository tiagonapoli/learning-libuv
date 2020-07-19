#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <uv.h>
#include "./utils/utils.h"
#define BUFFER_LEN 16
#define STDOUT_FD_NO 1

static char buffer[BUFFER_LEN];
static uv_buf_t iobuf;
static uv_fs_t open_req, read_req, write_req;

void on_open(uv_fs_t *req);
void on_read(uv_fs_t *req);
void on_write(uv_fs_t *req);

void on_open(uv_fs_t *req) {
	// The argument passed to on_open is the same as the original request
	assert(req == &open_req);

	printf("=============\n");
	printf("File opened: fdNo=%ld\n", req->result);
	printf("=============\n");
	print_proc_fd();

	if (req->result >= 0) {
		iobuf = uv_buf_init(buffer, BUFFER_LEN);
		uv_fs_read(uv_default_loop(), &read_req, req->result, &iobuf, 1, -1, on_read);
	} else {
		printf("Error opening file: %s\n", uv_strerror((int)req->result));
	}
}

void on_read(uv_fs_t *req) {
	if (req->result < 0) {
		printf("Read error: %s\n", uv_strerror(req->result));
	} else if (req->result == 0) {
		uv_fs_t close_req;

		// In this case we give NULL to the callback argument, so the operation
		// will be SYNCHRONOUS
		uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
	} else if (req->result > 0) {
		iobuf.len = req->result;
		uv_fs_write(uv_default_loop(), &write_req, STDOUT_FD_NO, &iobuf, 1, -1, on_write);
	}
}

void on_write(uv_fs_t *req) {
	if (req->result < 0) {
		printf("Write error: %s\n", uv_strerror(req->result));
	} else {
		uv_fs_read(uv_default_loop(), &read_req, open_req.result, &iobuf, 1, -1, on_read);
	}
}

void print_usage() { printf("Usage ./cat.out filepath\n"); }

int main(int argc, char *argv[]) {
	if (argc < 2) {
		print_usage();
		return 1;
	}

	uv_fs_open(uv_default_loop(), &open_req, argv[1], O_RDONLY, 0, on_open);
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_fs_req_cleanup(&open_req);
	uv_fs_req_cleanup(&read_req);
	uv_fs_req_cleanup(&write_req);
	return 0;
}