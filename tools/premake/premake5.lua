--  @file   premake5.lua
-- #########################################################################
-- Global compile settings

-- python tool define
IS_WINDOWS = string.match(_ACTION, 'vs') ~= nil
-- All libraries output directory
OUTPUT_DIR = "../../output/" .. _ACTION
-- root directory
ROOT_DIR = "../../"
WIN_ROOT_DIR = ".\\..\\..\\"
-- build directory
BUILD_DIR = "../../build/"

-- debug dir
DEBUG_DIR = OUTPUT_DIR

-----------------------------------------------------------------------------------------------------------

-- Common functional functions define
-- Enable multithread compile
function enable_multithread_comp(cppstdver)
    filter { "system:windows" }
        flags { "MultiProcessorCompile", "NoMinimalRebuild", cppstdver }
    filter {}
end


-- 启用预编译头文件机制
function enable_precompileheader(header_file, source_file)
    filter { "system:windows" }
        pchsource(source_file or "pch.cpp")
        pchheader(header_file or "pch.h")
        buildoptions { "/Zm1000" }
    filter {}

    filter { "system:not windows" }
        pchheader(header_file or "pch.h")
    filter {}
end

-- Set optimize options.
function set_optimize_opts()

end

-- set common options
function set_common_options()
    -- rdynamic coredump符号
    filter { "language:c", "system:not windows" }
        buildoptions {
            "-DLINUX -Wall -rdynamic -fPIC -D_FILE_OFFSET_BITS=64",
        }
    filter {}
	filter { "configurations:debug*", "language:c", "system:not windows" }
        buildoptions {
            "-ggdb -g",
        }
    filter {}
	filter { "configurations:debug*", "language:c", "system:windows" }
        runtime "Debug"
        optimize "Debug"
    filter {}

    filter { "configurations:debug*", "language:not c" }
        optimize "Debug"
    filter {}

    filter { "configurations:release*" }
        optimize "Speed"
    filter {}
end

-- lib include实现
function include_libfs(do_post_build)	
    -- includedirs
    includedirs {
        ROOT_DIR .. "/KcpCpp/include/",
    }

    libdirs { 
		ROOT_DIR .. "/KcpCpp/include/"
	}

    -- libdirs(linux)
    filter { "system:linux"}
		includedirs {
        "/usr/include/",
		}
        libdirs {
            ROOT_DIR .. "/usr/lib64/",
        }
		links {
		    "rt",
            "uuid",
			"pthread",
			"crypto",
			"ssl",
            "dl",
        }
    filter {}

end

-- zlib library:
-- local ZLIB_LIB = "../../FS/3rd_party/zlib"
-- #########################################################################

workspace ("Kcp_" .. _ACTION)
    -- location define
    location (BUILD_DIR .. _ACTION)
    -- target directory define
    targetdir (OUTPUT_DIR)

    -- configurations 默认64位 不输出32位
    configurations {"debug", "release"}

    -- architecture 全部配置都生成64位程序
    filter { "configurations:*" }
        architecture "x86_64"
    filter {}

    -- defines
    filter { "configurations:debug*" }
        defines {
            "DEBUG",
			"_DEBUG",
        }
    filter {}
	
    filter { "configurations:release*" }
        defines {
            "NDEBUG"
        }
    filter {}
	
    -- control symbols
    filter { "system:macosx", "language:c++" }
        symbols("On")
    filter {}

    -- characterset architecture 多字节字符
    filter { "language:c++" }
        characterset "MBCS"
    filter {}

    -- disable some warnings
    filter { "system:windows", "language:c++" }
        disablewarnings { "4091", "4819" }
    filter {}

-- core library testsuite compile setting
project "KcpCpp"
    -- language, kind
    language "c++"
    kind "ConsoleApp"
	
    -- symbols
	debugdir(DEBUG_DIR)
    symbols "On"

	enable_precompileheader("pch.h", ROOT_DIR .. "KcpCpp/kcp_pch/pch.cpp")

	includedirs {
	    "../../",
		"../../KcpCpp/include/",
		"../../KcpCpp/",
		"../../KcpCommon/",
		"../../DevelopmentKit/",
    }
	
	-- 设置通用选项
    set_common_options()
	
    -- files
    files {
		"../../KcpCpp/**.h",
        "../../KcpCpp/**.cpp",
        "../../example/**.h",
        "../../example/**.cpp",
        "../../KcpCommon/**.h",
        "../../KcpCommon/**.cpp",
        "../../KcpCommon/**.c",
		"../../DevelopmentKit/**.h",
		"../../DevelopmentKit/**.cpp",
    }

	-- links
    libdirs { OUTPUT_DIR }	
	include_libfs(true)

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    -- enable_multithread_comp("C++14")
	enable_multithread_comp()

    -- warnings
    filter { "system:not windows" }
        disablewarnings {
            "invalid-source-encoding",
        }
    filter {}

    -- optimize
    set_optimize_opts()

    -- set post build commands.
    filter { "system:windows" }
        postbuildcommands(string.format("start %srunfirstly_scripts.bat %s", WIN_ROOT_DIR, _ACTION))
    filter {}
		


-- core library testsuite compile setting
project "KcpClient"
    -- language, kind
    language "c++"
    kind "ConsoleApp"
	
    -- symbols
	debugdir(DEBUG_DIR)
    symbols "On"

	enable_precompileheader("pch.h", ROOT_DIR .. "KcpClient/client_pch/pch.cpp")

	includedirs {
	    "../../",
		"../../KcpCpp/include/",
		"../../KcpCpp/",
		"../../KcpCommon/",
		"../../DevelopmentKit/",
		"../../KcpClient/",
    }
	
	-- 设置通用选项
    set_common_options()
	
    -- files
    files {
		"../../KcpCpp/**.h",
        "../../KcpCpp/**.cpp",
        "../../KcpCommon/**.h",
        "../../KcpCommon/**.cpp",
        "../../KcpCommon/**.c",
		"../../DevelopmentKit/**.h",
		"../../DevelopmentKit/**.cpp",
		"../../KcpClient/**.h",
		"../../KcpClient/**.cpp",
    }

	-- links
    libdirs { OUTPUT_DIR }	
	include_libfs(true)

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    -- enable_multithread_comp("C++14")
	enable_multithread_comp()

    -- warnings
    filter { "system:not windows" }
        disablewarnings {
            "invalid-source-encoding",
        }
    filter {}

    -- optimize
    set_optimize_opts()

    -- set post build commands.
    filter { "system:windows" }
        postbuildcommands(string.format("start %srunfirstly_scripts.bat %s", WIN_ROOT_DIR, _ACTION))
    filter {}


-- ###########################################################################################################

-- core library testsuite compile setting
project "KcpServer"
-- language, kind
language "c++"
kind "ConsoleApp"

-- symbols
debugdir(DEBUG_DIR)
symbols "On"

enable_precompileheader("pch.h", ROOT_DIR .. "KcpServer/server_pch/pch.cpp")

includedirs {
    "../../",
    "../../KcpCpp/include/",
    "../../KcpCpp/",
    "../../KcpCommon/",
    "../../DevelopmentKit/",
    "../../KcpServer/",
}

-- 设置通用选项
set_common_options()

-- files
files {
    "../../KcpCpp/**.h",
    "../../KcpCpp/**.cpp",
    "../../KcpCommon/**.h",
    "../../KcpCommon/**.cpp",
    "../../KcpCommon/**.c",
    "../../DevelopmentKit/**.h",
    "../../DevelopmentKit/**.cpp",
    "../../KcpServer/**.h",
    "../../KcpServer/**.cpp",
}

-- links
libdirs { OUTPUT_DIR }	
include_libfs(true)

-- debug target suffix define
filter { "configurations:debug*" }
    targetsuffix "_debug"
filter {}

-- enable multithread compile
-- enable_multithread_comp("C++14")
enable_multithread_comp()

-- warnings
filter { "system:not windows" }
    disablewarnings {
        "invalid-source-encoding",
    }
filter {}

-- optimize
set_optimize_opts()

-- set post build commands.
filter { "system:windows" }
    postbuildcommands(string.format("start %srunfirstly_scripts.bat %s", WIN_ROOT_DIR, _ACTION))
filter {}

if IS_WINDOWS == false then
	print("builddir = " .. BUILD_DIR)
end

-- ############################################################################


-- ************************ KCP LIB ***************************************

project "Kcp"
    -- language, kind
    language "c"
    kind "SharedLib"
	
    -- symbols
	debugdir(DEBUG_DIR)
    symbols "On"
    
	-- 设置通用选项
    set_common_options()
	 
    -- files
    files {
        "../../kcplib/**.h",
		"../../kcplib/**.c",
    }

	defines { "KCP_LIB" }
	
    -- target prefix 前缀
    targetprefix "lib"

    filter { "system:not windows" }
        buildoptions {
            "-fvisibility=hidden",
        }
    filter {}
	
	enable_multithread_comp()

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}
