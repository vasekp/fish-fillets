#ifndef HEADER_PATH_H
#define HEADER_PATH_H

#include <string>

/**
 * File to a installed data.
 */
class File {
public:
	static File internal(const std::string &path);
	static File external(const std::string &path);

	std::string getPath() const { return m_path; }
	bool exists() const;
	std::string read() const;
	bool write(const std::string &data) const;

private:
	std::string m_path;
	enum class FileType {
		internal,
		external
	} m_type;

	explicit File(std::string path, FileType type) : m_path(path), m_type(type) { }
	static std::string localizePath(const std::string &original);
};

#endif

