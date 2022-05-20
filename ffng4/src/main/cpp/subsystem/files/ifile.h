#ifndef FISH_FILLETS_IFILE_H
#define FISH_FILLETS_IFILE_H

class IFile {
public:
    virtual ~IFile() { }

    virtual bool exists() const = 0;
    virtual std::string read() const = 0;
    virtual bool write(const std::string &data) const = 0;
};

#endif //FISH_FILLETS_IFILE_H
