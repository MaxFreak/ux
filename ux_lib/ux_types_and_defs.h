//
// Created by Bernd Lappas on 29.11.15.
//

#ifndef UX_UX_TYPES_AND_DEFS_H
#define UX_UX_TYPES_AND_DEFS_H

#include <string>
#include <iostream>
#include "ux_exception.h"

namespace ux
{
    typedef char ux_char;
    typedef unsigned char ux_uchar;
    typedef int ux_int;
    typedef unsigned int ux_uint;
    typedef unsigned char ux_byte;
    typedef unsigned char ux_ubyte;
    typedef float ux_value;
    typedef float ux_float;
    typedef double ux_double;
    typedef std::string ux_string;
    typedef bool ux_bool;
}

using ux::ux_char;
using ux::ux_uchar;
using ux::ux_int;
using ux::ux_uint;
using ux::ux_byte;
using ux::ux_ubyte;
using ux::ux_value;
using ux::ux_float;
using ux::ux_double;
using ux::ux_string;
using ux::ux_bool;

#define UX_TRACE_DEBUG(Message) (std::cout << "DEBUG: " << Message << "\n")
#define UX_TRACE_WARNING(Message) (std::cout << "WARNING: " << Message << "\n")
#define UX_TRACE_ERROR(Message) (std::cout << "ERROR: " << Message << " File: " << (__FILE__) << " Line: " << (__LINE__) << "\n")
#define UX_THROW_EXCEPTION(Exception, ErrorMessage) {std::cout << "ERROR: " << ErrorMessage << " File: " << (__FILE__) << " Line: " << (__LINE__) << "\n"; throw Exception;}
//#define UX_THROW_EXCEPTION(Exception, ErrorMessage)

#include <mutex>
#include <condition_variable>

namespace ux
{

    class semaphore
    {
    public:
        semaphore(int count = 0) : m_count(count) { }

        inline void notify()
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            m_count++;
            m_cv.notify_one();
        }

        inline void wait()
        {
            std::unique_lock<std::mutex> lock(m_mtx);

            while (m_count == 0)
            {
                m_cv.wait(lock);
            }
            m_count--;
        }

    private:
        std::mutex m_mtx;
        std::condition_variable m_cv;
        int m_count;
    };

    class id
    {
        ux_string m_id;

    public:
        // constructor
        id(const ux_string &id = "") : m_id(id)
        {
        }

        // copy constructor
        id(const id &org) : m_id(org.m_id)
        {
        }

        // copy assignment operator
        id &operator=(id org)
        {
            swap(*this, org);
            return *this;
        }

        // move constructor
        id(id &&temp) : id()
        {
            swap(*this, temp);
        }

        friend void swap(id &first, id &second) noexcept
        {
            using std::swap;
            swap(first.m_id, second.m_id);
        }
    };

////---------------------------------------------------------------
//// eC_Time64 is a 64Bit unsigned integer base class
//// Only used for time profiling
//
//    class eC_Time64
//    {
//
//    public:
//
//        // default constructor
//        inline eC_Time64() { }
//
//        // copy constructor
//        inline eC_Time64(const eC_Time64 &rkTime) { Set(rkTime.m_uiHigh, rkTime.m_uiLow); }
//
//        // 32bit constructor
//        inline eC_Time64(ux_uint uiLow) { Set(0, uiLow); }
//
//        // 64bit constructor
//        inline eC_Time64(ux_uint uiHigh, ux_uint uiLow) { Set(uiHigh, uiLow); }
//
//        // comparison
//        inline bool operator==(const eC_Time64 &rkTime) const;
//
//        inline bool operator!=(const eC_Time64 &rkTime) const;
//
//        // arithmetics
//        inline eC_Time64 operator-(const eC_Time64 &rkTime) const;
//
//        inline eC_Time64 operator+(const eC_Time64 &rkTime) const;
//
//        // access
//        inline void Set(ux_uint uiHigh, ux_uint uiLow);
//
//        // conversion
//        inline double toDouble() const;
//
//        // data
//        ux_uint m_uiHigh;
//        ux_uint m_uiLow;
//    };
//
////----------------------------------------------------------------------------
//    inline void eC_Time64::Set(ux_uint uiHigh, ux_uint uiLow)
//    {
//        m_uiHigh = uiHigh;
//        m_uiLow = uiLow;
//    }
//
////----------------------------------------------------------------------------
//    inline double eC_Time64::toDouble() const
//    {
//        double ret;
//
//        ret = (double) m_uiHigh;
////    ret= ret * (double)(65536.0) * (double)(65536.0)+ m_uiLow;
//
//        ret = ret * ((double) (0xffffffff) + 1) + m_uiLow;
////ret = (double)((long long)m_uiHigh << 32) + m_uiLow
//        return ret;
//    }
//
////----------------------------------------------------------------------------
//    inline bool eC_Time64::operator==(const eC_Time64 &rkTime) const
//    {
//        return (m_uiHigh == rkTime.m_uiHigh) && (m_uiLow == rkTime.m_uiLow);
//    }
//
////----------------------------------------------------------------------------
//    inline bool eC_Time64::operator!=(const eC_Time64 &rkTime) const
//    {
//        return (m_uiHigh != rkTime.m_uiHigh) || (m_uiLow != rkTime.m_uiLow);
//    }
//
////----------------------------------------------------------------------------
//    inline eC_Time64 eC_Time64::operator-(const eC_Time64 &rkTime) const
//    {
//        ux_uint uiCarry = 0;
//        eC_Time64 kRet;
//
//        kRet.m_uiLow = m_uiLow - rkTime.m_uiLow;
//
//        if (kRet.m_uiLow > m_uiLow)
//            uiCarry = 1;
//
//        kRet.m_uiHigh = m_uiHigh - rkTime.m_uiHigh - uiCarry;
//
//        return kRet;
//    }
//
////----------------------------------------------------------------------------
//    inline eC_Time64 eC_Time64::operator+(const eC_Time64 &rkTime) const
//    {
//        ux_uint uiCarry = 0;
//        eC_Time64 kRet;
//
//        kRet.m_uiLow = m_uiLow + rkTime.m_uiLow;
//
//        if (kRet.m_uiLow < m_uiLow)
//            uiCarry = 1;
//
//        kRet.m_uiHigh = m_uiHigh + rkTime.m_uiHigh + uiCarry;
//
//        return kRet;
//    }
} // namespace ux

#endif //UX_UX_TYPES_AND_DEFS_H
