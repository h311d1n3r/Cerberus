#ifndef CERBERUS_RUST_LIB_INSTALLER_H
#define CERBERUS_RUST_LIB_INSTALLER_H

#include <binaries/lib/install/lib_installer.h>
#include <utils/file_downloader.h>

class RustLibInstaller : public LibInstaller {
private:
    FileDownloader* downloader;
public:
    RustLibInstaller(std::string work_dir) : LibInstaller(work_dir) {
        downloader = new FileDownloader();
    }
    bool install_lib(LIBRARY* lib) override;
};

#endif //CERBERUS_RUST_LIB_INSTALLER_H
