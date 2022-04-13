#pragma once

#include <algorithm>
#include <memory>
#include <map>
#include <BlackBox/Core/smartptr.hpp>
#include <BlackBox/System/ZLib/IZlibDecompressor.h>

#if !BB_PLATFORM_LINUX && !BB_PLATFORM_ANDROID && !BB_PLATFORM_APPLE
	#pragma pack(push)
	#pragma pack(1)
	#define PACK_GCC
#else
	#define PACK_GCC __attribute__((packed))
#endif

/*
   4.3.6 Overall .ZIP file format:

      [local file header 1]
      [encryption header 1]
      [file data 1]
      [data descriptor 1]
      . 
      .
      .
      [local file header n]
      [encryption header n]
      [file data n]
      [data descriptor n]
      [archive decryption header] 
      [archive extra data record] 
      [central directory header 1]
      .
      .
      .
      [central directory header n]
      [zip64 end of central directory record]
      [zip64 end of central directory locator] 
      [end of central directory record]
*/

namespace ZipFile
{
	struct SRange
	{
		uint32 begin;
		uint32 end;
	};

	// compression methods
	enum
	{
		METHOD_STORE                             = 0,  // The file is stored (no compression)
		METHOD_SHRINK                            = 1,  // The file is Shrunk
		METHOD_REDUCE_1                          = 2,  // The file is Reduced with compression factor 1
		METHOD_REDUCE_2                          = 3,  // The file is Reduced with compression factor 2
		METHOD_REDUCE_3                          = 4,  // The file is Reduced with compression factor 3
		METHOD_REDUCE_4                          = 5,  // The file is Reduced with compression factor 4
		METHOD_IMPLODE                           = 6,  // The file is Imploded
		METHOD_TOKENIZE                          = 7,  // Reserved for Tokenizing compression algorithm
		METHOD_DEFLATE                           = 8,  // The file is Deflated
		METHOD_DEFLATE64                         = 9,  // Enhanced Deflating using Deflate64(tm)
		METHOD_IMPLODE_PKWARE                    = 10, // PKWARE Date Compression Library Imploding
		METHOD_DEFLATE_AND_ENCRYPT               = 11, // Deflate + Custom encryption (TEA)
		METHOD_DEFLATE_AND_STREAMCIPHER          = 12, // Deflate + stream cipher encryption on a per file basis
		METHOD_STORE_AND_STREAMCIPHER_KEYTABLE   = 13, // Store + Timur's encryption technique on a per file basis
		METHOD_DEFLATE_AND_STREAMCIPHER_KEYTABLE = 14, // Deflate + Timur's encryption technique on a per file basis
	};

	// end of Central Directory Record
	// followed by the .zip file comment (variable size, can be empty, obtained from nCommentLength)
	struct CDREnd
	{
		enum
		{
			SIGNATURE = 0x06054b50
		};
		uint32 lSignature;       // end of central dir signature    4 bytes  (0x06054b50)
		uint16 nDisk;            // number of this disk             2 bytes
		uint16 nCDRStartDisk;    // number of the disk with the start of the central directory  2 bytes
		uint16 numEntriesOnDisk; // total number of entries in the central directory on this disk  2 bytes
		uint16 numEntriesTotal;  // total number of entries in the central directory           2 bytes
		uint32 lCDRSize;         // size of the central directory   4 bytes
		uint32 lCDROffset;       // offset of start of central directory with respect to the starting disk number        4 bytes
		uint16 nCommentLength;   // .ZIP file comment length        2 bytes

		AUTO_STRUCT_INFO;

		// .ZIP file comment (variable size, can be empty) follows
	} PACK_GCC;

	struct SPakFileVersion
	{
		SPakFileVersion() = default;
		SPakFileVersion(byte major, byte minor)
		    : version(0)
		{
			version = major << 8 | minor << 0;
		}

		uint16 version;

		AUTO_STRUCT_INFO;
	};

	struct MemoryArena
	{
		constexpr static uint32 _base_size      = 0xffff;
		uint32                  _size           = _base_size;
		static constexpr float  _growing_factor = 1.6f;
		size_t                  _offset{};
		std::vector<char>       _data{};
		MemoryArena(uint32 size = 1)
		    : _size(size * _base_size)
		{
			_data.resize(this->_size);
		}

		void* alloc(size_t size)
		{
			assert(_offset + size < this->_size && "Not enough memory in arena!!!");

			bool enough_mem = (_offset + size) < this->_size;
			if (!enough_mem)
			{
				_data.resize(size_t(_data.size() * _growing_factor));
			}

			auto result = &_data[_offset];
			_offset += size;

			return result;
		}
		template<class T>
		T* alloc(size_t size)
		{
			return reinterpret_cast<T*>(alloc(size));
		}

		size_t size() { return _offset; }
		void*  data() { return &_data[0]; }
	};

	struct SVariableString
	{
		std::uint16_t size;
		char          data[1];

		SVariableString(std::uint16_t size, char* data)
		    : size(size)
		{
			strcpy((this->data), data);
		}
	};

	struct SDirEntry
	{
		std::int32_t    offset;
		uint32          size;
		char            test[256]{0};
		SVariableString file_name;

		SDirEntry(std::string_view file_name, std::int32_t offset, uint32 size)
		    : file_name(std::uint16_t(file_name.length()), (char*)file_name.data())
		    , offset(offset)
		    , size(size)
		{
		}

		string_view Name()
		{
			return string_view{file_name.data, file_name.size};
		}

		//static MemoryArena arena;

		//static SDirEntry*  alloc(size_t size)
		//{
		//	return (SDirEntry*)arena.alloc(size);
		//}
	};

	struct ArchiveInfo
	{
		uint32 NumberOfFiles{};
		uint32 FirstOffset{};
		uint32 CurrentOffset{};
	};

	// This descriptor exists only if bit 3 of the general
	// purpose bit flag is set (see below).  It is byte aligned
	// and immediately follows the last byte of compressed data.
	// This descriptor is used only when it was not possible to
	// seek in the output .ZIP file, e.g., when the output .ZIP file
	// was standard output or a non seekable device.  For Zip64 format
	// archives, the compressed and uncompressed sizes are 8 bytes each.
	struct DataDescriptor
	{
		uint32 lCRC32;            ///< crc-32                          4 bytes
		uint32 lSizeCompressed;   ///< compressed size                 4 bytes
		uint32 lSizeUncompressed; ///< uncompressed size               4 bytes

		AUTO_STRUCT_INFO;
	};

	// this is the local file header that appears before the compressed data
	// followed by:
	//    file name (variable size)
	//    extra field (variable size)
	struct LocalFileHeader
	{
		enum
		{
			SIGNATURE = 0x04034b50
		};
		uint32          lSignature;     // local file header signature     4 bytes  (0x04034b50)
		SPakFileVersion nVersionNeeded; // version needed to extract       2 bytes
		uint16          nFlags;         // general purpose bit flag        2 bytes
		uint16          nMethod;        // compression method              2 bytes
		uint16          nLastModTime;   // last mod file time              2 bytes
		uint16          nLastModDate;   // last mod file date              2 bytes
		DataDescriptor  desc;
		uint16          nFileNameLength;   // file name length                2 bytes
		uint16          nExtraFieldLength; // extra field length              2 bytes

		AUTO_STRUCT_INFO;
	};

	// the File Header as it appears in the CDR
	// followed by:
	//    file name (variable size)
	//    extra field (variable size)
	//    file comment (variable size)
	struct CentralDirectory
	{
		enum
		{
			SIGNATURE = 0x02014b50
		};
		uint32         lSignature;     // central file header signature   4 bytes  (0x02014b50)
		uint16         nVersionMadeBy; // version made by                 2 bytes
		uint16         nVersionNeeded; // version needed to extract       2 bytes
		uint16         nFlags;         // general purpose bit flag        2 bytes
		uint16         nMethod;        // compression method              2 bytes
		uint16         nLastModTime;   // last mod file time              2 bytes
		uint16         nLastModDate;   // last mod file date              2 bytes
		DataDescriptor desc;
		uint16         nFileNameLength;    // file name length                2 bytes
		uint16         nExtraFieldLength;  // extra field length              2 bytes
		uint16         nFileCommentLength; // file comment length             2 bytes
		uint16         nDiskNumberStart;   // disk number start               2 bytes
		uint16         nAttrInternal;      // internal file attributes        2 bytes
		uint32         lAttrExternal;      // external file attributes        4 bytes

		// This is the offset from the start of the first disk on
		// which this file appears, to where the local header should
		// be found.  If an archive is in zip64 format and the value
		// in this field is 0xFFFFFFFF, the size will be in the
		// corresponding 8 byte zip64 extended information extra field.
		enum
		{
			ZIP64_LOCAL_HEADER_OFFSET = 0xFFFFFFFF
		};
		uint32 lLocalHeaderOffset; // relative offset of local header 4 bytes

		AUTO_STRUCT_INFO;
	} PACK_GCC;

	struct SArchiveHandle
	{
		std::unique_ptr<CFileMapping> fm;
		// Entry of file
		LocalFileHeader*              header{};
		CDREnd*                       m_CentralDirectoryRecordEnd;

		SArchiveHandle(std::string_view file)
		    : SArchiveHandle(std::move(std::make_unique<CFileMapping>(file.data())))
		{
		}
		SArchiveHandle(std::unique_ptr<CFileMapping>&& fm)
		    : SArchiveHandle((void*)fm->getData(), fm->getSize())
		{
			// HACK!!!
			this->fm = std::move(fm);
		}

		SArchiveHandle(void* data, size_t size, std::unique_ptr<CFileMapping> fm = nullptr)
		    : fm(std::move(fm))
		{
			header                      = (LocalFileHeader*)data;
			m_CentralDirectoryRecordEnd = (CDREnd*)((char*)data + size - sizeof CDREnd);
		}

		operator LocalFileHeader&()
		{
			return *header;
		}

		operator bool()
		{
			return header != nullptr;
		}

		struct Iterator
		{
			using iterator_category = std::forward_iterator_tag;
			using difference_type   = std::ptrdiff_t;
			using value_type        = CentralDirectory;
			using pointer           = value_type*;
			using reference         = value_type&;

			Iterator(pointer ptr)
			    : m_ptr(ptr)
			{
			}
			reference operator*() const { return *m_ptr; }
			pointer   operator->() { return m_ptr; }

			// Prefix increment
			Iterator& operator++()
			{
				m_ptr = pointer((byte*)m_ptr +
				                // clang-format off
				(
					sizeof CentralDirectory + 
					m_ptr->nFileNameLength +
					m_ptr->nFileCommentLength +
					m_ptr->nExtraFieldLength
				));
				// clang-format on
				return *this;
			}

			// Postfix increment
			Iterator operator++(int)
			{
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

			friend bool operator==(const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
			friend bool operator!=(const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };
			/////////////////////////////////////////////

		private:
			pointer m_ptr;
			byte*   base;
		};

		Iterator begin() { return Iterator(reinterpret_cast<Iterator::pointer>((byte*)header + m_CentralDirectoryRecordEnd->lCDROffset)); }
		Iterator end() { return Iterator((Iterator::pointer)((byte*)m_CentralDirectoryRecordEnd)); }
	};

	std::string_view remove_leading_ups(std::string_view str);
	void             WriteArchiveRecursive(ArchiveInfo& ar, std::filesystem::directory_entry dir_entry, std::ofstream& of);

	void             WriteDirectoryEntry(std::ofstream& of, ZipFile::ArchiveInfo& ar, std::filesystem::directory_entry dir_entry);

	template<class Header>
	void WriteTime(Header& cd);
	void FillLocalFileHeaderAndCentralDirectory(
	    const std::string_view& reduced_path,
	    unsigned int            file_size,
	    void*                   file_data,
	    std::ofstream&          of,
	    ArchiveInfo&            info);

	void           CreateArchive(const std::string& pattern, const std::string out_file);
	SArchiveHandle OpenArchive(std::string_view file);
	void           List(const string& file);
	void           create_file(LocalFileHeader& ar, fs::path filename, SRange range);
	void           create_file(LocalFileHeader& ar, fs::path filename, uint32 offset, uint32 size);
	void           Extract(const string& file, const string& base, const string& pattern);

#if 0
	template<typename F>
	void foreach (LocalFileHeader& ar, F func)
	{
		auto* entry = reinterpret_cast<SDirEntry*>((byte*)&ar + ar.toc_offset);
		for (size_t i = 0; i < ar.number_of_files; i++)
		{
			func(ar, entry);
			entry = (SDirEntry*)((byte*)entry->file_name.data + entry->file_name.size + 1);
		}
	}
#endif

#include <unordered_map>

	struct File : public _i_reference_target_t
	{
		std::uint32_t offset{};
		uint32        size{};
		uint32        compressed_size{};
		string_view   name{};
		char*         base{};
		bool          compressed  = false;
		bool          need_delete = true;
		File(
		    std::uint32_t offset,
		    uint32        size,
		    uint32        compressed_size,
		    string_view   name,
		    char*         base,
		    bool          compressed,
		    bool          need_delete)
		    : offset(offset)
		    , size(size)
		    , compressed_size(compressed_size)
		    , name(name)
		    , base(base)
		    , compressed(compressed)
		    , need_delete(need_delete)
		{
		}
		File() {}
		~File()
		{
			//FIXME: rewrite this HELL
			if (compressed && need_delete)
				delete[] base;
		}

		File(ZipFile::CentralDirectory& entry, void* header)
		{
			auto  name       = string_view((char*)header + entry.lLocalHeaderOffset + sizeof LocalFileHeader, entry.nFileNameLength); //
			bool  compressed = entry.desc.lSizeCompressed != entry.desc.lSizeUncompressed;
			auto& lfh        = *(LocalFileHeader*)((char*)header + entry.lLocalHeaderOffset);
			File  file{entry.lLocalHeaderOffset + sizeof LocalFileHeader + lfh.nFileNameLength + lfh.nExtraFieldLength, entry.desc.lSizeUncompressed, entry.desc.lSizeCompressed, name, (char*)header, compressed, false};

			std::swap(*this, file);
		}

		static File* CopyToHeap(File* file)
		{
#ifndef RC_COMPILER
			IZLibInflateStream* pInflateStream = ((CSystem*)(Env::System()))->GetIZLibDecompressor()->CreateInflateStream();

			File*               result;
			char*               BaseHeapAddres = new char[file->size];
			{
				pInflateStream->SetOutputBuffer(const_cast<char*>(BaseHeapAddres), file->size);
				pInflateStream->Input(file->base + file->offset, file->compressed_size);
				pInflateStream->EndInput();
				EZInflateState state = pInflateStream->GetState();
				assert(state == eZInfState_Finished);
				pInflateStream->Release();

				if (state == eZInfState_Error)
				{
					return nullptr;
				}
			}
			result = DEBUG_NEW File{0, file->size, file->compressed_size, file->name, BaseHeapAddres, file->compressed, true};

			return result;
#else
			return nullptr;
#endif
		}

		static File* CreateFrom(File* file)
		{
			if (file->compressed)
			{
				return CopyToHeap(file);
			}
			else
			{
				return file;
			}
		}
	};

	struct MyFile
	{
		_smart_ptr<File> m_pFileData{};
		std::atomic_bool m_fileSlotInUse{};
		long             m_nCurSeek{};
		int              m_nFlags{};

		MyFile(File* file)
		    : m_pFileData(file)
		    , m_fileSlotInUse(false)
		{
		}

		MyFile()
		    : m_nCurSeek(0)
		    , m_pFileData(nullptr)
		    , m_nFlags(0)
		    , m_fileSlotInUse(false)
		{
		}
		~MyFile()             = default;

		MyFile(const MyFile&) = delete;
		MyFile& operator=(const MyFile&) = delete;

		MyFile(MyFile&& other)
		    : m_nCurSeek(other.m_nCurSeek)
		    , m_pFileData(std::move(other.m_pFileData))
		    , m_nFlags(other.m_nFlags)
		    , m_fileSlotInUse(other.m_fileSlotInUse.exchange(false))
		{
		}
		MyFile& operator=(MyFile&& other)
		{
			m_nCurSeek      = other.m_nCurSeek;
			m_pFileData     = std::move(other.m_pFileData);
			m_nFlags        = other.m_nFlags;
			m_fileSlotInUse = other.m_fileSlotInUse.exchange(false);
			return *this;
		}

		bool TryConstruct(File* pFileData, unsigned nFlags)
		{
			bool expectedInUse = false;
			if (m_fileSlotInUse.compare_exchange_strong(expectedInUse, true))
			{
				m_pFileData = pFileData;
				m_nFlags    = nFlags;
				m_nCurSeek  = 0;
				return true;
			}
			return false;
		}

		void Destruct()
		{
			assert(m_pFileData);
			assert(m_fileSlotInUse);
			// mark it free, and deallocate the pseudo file memory
			m_pFileData     = NULL;

			m_fileSlotInUse = false;
		}

		File* GetFile() { return m_pFileData; }

	public:
		int FRead(void* dst, size_t size, size_t nCount, FILE* file)
		{
			if (!GetFile())
				return 0;

			size_t nTotal = size * nCount;
			if (!nTotal || (unsigned)m_nCurSeek >= GetFileSize())
				return 0;
			if (nTotal > GetFileSize() - m_nCurSeek)
			{
				nTotal = GetFileSize() - m_nCurSeek;
				if (nTotal < size)
					return 0;
				nTotal -= nTotal % size;
			}
			{
				auto offset = m_pFileData->base + m_pFileData->offset + m_nCurSeek;
				memcpy(dst, offset, nTotal);

				m_nCurSeek += nTotal;
				return nTotal;
			}
			return 0;
		}

		bool     Eof() { return m_nCurSeek < Size(); }
		long     FTell() { return m_nCurSeek; }
		unsigned GetFileSize() { return m_pFileData->size; }
		//////////////////////////////////////////////////////////////////////////
		int      FSeek(long nOffset, int nMode)
		{
			if (!m_pFileData)
				return -1;

			switch (nMode)
			{
			case SEEK_SET:
				m_nCurSeek = nOffset;
				break;
			case SEEK_CUR:
				m_nCurSeek += nOffset;
				break;
			case SEEK_END:
				m_nCurSeek = GetFileSize() - nOffset;
				break;
			default:
				assert(0);
				return -1;
			}
			return 0;
		}

		size_t Size()
		{
			return m_pFileData->size;
		}
	};

	class CPak
	{
	public:
		using KeyType = string_view;
		template<typename Type>
		using MapType =
#if 0
		std::unordered_map<KeyType,Type>
#else
		    std::map<KeyType, Type>
#endif
		    ;

		ZipFile::File create_file(ZipFile::CentralDirectory& entry, void* header)
		{
#if 0
			auto name       = string_view((char*)header + entry.lLocalHeaderOffset + sizeof LocalFileHeader, entry.nFileNameLength); //
			bool compressed = entry.desc.lSizeCompressed != entry.desc.lSizeUncompressed;
			File file{entry.lLocalHeaderOffset + sizeof LocalFileHeader + name.length(), entry.desc.lSizeUncompressed, entry.desc.lSizeCompressed, name, (char*)header, compressed};
#endif
			assert(0);
			return File{};
		}

		using FileList = MapType<_smart_ptr<File>>;
		bool OpenPak(string_view pak)
		{
			SArchiveHandle ar{pak};
			if (!ar) return false;

			for (auto& entry : ar)
			{
				_smart_ptr<File> file = DEBUG_NEW File(entry, ar.header);

				if (auto it = m_Files.find(file->name); it != m_Files.end()) printf("Eroror, file %s already mapped\n", file->name.data());

				m_Files[file->name] = file;
			}
			m_Archives.emplace_back(std::move(ar));
			return true;
		}
		//_smart_ptr<MyFile>
		//_smart_ptr<MyFile>
		//FOpen(string_view fname)
		//{
		//	if (auto it = m_Files.find(fname); it != m_Files.end()) return _smart_ptr<MyFile>(new MyFile(File::CreateFrom(&it->second)));
		//	return nullptr;
		//}
		void Dump()
		{
			for (auto& f : m_Files)
			{
				printf("%*.*s\n\tcompressed: %s\n", f.first.size(), f.first.size(), f.first.data(), f.second->compressed ? "true" : "false");
			}
		}

	public:
		FileList                    m_Files;
		std::vector<SArchiveHandle> m_Archives;
	};

} // namespace ZipFile

#undef PACK_GCC

#if !BB_PLATFORM_LINUX && !CRY_PLATFORM_ANDROID && !CRY_PLATFORM_APPLE
	#pragma pack(pop)
#endif
