#ifndef CERBERUS_GO_LIB_INSTALLER_H
#define CERBERUS_GO_LIB_INSTALLER_H

#include <binaries/lib/install/lib_installer.h>
#include <memory>

class GoLibInstaller : public LibInstaller {
private:
    bool install_go_version(std::string version);
    std::string goliath_output_dir;
public:
    GoLibInstaller(std::string work_dir, BIN_ARCH arch) : LibInstaller(work_dir, arch) {}
    bool install_lib(LIBRARY lib) override;
    bool pre_install_hook(std::vector<std::unique_ptr<LIBRARY>>& libs) override;
    bool post_install_hook() override;
};

#endif //CERBERUS_GO_LIB_INSTALLER_H
