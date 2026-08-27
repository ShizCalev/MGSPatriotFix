// ReSharper disable CppClangTidyClangDiagnosticUniqueObjectDuplication
#pragma once


namespace MGS4_LinkVarBuf
{
    inline uintptr_t* linkvarbuf = nullptr;

    template <typename T, uintptr_t Offset>
    struct LinkVarValue
    {
        operator T& () const
        {
            return *reinterpret_cast<T*>(*linkvarbuf + Offset);
        }

        T& get() const
        {
            return *reinterpret_cast<T*>(*linkvarbuf + Offset);
        }

        LinkVarValue& operator=(const T value)
        {
            get() = value;
            return *this;
        }
    };

    template <typename T, uintptr_t Offset>
    struct LinkVarPointer
    {
        operator T* () const
        {
            return reinterpret_cast<T*>(*linkvarbuf + Offset);
        }

        T* get() const
        {
            return reinterpret_cast<T*>(*linkvarbuf + Offset);
        }

        T& operator[](const size_t index) const
        {
            return get()[index];
        }
    };




}

