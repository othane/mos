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
 
caddr_t _sbrk(int incr)
{
	return (caddr_t)0;
}
 
int _write(int file, char *ptr, int len)
{
	int i;
	#if 0
	for (i = 0; i < len; i++)
		ITM_SendChar((uint32_t)ptr[i]);
	#endif
	return i;
}

