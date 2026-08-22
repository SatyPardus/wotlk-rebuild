# XvidDecoder

A drop-in replacement for Blizzard's `DivxDecoder.dll`: it exports the same four
functions the client resolves at runtime, backed by
[xvidcore](https://github.com/ShiftMediaProject/xvid) instead of DivX.

Use it when you build the client for an architecture the shipped
`DivxDecoder.dll` does not support — that DLL is 32-bit only (`IMAGE_FILE_MACHINE_I386`),
so an x64 build cannot load it and `CSimpleMovieFrame::StartMovie` fails.

## The ABI

`src/ui/CSimpleMovieFrame.cpp` looks for a module named `DivxDecoder.dll`,
`XvidDecoder.dll`, `libXvidDecoder.dylib`, `XvidDecoder.dylib`,
`libXvidDecoder.so` or `XvidDecoder.so` (in that order) and resolves:

| function | notes |
|---|---|
| `int InitializeDivxDecoder(unsigned index, unsigned width, unsigned height)` | `index` is 1-based |
| `int SetOutputFormat(unsigned index, unsigned one, unsigned width, unsigned height)` | `one` is always 1 |
| `int DivxDecode(unsigned index, decoder_data_t* data, unsigned zero)` | |
| `int UnInitializeDivxDecoder(unsigned index)` | |

All four return **0 on success**. `__cdecl` on 32-bit Windows; the default
convention everywhere else.

```c
typedef struct {
    void*        output;      /* destination, already offset into the image buffer */
    void*        input;       /* one compressed frame */
    unsigned int input_size;
    int          update;      /* 0 = decode for state only, produce no pixels */
    int          zero0, zero1;
} decoder_data_t;
```

Output is 32 bits per pixel in BGRA byte order (the textures are created as
`GxTex_Argb8888`), with a row stride of `width * 4` — 3200 for the 800-wide
movie formats, 4096 for the 1024-wide ones, matching `s_strideData`.

## Building

xvidcore is vendored at `vendor/libxvidcore-1.3.7.1`, so there is nothing to
fetch. The module is built by default whenever that directory is present:

```
cmake -B build
cmake --build build --target XvidDecoder
```

The DLL lands in the build's `bin/` directory next to `Whoa.exe`. Nothing else
needs to be deployed — `libxvidcore.lib` is linked **statically**.

Set `-DWHOA_BUILD_XVID_DECODER=OFF` to skip it. To build against an xvid tree
elsewhere, use `-DXVIDCORE_ROOT=<path>`, or set `XVIDCORE_INCLUDE_DIR` and
`XVIDCORE_LIBRARY` directly.

### Which library gets linked

The vendored package ships two files per architecture in `lib/x86` and
`lib/x64`, and only one of them is right:

| file | what it is |
|---|---|
| `libxvidcore.lib` (~3.4 MB) | the static archive — **this is the one** |
| `xvidcore.lib` (~4 KB) | import library for `xvidcore.dll` — would put you back to two DLLs |

`find_library` only looks for the `libxvidcore` name, and there is a second
guard: if the selected library turns out to reference `xvidcore.dll`, the
configure step fails with an explanation instead of silently producing a
dependent DLL.

### CRT linkage

The vendored `libxvidcore.lib` is compiled against the **dynamic** CRT — its
object directives say `/DEFAULTLIB:"MSVCRT"` — so this target uses `/MD` to
match. Mixing them produces `LNK2038: mismatch detected for 'RuntimeLibrary'`.

That means the finished DLL imports `VCRUNTIME140.dll` and a handful of
`api-ms-win-crt-*` stubs, unlike Blizzard's `DivxDecoder.dll`, which imports
only `KERNEL32`. Those are OS-provided redistributables, not files you ship
alongside the game, so it is still a single deployed module. If you want the
KERNEL32-only profile, rebuild xvidcore with `/MT` and configure with
`-DXVIDDECODER_STATIC_CRT=ON`.

## Verifying you really got one DLL

```
dumpbin /dependents build/bin/XvidDecoder.dll
```

Anything beyond `KERNEL32.dll` means something is still linked dynamically; a
line mentioning `xvidcore.dll` means the import library slipped in.

## Caveats

- **Codec generation.** xvidcore decodes MPEG-4 Part 2 (DivX 4/5, Xvid). If a
  cinematic turns out to be DivX 3 / MS-MPEG4v3, `xvid_decore` returns
  `XVID_ERR_FORMAT` and every frame fails. Same shim, different backend: swap in
  libavcodec's `AV_CODEC_ID_MSMPEG4V3`.
- **Licensing.** xvidcore is GPL, so a statically linked `XvidDecoder.dll` is a
  GPL work. Fine for local builds; relevant if you distribute binaries.
  libavcodec (LGPL) is the friendlier choice for redistribution, at the cost of
  keeping it as a separate shared library.
- The code targets the xvid 1.3.x API (`xvid_global`, `xvid_decore`,
  `xvid_dec_create_t`, `xvid_dec_frame_t`).
