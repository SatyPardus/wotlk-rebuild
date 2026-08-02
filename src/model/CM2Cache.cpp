#include "model/CM2Cache.hpp"
#include "gx/Device.hpp"
#include "model/CM2Shared.hpp"
#include "model/Model2.hpp"
#include "util/Filesystem.hpp"
#include "util/SFile.hpp"
#include <cstring>
#include <bc/Memory.hpp>
#include <storm/String.hpp>
#include <tempest/Box.hpp>

CM2Cache CM2Cache::s_cache;

void CM2Cache::BeginThread(void (*callback)(void*), void* arg) {
    // TODO
}

// OFFSET: 0x81C390
CM2Shared* CM2Cache::CreateShared(const char* path, uint32_t flags) {
    char convertedPath[STORM_MAX_PATH];
    if (!M2ConvertModelFileName(path, convertedPath, STORM_MAX_PATH, flags)) {
        return nullptr;
    }

    char* ext = OsPathFindExtensionWithDot(convertedPath);

    CAaBox v28;
    ModelBlobQuery(convertedPath, v28.b, v28.t);

    if (ext) {
        *ext = '.';
    }

    bool useFullPath = (flags & 0x10) != 0;
    const char* key;

    if (useFullPath) {
        key = convertedPath;
    } else {
        key = ext;
        bool foundKey = false;
        if (ext > convertedPath) {
            while (*key != '\\' && *key != '/') {
                if (--key <= convertedPath) {
                    foundKey = true;
                    break;
                }
            }

            if (!foundKey) {
                ++key;
            }
        }
    }

    unsigned int hash = 0;
    for (const char* p = key; *p; ++p)
        hash = hash * 19 + (unsigned char)*p;

    unsigned int bucket = hash % 1021;

    CM2Shared* node = this->m_shared[bucket];
    CM2Shared** slot = &this->m_shared[bucket];
    if (node) {
        int cmp = 0;
        bool found = false;

        while (node) {
            if (node->m_fileNameHash >= hash) {
                if (node->m_fileNameHash > hash)
                    break;

                const char* fileName = useFullPath ? node->m_filePath : node->m_fileNameWithoutPath;
                cmp = strcmp(fileName, key);
                if (cmp >= 0) {
                    found = true;
                    break;
                }
            }

            slot = &node->m_next;
            node = node->m_next;
        }

        if (found && cmp <= 0) {
            (*slot)->AddRef();
            return *slot;
        }
    }

    SFile* fileptr;

    if (SFile::OpenEx(nullptr, convertedPath, (flags >> 2) & 1, &fileptr)) {
        auto shared = NEW(CM2Shared, this);

        if (shared->Load(fileptr, flags & 0x4, &v28)) {
            strcpy(shared->m_filePath, convertedPath);
            shared->m_fileNameHash = hash;
            shared->ext = strrchr(shared->m_filePath, '.');
            shared->m_fileNameWithoutPath = shared->ext;

            if (shared->ext > shared->m_filePath) {
                while (*shared->m_fileNameWithoutPath != '\\' && *shared->m_fileNameWithoutPath != '/') {
                    --shared->m_fileNameWithoutPath;
                    if (shared->m_fileNameWithoutPath <= shared->m_filePath)
                        break;
                }
                if (shared->m_fileNameWithoutPath > shared->m_filePath)
                    ++shared->m_fileNameWithoutPath;
            }

            if ((flags & 0x8) == 0) {
                shared->m_previous = (CM2Shared*)slot;
                shared->m_next = *slot;

                if (*slot)
                    (*slot)->m_previous = (CM2Shared*)&shared->m_next;

                *slot = shared;
            }

            if ((flags & 0x40) != 0) {
                shared->m_flag40 = 1;
            }

            return shared;
        }

        SFile::Close(fileptr);
        DEL(shared);
    }

    return nullptr;
}

void CM2Cache::GarbageCollect(int32_t a2) {
    // TODO
}

int32_t CM2Cache::Initialize(uint32_t flags) {
    if (this->m_initialized) {
        // TODO

        return 1;
    }

    // TODO

    if (flags & 0x8) {
        if (GxCaps().m_shaderTargets[GxSh_Vertex] > GxShVS_none && GxCaps().m_shaderTargets[GxSh_Pixel] > GxShPS_none) {
            this->m_flags |= 0x8;
        }
    }

    // TODO

    this->m_initialized = 1;

    return 1;
}

void CM2Cache::UpdateShared() {
    // TODO
}

void CM2Cache::WaitThread() {
    // TODO
}
