#include <sys/stat.h>
#include <hal.h>

int _close(int file)
{
	return 0;
}
 
int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}
 
int _isatty(int file)
{
	return 1;
}
 
int _lseek(int file, int ptr, int dir)
{
	return 0;
}
 
int _open(const char *name, int flags, int mode)
{
	return -1;
}
 
int _read(int file, char *ptr, int len)
{
	return 0;
}

weak unsigned char _heap_low; // defined by the linker
weak unsigned char _heap_top; // "
caddr_t _sbrk(int incr)
{
	// bare bones heap allocator
	static unsigned char *heap_end = 0;
	unsigned char *prev_heap_end;

	// initialize
	if(heap_end == 0)
		heap_end = &_heap_low;

	prev_heap_end = heap_end;
	if(heap_end + incr > &_heap_top)
		// heap overflow (not good!)
		return NULL;
	heap_end += incr;

	return (caddr_t) prev_heap_end;
}
 
int _write(int file, char *ptr, int len)
{
	return 0;
}

