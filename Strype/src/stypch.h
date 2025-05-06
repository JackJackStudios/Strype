#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <format>
#include <filesystem>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "Strype/Core/Log.h"
#include "Strype/Core/Base.h"
#include "Strype/Core/PlatformDetection.h"
#include "Strype/Core/Hash.h"
#include "Strype/Core/Version.h"

#ifdef STY_PLATFORM_WINDOWS
#include <Windows.h>
#endif
