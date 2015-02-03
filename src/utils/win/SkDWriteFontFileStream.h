/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkDWriteFontFileStream_DEFINED
#define SkDWriteFontFileStream_DEFINED

#include "SkTypes.h"

#include "SkMutex.h"
#include "SkStream.h"
#include "SkTScopedComPtr.h"

#include <dwrite.h>

/**
 *  An SkStream backed by an IDWriteFontFileStream.
 *  This allows Skia code to read an IDWriteFontFileStream.
 */
class SkDWriteFontFileStream : public SkStreamMemory {
public:
    explicit SkDWriteFontFileStream(IDWriteFontFileStream* fontFileStream);
    virtual ~SkDWriteFontFileStream();

    size_t read(void* buffer, size_t size) SK_OVERRIDE;
    bool isAtEnd() const SK_OVERRIDE;
    bool rewind() SK_OVERRIDE;
    SkDWriteFontFileStream* duplicate() const SK_OVERRIDE;
    size_t getPosition() const SK_OVERRIDE;
    bool seek(size_t position) SK_OVERRIDE;
    bool move(long offset) SK_OVERRIDE;
    SkDWriteFontFileStream* fork() const SK_OVERRIDE;
    size_t getLength() const SK_OVERRIDE;
    const void* getMemoryBase() SK_OVERRIDE;

private:
    SkTScopedComPtr<IDWriteFontFileStream> fFontFileStream;
    size_t fPos;
    const void* fLockedMemory;
    void* fFragmentLock;
};

/**
 *  An IDWriteFontFileStream backed by an SkStream.
 *  This allows DirectWrite to read an SkStream.
 */
class SkDWriteFontFileStreamWrapper : public IDWriteFontFileStream {
public:
    // IUnknown methods
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

    // IDWriteFontFileStream methods
    virtual HRESULT STDMETHODCALLTYPE ReadFileFragment(
        void const** fragmentStart,
        UINT64 fileOffset,
        UINT64 fragmentSize,
        void** fragmentContext);

    virtual void STDMETHODCALLTYPE ReleaseFileFragment(void* fragmentContext);
    virtual HRESULT STDMETHODCALLTYPE GetFileSize(UINT64* fileSize);
    virtual HRESULT STDMETHODCALLTYPE GetLastWriteTime(UINT64* lastWriteTime);

    static HRESULT Create(SkStream* stream, SkDWriteFontFileStreamWrapper** streamFontFileStream);

private:
    explicit SkDWriteFontFileStreamWrapper(SkStream* stream);
    virtual ~SkDWriteFontFileStreamWrapper() { }

    ULONG fRefCount;
    SkAutoTDelete<SkStream> fStream;
    SkMutex fStreamMutex;
};
#endif
