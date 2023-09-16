#ifndef CERBERUS_LIB_INSTALLER_H
#define CERBERUS_LIB_INSTALLER_H

#include <string>
#include <binaries/bin_types.h>
#include <memory>
#include <vector>

class LibInstaller {
protected:
    std::string work_dir;
    BIN_ARCH arch;
public:
    LibInstaller(std::string work_dir, BIN_ARCH arch) : work_dir(work_dir), arch(arch) {}
    virtual bool install_lib(LIBRARY lib) = 0;
    virtual bool pre_install_hook(std::vector<std::unique_ptr<LIBRARY>>& libs) = 0;
    virtual bool post_install_hook() = 0;};

#endif //CERBERUS_LIB_INSTALLER_H
