// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <memory>
#include <string>

#include "common/logging/log.h"
#include "common/make_unique.h"
#include "common/string_util.h"

#include "core/file_sys/archive_romfs.h"
#include "core/hle/kernel/process.h"
#include "core/hle/service/fs/archive.h"
#include "core/loader/3dsx.h"
#include "core/loader/elf.h"
#include "core/loader/ncch.h"
#if ENABLE_BINARY_TRANSLATION
#include "core/binary_translation/binary_translation_loader.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Loader {

const std::initializer_list<Kernel::AddressMapping> default_address_mappings = {
    { 0x1FF50000,   0x8000, true  }, // part of DSP RAM
    { 0x1FF70000,   0x8000, true  }, // part of DSP RAM
    { 0x1F000000, 0x600000, false }, // entire VRAM
};

u32 ROMCodeStart;
u32 ROMCodeSize;
u32 ROMReadOnlyDataStart;
u32 ROMReadOnlyDataSize;

FileType IdentifyFile(FileUtil::IOFile& file) {
    FileType type;

#define CHECK_TYPE(loader) \
    type = AppLoader_##loader::IdentifyType(file); \
    if (FileType::Error != type) \
        return type;

    CHECK_TYPE(THREEDSX)
    CHECK_TYPE(ELF)
    CHECK_TYPE(NCCH)

#undef CHECK_TYPE

    return FileType::Unknown;
}

FileType IdentifyFile(const std::string& file_name) {
    FileUtil::IOFile file(file_name, "rb");
    if (!file.IsOpen()) {
        LOG_ERROR(Loader, "Failed to load file %s", file_name.c_str());
        return FileType::Unknown;
    }

    return IdentifyFile(file);
}

FileType GuessFromExtension(const std::string& extension_) {
    std::string extension = Common::ToLower(extension_);

    if (extension == ".elf" || extension == ".axf")
        return FileType::ELF;

    if (extension == ".cci" || extension == ".3ds")
        return FileType::CCI;

    if (extension == ".cxi")
        return FileType::CXI;

    if (extension == ".3dsx")
        return FileType::THREEDSX;

    return FileType::Unknown;
}

const char* GetFileTypeString(FileType type) {
    switch (type) {
    case FileType::CCI:
        return "NCSD";
    case FileType::CXI:
        return "NCCH";
    case FileType::CIA:
        return "CIA";
    case FileType::ELF:
        return "ELF";
    case FileType::THREEDSX:
        return "3DSX";
    case FileType::Error:
    case FileType::Unknown:
        break;
    }

    return "unknown";
}

ResultStatus LoadFile(const std::string& filename) {
    ROMCodeStart = 0;
    ROMCodeSize = 0;
    ROMReadOnlyDataStart = 0;
    ROMReadOnlyDataSize = 0;
    FileUtil::IOFile file(filename, "rb");
    if (!file.IsOpen()) {
        LOG_ERROR(Loader, "Failed to load file %s", filename.c_str());
        return ResultStatus::Error;
    }

    std::string filename_filename, filename_extension;
    Common::SplitPath(filename, nullptr, &filename_filename, &filename_extension);

    FileType type = IdentifyFile(file);
    FileType filename_type = GuessFromExtension(filename_extension);

    if (type != filename_type) {
        LOG_WARNING(Loader, "File %s has a different type than its extension.", filename.c_str());
        if (FileType::Unknown == type)
            type = filename_type;
    }

    LOG_INFO(Loader, "Loading file %s as %s...", filename.c_str(), GetFileTypeString(type));

    ResultStatus status = ResultStatus::Error;
    switch (type) {

    //3DSX file format...
    case FileType::THREEDSX:
    {
        AppLoader_THREEDSX app_loader(std::move(file), filename_filename, filename);
        // Load application and RomFS
        if (ResultStatus::Success == app_loader.Load()) {
            Service::FS::RegisterArchiveType(Common::make_unique<FileSys::ArchiveFactory_RomFS>(app_loader), Service::FS::ArchiveIdCode::RomFS);
            status = ResultStatus::Success;
        }
        break;
    }

    // Standard ELF file format...
    case FileType::ELF:
        status = AppLoader_ELF(std::move(file), filename_filename).Load();
        break;

    // NCCH/NCSD container formats...
    case FileType::CXI:
    case FileType::CCI:
    {
        AppLoader_NCCH app_loader(std::move(file), filename);

        // Load application and RomFS
        if (ResultStatus::Success == app_loader.Load()) {
            Service::FS::RegisterArchiveType(Common::make_unique<FileSys::ArchiveFactory_RomFS>(app_loader), Service::FS::ArchiveIdCode::RomFS);
            status = ResultStatus::Success;
        }
        break;
    }

    // CIA file format...
    case FileType::CIA:
        status = ResultStatus::ErrorNotImplemented;
        break;

    // Error occurred durring IdentifyFile...
    case FileType::Error:

    // IdentifyFile could know identify file type...
    case FileType::Unknown:
    {
        LOG_CRITICAL(Loader, "File %s is of unknown type.", filename.c_str());
        status = ResultStatus::ErrorInvalidFormat;
    }
    }
    #if ENABLE_BINARY_TRANSLATION
    if (status == ResultStatus::Success)
    {
        std::unique_ptr<FileUtil::IOFile> optimized_file(new FileUtil::IOFile(filename + ".obj", "rb"));
        if (!optimized_file->IsOpen())
        {
            LOG_WARNING(Loader, "Failed to load optimized file %s.obj", filename.c_str());
        }
        else
        {
            BinaryTranslationLoader::Load(*optimized_file);
        }
    }
    #endif
    return status;
}

} // namespace Loader
