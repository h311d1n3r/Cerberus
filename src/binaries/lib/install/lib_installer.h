#ifndef CERBERUS_LIB_INSTALLER_H
#define CERBERUS_LIB_INSTALLER_H

#include <string>
#include <binaries/bin_types.h>

class LibInstaller {
protected:
    std::string work_dir;
    BIN_ARCH arch;
public:
    LibInstaller(std::string work_dir, BIN_ARCH arch) : work_dir(work_dir), arch(arch) {}
    virtual bool install_lib(LIBRARY* lib) = 0;
};

#endif //CERBERUS_LIB_INSTALLER_H
