

#include "unity_bundle.h"
#include "binary_utility.h"

#include <lz4.h>

/* 内部用 */
namespace unity_bundle
{
	enum ECompressionType : int8_t
	{
		None,
		Lzma,
		Lz4,
		Lz4HC,
		Lzham,
	};

	enum EArchiveFlags : uint16_t
	{
		CompressionTypeMask = 0x3f,
		BlocksAndDirectoryInfoCombined = 0x40,
		BlocksInfoAtTheEnd = 0x80,
		OldWebPluginCompatibility = 0x100,
		BlockInfoNeedPaddingAtStart = 0x200
	};

	struct StorageBlock
	{
		uint32_t decompressedSize = 0;
		uint32_t compressedSize = 0;
		uint16_t flag = 0;
	};

	struct Node
	{
		uint64_t offset = 0;
		uint64_t size = 0;
		uint32_t flags = 0;
		std::string_view path;
	};
} /* namespace unity_bundle */

std::vector<std::string> unity_bundle::DecompressUnityFs(const std::string& file)
{
	binary_utility::CBinaryReader fileReader(file.data(), file.length());

	std::string_view signature = fileReader.readNullterminatedString();
	if (signature != "UnityFS")return {};

	const uint32_t version = fileReader.readUInt32Be();
	std::string_view unityVersion = fileReader.readNullterminatedString();
	std::string_view unityRevision = fileReader.readNullterminatedString();

	std::vector<uint8_t> decompedssedBlockInfo;

	const uint64_t nFileSize = fileReader.readUInt64Be();
	const uint32_t nCompressedBlockSize = fileReader.readUInt32Be();
	const uint32_t nDecompressedBlockSize = fileReader.readUInt32Be();
	const uint32_t headerFlags = fileReader.readUInt32Be();

	if (version >= 7)
	{
		fileReader.setPos((fileReader.pos() + 0xf) & ~0xf);
	}

	if (fileReader.pos() + nCompressedBlockSize >= file.size()) return {};

	std::string_view blockInfoBytes;
	if (headerFlags & EArchiveFlags::BlocksInfoAtTheEnd)
	{
		blockInfoBytes = { &file[nFileSize - nCompressedBlockSize] , nCompressedBlockSize };
	}
	else
	{
		blockInfoBytes = { &file[fileReader.pos()], nCompressedBlockSize };
		fileReader.setPos(fileReader.pos() + blockInfoBytes.size());
	}

	ECompressionType blockInfoCompressionType = static_cast<ECompressionType>(headerFlags & EArchiveFlags::CompressionTypeMask);
	switch (blockInfoCompressionType)
	{
	case ECompressionType::Lzma:
		break;
	case ECompressionType::Lz4:
	case ECompressionType::Lz4HC:
		decompedssedBlockInfo.resize(nDecompressedBlockSize);
		::LZ4_decompress_safe(
			blockInfoBytes.data(),
			reinterpret_cast<char*>(decompedssedBlockInfo.data()),
			nCompressedBlockSize, nDecompressedBlockSize);
		break;
	default:
		break;
	}

	binary_utility::CBinaryReader decompressedBlockReader(decompedssedBlockInfo.data(), decompedssedBlockInfo.size());
	decompressedBlockReader.skip(16);

	uint32_t nBlockCount = decompressedBlockReader.readUInt32Be();
	std::vector<StorageBlock> storageBlocks(nBlockCount);
	for (uint32_t i = 0; i < nBlockCount; ++i)
	{
		storageBlocks[i] =
		{
			.decompressedSize = decompressedBlockReader.readUInt32Be(),
			.compressedSize = decompressedBlockReader.readUInt32Be(),
			.flag = decompressedBlockReader.readUInt16Be()
		};
	}

	uint32_t nNodeCount = decompressedBlockReader.readUInt32Be();
	std::vector<Node> nodes(nNodeCount);
	for (uint32_t i = 0; i < nNodeCount; ++i)
	{
		nodes[i] =
		{
			.offset = decompressedBlockReader.readUInt64Be(),
			.size = decompressedBlockReader.readUInt64Be(),
			.flags = decompressedBlockReader.readUInt32Be(),
			.path = decompressedBlockReader.readNullterminatedString(),
		};
	}

	if (headerFlags & EArchiveFlags::BlockInfoNeedPaddingAtStart)
	{
		fileReader.setPos((fileReader.pos() + 0xf) & ~0xf);
	}

	std::vector<std::string> decompressedBlocks;
	for (const auto& storageBlock : storageBlocks)
	{
		if (fileReader.pos() + storageBlock.compressedSize >= file.size())continue;

		std::string decompedssedBlockBuffer;
		std::string_view compressedBlockBytes;

		compressedBlockBytes = { &file[fileReader.pos()], storageBlock.compressedSize };
		fileReader.setPos(fileReader.pos() + storageBlock.compressedSize);

		ECompressionType compressionType = static_cast<ECompressionType>(storageBlock.flag & EArchiveFlags::CompressionTypeMask);
		switch (blockInfoCompressionType)
		{
		case ECompressionType::Lzma:
			break;
		case ECompressionType::Lz4:
		case ECompressionType::Lz4HC:
			decompedssedBlockBuffer.resize(storageBlock.decompressedSize);
			::LZ4_decompress_safe(
				compressedBlockBytes.data(),
				reinterpret_cast<char*>(decompedssedBlockBuffer.data()),
				storageBlock.compressedSize, storageBlock.decompressedSize);
			break;
		default:
			break;
		}

		decompressedBlocks.push_back(std::move(decompedssedBlockBuffer));
	}

	return decompressedBlocks;
}
