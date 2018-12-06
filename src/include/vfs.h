#pragma once
#include "annotations.h"
#include "vfs.h"
#include "mem.h"

//Linked list of commands to queue up before accessing disk.
struct disk_command_t
{
	uint64_t LBA;
	size_t Length;
	void* SourceDest;
	void* ProcessQueue;
	disk_command_t* Next;
};

class IFile;

typedef result_t(*ReadDirCallback)(const char* entryName, uint64_t inodeNum, void* state);

class IINode
{
	public:
	IINode()
	{
		_fileSizeBytes = _inodeNumber = 0;
		_isFile = false;
	}

	virtual ~IINode() = 0;

	inline uint64_t GetInodeNumber()
	{
		return _inodeNumber;
	}

	//File Functions

	/*
	 Opens the file.
	 */
	virtual const IFile* OpenFile() = 0;

	inline bool IsFile()
	{
		return _isFile;
	}
	
	/*
	Zero bytes means invalid. Either the file has not been opened, does not exist, or this is a directory.
	*/
	inline size_t GetFileSize()
	{
		return _fileSizeBytes;
	}

	//Directory Functions

	virtual result_t ReadDirectory(_IN_CALLBACK_ ReadDirCallback callback, _IN_POINTER_ void* state) = 0;

	virtual result_t Unlink(const char* entryName) = 0;

	protected:
	size_t _fileSizeBytes;
	uint64_t _inodeNumber;
	bool _isFile;
};

class IFile
{
	IFile()
	{
		_inode = nullptr;
		_offset = 0;
	}

	//Closes the file.
	virtual ~IFile() = 0;

	virtual result_t Read(_IN_ARR_ void* dst, _IN_SIZE_ size_t bytesToRead, _IN_OPT_ _OUT_SIZE_ size_t* bytesRead = nullptr) = 0;

	virtual result_t Write(_IN_ARR_ void* src, _IN_SIZE_ size_t bytesToWrite, _IN_OPT_ _OUT_SIZE_ size_t* bytesWritten = nullptr) = 0;

	inline const IINode* GetInode()
	{
		return _inode;
	}

	inline size_t GetFileOffset()
	{
		return _offset;
	}

	inline result_t Seek(_IN_SIZE_ size_t bytes)
	{
		if (_offset + bytes >= _inode->GetFileSize())
		{
			return RES_FILE_SEEK_INVALID;
		}

		return RES_SUCCESS;
	}

	protected:
	IINode* _inode;
	size_t _offset;
};

class ISuperBlock
{
	public:
	ISuperBlock()
	{
		_rootInode = nullptr;
		_name = nullptr;
	}

	virtual ~ISuperBlock() = 0;

	virtual result_t LoadFromPartition() = 0;

	/*
	 Returns a pointer the INode that has number inodeNum.
	 */
	virtual const IINode* ReadINode(const uint64_t inodeNum) = 0;

	virtual result_t SyncFileSYstem() = 0;

	virtual result_t WriteSuperBlock() = 0;

	/*
	 Fills str with the partition name. It number of character put in str is the minimum
	 between length(str) and length(_name).
	 */
	inline void CopyName(_IN_POINTER_ char* const str)
	{
		size_t numChars = GetMin(MemoryLength(str), MemoryLength(_name));
		MemoryCopy(str, _name, numChars);
	}

	protected:
	IINode* _rootInode;
	char* _name;
	size_t _nameLen;
};

