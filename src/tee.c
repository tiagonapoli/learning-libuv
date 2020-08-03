#include <uv.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define STDOUT_FD 1
#define STDIN_FD 0

uv_pipe_t stdin_pipe, stdout_pipe, file_pipe;
uv_loop_t *loop;

typedef struct {
	uv_write_t req;
	uv_buf_t buf;
} write_req_t;

void free_buf(uv_write_t *req, int status, const char *stream_name) {
	write_req_t *r = (write_req_t *)req;
	free(r->buf.base);
	free(r);
	printf("Wrote to %s %ld bytes [status=%d]\n", stream_name, r->buf.len, status);
}

void on_stdout_write(uv_write_t *req, int status) { free_buf(req, status, "stdout"); }

void on_file_write(uv_write_t *req, int status) { free_buf(req, status, "file"); }

void write_data(uv_stream_t *dest, ssize_t size, const uv_buf_t *buf, uv_write_cb cb) {
	write_req_t *req = (write_req_t *)malloc(sizeof(write_req_t));
	req->buf = uv_buf_init((char *)malloc(size), size);
	memcpy(req->buf.base, buf->base, size);
	uv_write((uv_write_t *)req, (uv_stream_t *)dest, &req->buf, 1, cb);
}

void alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) { *buf = uv_buf_init((char *)malloc(suggested_size), suggested_size); }

void read_stdin(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
	if (nread < 0) {
		if (nread == UV_EOF) {
			uv_close((uv_handle_t *)&stdin_pipe, NULL);
		}
	} else if (nread > 0) {
		write_data((uv_stream_t *)&stdout_pipe, nread, buf, on_stdout_write);
		write_data((uv_stream_t *)&file_pipe, nread, buf, on_file_write);
	}

	if (buf->len > 0) {
		free(buf->base);
	}
}

void printUsage() { printf("Usage ./tee.out outputFile"); }

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printUsage();
		exit(EXIT_FAILURE);
	}

	loop = uv_default_loop();

	uv_pipe_init(loop, &stdin_pipe, 0);
	uv_pipe_open(&stdin_pipe, STDIN_FD);

	uv_pipe_init(loop, &stdout_pipe, 0);
	uv_pipe_open(&stdout_pipe, STDOUT_FD);

	uv_fs_t file_req;
	int fd = uv_fs_open(loop, &file_req, argv[1], O_CREAT | O_RDWR, S_IRGRP | S_IROTH | S_IRUSR | S_IWUSR, NULL);
	uv_pipe_init(loop, &file_pipe, 0);
	uv_pipe_open(&file_pipe, fd);

	uv_read_start((uv_stream_t *)&stdin_pipe, alloc, read_stdin);

	uv_run(loop, UV_RUN_DEFAULT);

	uv_fs_req_cleanup(&file_req);
}