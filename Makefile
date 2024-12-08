all: memtest_sys memtest_sys_dbg memtest_dev

memtest_sys: memtest.c
	@echo "Building memtest_sys (system malloc, no debug)..."
	gcc -o memtest_sys memtest.c
	@echo ""

memtest_sys_dbg: memtest.c
	@echo "Building memtest_sys (system malloc, debug enabled)..."
	gcc -o memtest_sys_dbg -DDEBUG memtest.c
	@echo ""

memtest_dev: memtest.c memory_manager.c memory_manager.h
	@echo "Building memtest_dev (our malloc, debug enabled)..."
	gcc -o memtest_dev -DDEBUG -Wno-deprecated-declarations memtest.c memory_manager.c
	@echo ""

clean:
	rm -f memtest_sys memtest_dev memtest_sys_dbg
