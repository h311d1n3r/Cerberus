#ifndef CERBERUS_LIB_INSTALLER_H
#define CERBERUS_LIB_INSTALLER_H

#include <string>
#include <binaries/bin_types.h>

class LibInstaller {
protected:
    std::string work_dir;
public:
    LibInstaller(std::string work_dir) : work_dir(work_dir) {}
    virtual bool install_lib(LIBRARY* lib) = 0;
};

#endif //CERBERUS_LIB_INSTALLER_H
