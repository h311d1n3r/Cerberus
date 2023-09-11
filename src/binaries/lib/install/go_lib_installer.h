#ifndef CERBERUS_GO_LIB_INSTALLER_H
#define CERBERUS_GO_LIB_INSTALLER_H

#include <binaries/lib/install/lib_installer.h>

class GoLibInstaller : public LibInstaller {
public:
    GoLibInstaller(std::string work_dir, BIN_ARCH arch) : LibInstaller(work_dir, arch) {}
    bool install_lib(LIBRARY* lib) override;
};

#endif //CERBERUS_GO_LIB_INSTALLER_H
