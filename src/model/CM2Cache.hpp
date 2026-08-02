#ifndef MODEL_C_M2_CACHE_HPP
#define MODEL_C_M2_CACHE_HPP

#include <cstdint>

class CM2Shared;

class CM2Cache {
    public:
        // Static variables
        static CM2Cache s_cache;

        // Member variables
        uint32_t m_initialized = 0;
        uint32_t m_flags = 0;
        //DWORD ukn3;
        //DWORD ukn4;
        CM2Shared* m_shared[1021];
        //DWORD ukn6;
        //DWORD ukn7;
        //DWORD ukn8;
        //DWORD ukn9;
        //DWORD ukn10;
        //DWORD ukn11;
        //DWORD ukn12;
        //DWORD ukn13[15];
        //DWORD ukn14;
        //DWORD ukn15[16];
        //TSLIst ukn16;

        // Member functions
        void BeginThread(void (*callback)(void*), void* arg);
        CM2Shared* CreateShared(const char*, uint32_t);
        void GarbageCollect(int32_t a2);
        int32_t Initialize(uint32_t flags);
        void UpdateShared();
        void WaitThread();
};

#endif
