
#ifndef __IKCP_EXPORT_H__
#define __IKCP_EXPORT_H__

#pragma once

#ifndef KCP_EXPORT
    #ifdef KCP_LIB
        #ifdef _WIN32
            #define KCP_EXPORT _declspec(dllexport) extern 
        #else
            #define KCP_EXPORT __attribute__((__visibility__("default"))) extern  // default 是默认导出符号（linux下）
        #endif
    #else
        #ifdef _WIN32
            #define KCP_EXPORT _declspec(dllimport) extern 

        #else
            #define KCP_EXPORT  __attribute__((__visibility__("default"))) extern  // default 是默认导出符号（linux下）
        #endif
    #endif
#endif

#endif
