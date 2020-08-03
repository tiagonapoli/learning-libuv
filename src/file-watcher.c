#include <uv.h>
#include <stdlib.h>
#include <stdio.h>

uv_loop_t* loop;

void printUsage() { printf("Usage ./file-watcher.out <file1> [file2]...\n"); }

void fs_event_cb(uv_fs_event_t* handle, const char* filename, int events, int status) {
	const size_t buf_sz = 100;
	char path[buf_sz];
	size_t sz = buf_sz;
	int err;

	int* cb_calls = (int*)handle->data;
	*cb_calls = *cb_calls + 1;
	err = uv_fs_event_getpath(handle, path, &sz);
	if (err != 0) {
		if (err == UV_ENOBUFS) {
			printf("[event-callback] %s Required buffer of size at least %ld, got %ld\n", uv_err_name(err), buf_sz, sz);
			return;
		}

		printf("[event-callback] %s Error\n", uv_err_name(err));
	} else {
		printf("Change detected in: %s [origin: %s] [handler_cb_calls: %d]\n", path, filename, *cb_calls);
		if (events & UV_RENAME) {
			printf("rename\n");
		}

		if (events & UV_CHANGE) {
			printf("change\n");
		}

		printf("\n");
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printUsage();
		exit(EXIT_FAILURE);
	}

	loop = uv_default_loop();

	int err;
	for (int i = 1; i < argc; i++) {
		printf("Adding watch to file: %s\n", argv[i]);
		uv_fs_event_t* fs_event_handle = (uv_fs_event_t*)malloc(sizeof(uv_fs_event_t));

        int* cb_calls = (int*)malloc(sizeof(int));
        *cb_calls = 0;
		fs_event_handle->data = cb_calls;
		uv_fs_event_init(loop, fs_event_handle);
		err = uv_fs_event_start(fs_event_handle, fs_event_cb, argv[i], UV_FS_EVENT_RECURSIVE);
		if (err != 0) {
			printf("uv_fs_event_start: %s\n", uv_err_name(err));
			exit(EXIT_FAILURE);
		}
	}

	uv_run(loop, UV_RUN_DEFAULT);
}