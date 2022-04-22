#ifndef HEADER_PATH_H
#define HEADER_PATH_H

#include <string>

/**
 * Path to a installed data.
 */
class Path {
	public:
		enum FileType {
				INTERNAL = 1,
				EXTERNAL = 2
		};
    private:
        std::string m_path;
        FileType m_type;
    private:
        static std::string localizePath(const std::string &original);
        static Path constructPath(const std::string &file, const FileType &type);
        explicit Path(const std::string &file, const FileType &type);
    public:
        static Path dataSystemPath(const std::string &file);
        static Path dataUserPath(const std::string &file);

        std::string getPosixName() const { return m_path; }
        int getType() const { return m_type; }
        std::string getNative() const;
        bool exists() const;
        std::string read() const;
        bool write(const std::string &data) const;
};

#endif

