/*
 * XvidDecoder -- Blizzard's DivxDecoder ABI implemented on top of xvidcore.
 *
 * The client loads a decoder module by name (src/ui/CSimpleMovieFrame.cpp:72)
 * and resolves exactly four symbols from it:
 *
 *     InitializeDivxDecoder(index, width, height)
 *     SetOutputFormat(index, 1, width, height)
 *     DivxDecode(index, decoder_data_t*, 0)
 *     UnInitializeDivxDecoder(index)
 *
 * All four return 0 on success -- see CSimpleMovieFrame.cpp:568 and :718,
 * which treat a non-zero result as failure.
 *
 * Blizzard's DivxDecoder.dll statically links the codec, so it imports nothing
 * but KERNEL32. This file is built the same way: link xvidcore *statically* and
 * the result is a single self-contained DLL.
 */

#include <string.h>
#include <xvid.h>

#if defined(_WIN32)
    #define XVDEC_EXPORT __declspec(dllexport)
    #if !defined(_WIN64)
        /* x86 uses __cdecl; x64 has only one calling convention */
        #define XVDEC_CALL __cdecl
    #else
        #define XVDEC_CALL
    #endif
#else
    #define XVDEC_EXPORT __attribute__((visibility("default")))
    #define XVDEC_CALL
#endif

/* Layout must match decoder_data_t in src/ui/CSimpleMovieFrame.cpp:48 */
typedef struct {
    void*        output;
    void*        input;
    unsigned int input_size;
    int          update;
    int          zero0;
    int          zero1;
} decoder_data_t;

/*
 * Decoder slots. The client hands out 1-based indices
 * (m_decoder = ++s_decoderIndex, CSimpleMovieFrame.cpp:561) and only ever has a
 * handful of movie frames alive, so a small fixed table is enough.
 */
#define XVDEC_MAX_SLOTS 16

typedef struct {
    void*        handle;
    unsigned int width;
    unsigned int height;
} xvdec_slot_t;

static xvdec_slot_t s_slots[XVDEC_MAX_SLOTS];
static int          s_globalInit = 0;

static int xvdec_global_init(void) {
    xvid_gbl_init_t gbl;

    if (s_globalInit) {
        return 0;
    }

    memset(&gbl, 0, sizeof(gbl));
    gbl.version = XVID_VERSION;

    if (xvid_global(0, XVID_GBL_INIT, &gbl, 0) < 0) {
        return 1;
    }

    s_globalInit = 1;
    return 0;
}

static void xvdec_destroy_slot(xvdec_slot_t* slot) {
    if (slot->handle) {
        xvid_decore(slot->handle, XVID_DEC_DESTROY, 0, 0);
        slot->handle = 0;
    }
}

XVDEC_EXPORT int XVDEC_CALL InitializeDivxDecoder(unsigned int index, unsigned int width, unsigned int height) {
    xvid_dec_create_t create;

    if (index >= XVDEC_MAX_SLOTS) {
        return 1;
    }

    if (xvdec_global_init()) {
        return 1;
    }

    /* Re-initializing a live slot is not an error; drop the old decoder. */
    xvdec_destroy_slot(&s_slots[index]);

    memset(&create, 0, sizeof(create));
    create.version = XVID_VERSION;
    create.width   = (int)width;
    create.height  = (int)height;

    if (xvid_decore(0, XVID_DEC_CREATE, &create, 0) < 0) {
        return 1;
    }

    s_slots[index].handle = create.handle;
    s_slots[index].width  = width;
    s_slots[index].height = height;

    return 0;
}

/*
 * The client always passes one for the second argument. In Blizzard's decoder
 * that selects the output colorspace; the only format the client can consume is
 * 32 bits per pixel, because the movie textures are created as GxTex_Argb8888
 * (CSimpleMovieFrame.cpp:614) over a width*height*4 buffer (:598). So the value
 * is recorded for reference and the colorspace is fixed at BGRA.
 */
XVDEC_EXPORT int XVDEC_CALL SetOutputFormat(unsigned int index, unsigned int one, unsigned int width, unsigned int height) {
    (void)one;

    if (index >= XVDEC_MAX_SLOTS || !s_slots[index].handle) {
        return 1;
    }

    s_slots[index].width  = width;
    s_slots[index].height = height;

    return 0;
}

XVDEC_EXPORT int XVDEC_CALL DivxDecode(unsigned int index, void* data, unsigned int zero) {
    decoder_data_t*  d = (decoder_data_t*)data;
    xvid_dec_frame_t frame;

    (void)zero;

    if (index >= XVDEC_MAX_SLOTS || !s_slots[index].handle || !d) {
        return 1;
    }

    memset(&frame, 0, sizeof(frame));
    frame.version   = XVID_VERSION;
    frame.general   = XVID_LOWDELAY;
    frame.bitstream = d->input;
    frame.length    = (int)d->input_size;

    if (d->update && d->output) {
        /*
         * output points into the frame's image buffer, already offset by
         * s_imageDataOffsets[...] (CSimpleMovieFrame.cpp:710). One row is
         * width*4 bytes -- 3200 for the 800-wide formats, 4096 for the
         * 1024-wide ones, matching the stride column of s_strideData.
         *
         * xvid writes the RGB colorspaces bottom-up, following the Windows DIB
         * convention; the client uploads rows top-down. XVID_CSP_VFLIP makes
         * xvid emit them in the order the textures expect.
         */
        frame.output.csp       = XVID_CSP_BGRA | XVID_CSP_VFLIP;
        frame.output.plane[0]  = d->output;
        frame.output.stride[0] = (int)(s_slots[index].width * 4);
    } else {
        /* Catch-up frame: advance decoder state without producing pixels. */
        frame.output.csp = XVID_CSP_NULL;
    }

    if (xvid_decore(s_slots[index].handle, XVID_DEC_DECODE, &frame, 0) < 0) {
        return 1;
    }

    return 0;
}

XVDEC_EXPORT int XVDEC_CALL UnInitializeDivxDecoder(unsigned int index) {
    if (index >= XVDEC_MAX_SLOTS || !s_slots[index].handle) {
        return 1;
    }

    xvdec_destroy_slot(&s_slots[index]);

    return 0;
}
