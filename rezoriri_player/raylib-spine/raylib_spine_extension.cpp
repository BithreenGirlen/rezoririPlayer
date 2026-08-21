
#include "raylib_spine_extension.h"

#include <spine/SpineString.h> /* spine::String::buffer() */
#include <raylib.h>

void* CRaylibSpineExtension::_alloc(size_t size, [[maybe_unused]] const char* file, [[maybe_unused]] int line)
{
	return ::MemAlloc(size);
}

void* CRaylibSpineExtension::_calloc(size_t size, [[maybe_unused]] const char* file, [[maybe_unused]] int line)
{
	if (size == 0) return nullptr;

	void* p = ::MemAlloc(size);
	if (p != nullptr)
	{
		::memset(p, 0, size);
	}

	return p;
}

void* CRaylibSpineExtension::_realloc(void* ptr, size_t size, [[maybe_unused]] const char* file, [[maybe_unused]] int line)
{
	return ::MemRealloc(ptr, size);
}

void CRaylibSpineExtension::_free(void* mem, [[maybe_unused]] const char* file, [[maybe_unused]] int line)
{
	::MemFree(mem);
}

char* CRaylibSpineExtension::_readFile([[maybe_unused]] const spine::String& path, [[maybe_unused]] int* length)
{
	return reinterpret_cast<char*>(::LoadFileData(path.buffer(), length));
}

spine::SpineExtension* spine::getDefaultExtension()
{
	static CRaylibSpineExtension s_raylibSpineExtension;

	return &s_raylibSpineExtension;
}