#ifndef UNITY_BUNDLE_H_
#define UNITY_BUNDLE_H_

#include <string>
#include <vector>

namespace unity_bundle
{
	/// @brief Parse header and decompress the blocks
	/// @param file Binary UnityFS
	/// @return Decompressed blocks in binary format
	std::vector<std::string> DecompressUnityFs(const std::string& file);
}

#endif // !UNITY_BUNDLE_H_
