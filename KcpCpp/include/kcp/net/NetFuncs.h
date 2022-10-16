#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_NET_NET_FUNCS_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_NET_NET_FUNCS_H__

#pragma once

#include "kcp/common/common.h"

#ifndef SHUT_RDWR
#define SHUT_RDWR SD_BOTH
#endif

KCP_CPP_BEGIN
    ///socket()



#ifdef _WIN32
	inline int GetErrorCode()
	{
		const auto error = ::WSAGetLastError();

		//We need to posixify the values that we are actually using inside this header.
		switch (error)
		{
			case WSAEWOULDBLOCK:
				return EWOULDBLOCK;
			case WSAEBADF:
				return EBADF;
			case WSAEINTR:
				return EINTR;
			default:
				return error;
		}
	}
#else
inline int GetErrorCode()
{
    return errno;
}

inline int closesocket(SOCKET in)
{
    return ::close(in);
}
#endif

KCP_CPP_END

#endif
