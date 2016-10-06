#pragma once

#include "library/sp.h"
#include "library/strings.h"
#include <cstdint>
#include <istream>

#define PROGRAM_NAME "OpenApoc"
#define PROGRAM_ORGANISATION "OpenApoc"

namespace OpenApoc
{

class IFileImpl
{
  public:
	virtual ~IFileImpl();
};

class IFile : public std::istream
{
  private:
	std::unique_ptr<IFileImpl> f;
	IFile();
	friend class FileSystem;

  public:
	~IFile() override;
	size_t size() const;
	std::unique_ptr<char[]> readAll();
	bool readule16(uint16_t &val);
	bool readule32(uint32_t &val);
	const UString &fileName() const;
	const UString &systemPath() const;
	IFile(IFile &&other);
};

class FileSystem
{
  private:
	UString writeDir;

  public:
	FileSystem(std::vector<UString> paths);
	~FileSystem();
	IFile open(const UString &path);
	UString getCorrectCaseFilename(const UString &path);
	std::list<UString> enumerateDirectory(const UString &path, const UString &extension) const;
};

} // namespace OpenApoc
