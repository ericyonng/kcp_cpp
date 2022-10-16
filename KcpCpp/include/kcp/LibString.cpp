#include "pch.h"
#include "kcp/LibString.h"

KCP_CPP_BEGIN

const Byte8 * LibString::endl = "\r\n";

// 默认需要剔除的符号
const std::string LibString::_defStripChars = DEF_STRIP_CHARS;

KCP_CPP_END
