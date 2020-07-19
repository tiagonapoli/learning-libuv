#include <stdio.h>
#include <uv.h>

static int counter = 0;
void wait_for_a_while(uv_idle_t* handler) {
	counter++;
	if (counter % 500000 == 0) {
		printf("Idling callback... %d\n", counter);
	}

	if (counter >= 1000000) {
		uv_idle_stop(handler);
	}
}

int main() {
	uv_idle_t idler;

	uv_idle_init(uv_default_loop(), &idler);
	uv_idle_start(&idler, wait_for_a_while);

	printf("Start event loop\n");
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	printf("Event loop finished\n");
	uv_loop_close(uv_default_loop());
	printf("Event loop closed\n");

	return 0;
}