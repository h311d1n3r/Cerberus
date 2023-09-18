#ifndef CERBERUS_RUST_LIB_INSTALLER_H
#define CERBERUS_RUST_LIB_INSTALLER_H

#include <binaries/lib/install/lib_installer.h>
#include <utils/file_downloader.h>
#include <command/command_executor.h>

struct PATCH {
    std::string name;
    void (*patch_func)(std::string crate_path);
};

class RustBuildFixer {
private:
    std::string crate_path;
    BIN_TYPE type;
    PATCH* last_patch = nullptr;
    CommandExecutor executor;
public:
    RustBuildFixer(std::string crate_path, BIN_TYPE type) : crate_path(crate_path), type(type), executor(crate_path) {}
    bool process_error(std::string command, std::string error);
};

class RustLibInstaller : public LibInstaller {
private:
    FileDownloader downloader;
    void check_and_install_arch(std::string arch_name);
public:
    RustLibInstaller(std::string work_dir, BIN_ARCH arch, BIN_TYPE type);
    bool install_lib(LIBRARY lib) override;
    bool pre_install_hook(std::vector<std::unique_ptr<LIBRARY>>& libs) override {return true;}
    bool post_install_hook() override {return true;}
};

#endif //CERBERUS_RUST_LIB_INSTALLER_H
