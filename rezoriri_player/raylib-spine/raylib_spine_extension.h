#ifndef RAYLIB_SPINE_EXTENSION_H_
#define RAYLIB_SPINE_EXTENSION_H_

#include <spine/Extension.h>

class CRaylibSpineExtension : public spine::SpineExtension
{
protected:
	void* _alloc(size_t size, const char* file, int line) override;
	void* _calloc(size_t size, const char* file, int line) override;
	void* _realloc(void* ptr, size_t size, const char* file, int line) override;
	void _free(void* mem, const char* file, int line) override;
	char* _readFile(const spine::String& path, int* length) override;
};

#endif // !RAYLIB_SPINE_EXTENSION_H_
