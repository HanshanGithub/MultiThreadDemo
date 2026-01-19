# SQLCipher 4.5.5 for Windows x64

## 目录结构

```
sqlcipher-4.5.5/
├── include/
│   └── sqlite3.h              # C/C++ 头文件
├── bin/
│   ├── debug/                 # Debug 版本运行时文件
│   │   ├── sqlite3.dll        # Debug 版本动态库 (1.76 MB)
│   │   ├── sqlite3.pdb        # Debug 符号文件 (2.64 MB)
│   │   ├── libcrypto-1_1-x64.dll  # OpenSSL 加密库 (2.88 MB)
│   │   └── libssl-1_1-x64.dll     # OpenSSL SSL 库 (768 KB)
│   └── release/               # Release 版本运行时文件
│       ├── sqlite3.dll        # Release 版本动态库 (1.82 MB)
│       ├── sqlite3.pdb        # Release 符号文件 (5.07 MB)
│       ├── libcrypto-1_1-x64.dll  # OpenSSL 加密库 (2.88 MB)
│       └── libssl-1_1-x64.dll     # OpenSSL SSL 库 (768 KB)
├── lib/
│   ├── debug/                 # Debug 版本链接库
│   │   ├── sqlite3.lib        # Debug 导入库 (67 KB)
│   │   ├── sqlite3.dll        # Debug 动态库（与 bin/debug 相同）
│   │   └── sqlite3.pdb        # Debug 符号文件
│   └── release/               # Release 版本链接库
│       ├── sqlite3.lib        # Release 导入库 (67 KB)
│       ├── sqlite3.dll        # Release 动态库（与 bin/release 相同）
│       └── sqlite3.pdb        # Release 符号文件
└── README.md                  # 本文件
```

## 编译信息

- **SQLCipher 版本**: 4.5.5 (基于 SQLite 3.42.0)
- **编译器**: Visual Studio 2022 (MSVC 19.44)
- **平台**: Windows x64
- **OpenSSL 版本**: 1.1.1w (兼容 Windows 7)
- **运行时库**: 动态链接 (`/MD` 和 `/MDd`)

### 编译配置

**Release 版本**:
- `USE_CRT_DLL=1` (动态链接运行时库 `/MD`)
- `OPTIMIZATIONS=2` (O2 优化)
- 无调试信息（但包含 PDB 用于符号调试）

**Debug 版本**:
- `USE_CRT_DLL=1` (动态链接运行时库 `/MDd`)
- `DEBUG=2` (启用调试功能和符号)
- `OPTIMIZATIONS=0` (无优化，便于调试)
- 包含完整调试符号

### SQLCipher 编译选项

- `SQLITE_HAS_CODEC=1` - 启用加密功能
- `SQLITE_TEMP_STORE=2` - 临时存储设置（SQLCipher 要求）
- `SQLCIPHER_CRYPTO_OPENSSL` - 使用 OpenSSL 作为加密提供者

## 项目集成

### 1. Visual Studio 项目配置（.vcxproj）

在项目属性中配置：

**包含目录**:
- `$(SolutionDir)3rdparty\sqlcipher-4.5.5\include`

**库目录**:
- Debug: `$(SolutionDir)3rdparty\sqlcipher-4.5.5\lib\debug`
- Release: `$(SolutionDir)3rdparty\sqlcipher-4.5.5\lib\release`

**附加依赖项**:
- Debug: `sqlite3.lib`
- Release: `sqlite3.lib`

**复制 DLL 到输出目录**（在项目设置中添加生成后事件）:
```
Debug:
copy /Y "$(SolutionDir)3rdparty\sqlcipher-4.5.5\bin\debug\sqlite3.dll" "$(OutDir)"
copy /Y "$(SolutionDir)3rdparty\sqlcipher-4.5.5\bin\debug\libcrypto-1_1-x64.dll" "$(OutDir)"

Release:
copy /Y "$(SolutionDir)3rdparty\sqlcipher-4.5.5\bin\release\sqlite3.dll" "$(OutDir)"
copy /Y "$(SolutionDir)3rdparty\sqlcipher-4.5.5\bin\release\libcrypto-1_1-x64.dll" "$(OutDir)"
```

### 2. CMake 配置示例

```cmake
# 设置 SQLCipher 路径
set(SQLCIPHER_DIR "${CMAKE_SOURCE_DIR}/3rdparty/sqlcipher-4.5.5")

# 包含头文件目录
include_directories(${SQLCIPHER_DIR}/include)

# 根据构建类型选择库文件
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SQLCIPHER_LIB_DIR ${SQLCIPHER_DIR}/lib/debug)
    set(SQLCIPHER_DLL_DIR ${SQLCIPHER_DIR}/bin/debug)
else()
    set(SQLCIPHER_LIB_DIR ${SQLCIPHER_DIR}/lib/release)
    set(SQLCIPHER_DLL_DIR ${SQLCIPHER_DIR}/bin/release)
endif()

# 链接库
target_link_directories(your_target_name PRIVATE ${SQLCIPHER_LIB_DIR})
target_link_libraries(your_target_name PRIVATE sqlite3)

# 复制 DLL 到输出目录
add_custom_command(TARGET your_target_name POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SQLCIPHER_DLL_DIR}/sqlite3.dll"
        "${SQLCIPHER_DLL_DIR}/libcrypto-1_1-x64.dll"
        $<TARGET_FILE_DIR:your_target_name>
)
```

### 3. 命令行编译（MSVC）

```batch
REM Debug 版本
cl /MDd /I"3rdparty\sqlcipher-4.5.5\include" main.cpp /link /LIBPATH:"3rdparty\sqlcipher-4.5.5\lib\debug" sqlite3.lib /OUT:app_debug.exe

REM Release 版本
cl /MD /O2 /I"3rdparty\sqlcipher-4.5.5\include" main.cpp /link /LIBPATH:"3rdparty\sqlcipher-4.5.5\lib\release" sqlite3.lib /OUT:app_release.exe
```

### 4. C++ 代码示例

```cpp
#include "sqlite3.h"
#include <iostream>
#include <string>

// 简单的错误处理函数
void check_error(sqlite3 *db, int rc, const char *operation) {
    if (rc != SQLITE_OK) {
        std::cerr << operation << " 失败: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(1);
    }
}

int main() {
    sqlite3 *db = nullptr;
    int rc;
    char *errMsg = nullptr;
    
    // 打开数据库
    rc = sqlite3_open("encrypted.db", &db);
    check_error(db, rc, "打开数据库");
    std::cout << "数据库已打开" << std::endl;
    
    // 设置加密密钥（必须在打开数据库后立即执行，在任何其他操作之前）
    rc = sqlite3_exec(db, "PRAGMA key = 'your-secret-key-12345';", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "设置密钥失败: " << (errMsg ? errMsg : "未知错误") << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return 1;
    }
    std::cout << "加密密钥已设置" << std::endl;
    
    // 创建表
    const char *createTableSQL = 
        "CREATE TABLE IF NOT EXISTS users("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "email TEXT UNIQUE"
        ");";
    
    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
    check_error(db, rc, "创建表");
    std::cout << "表已创建" << std::endl;
    
    // 插入数据
    const char *insertSQL = 
        "INSERT INTO users(name, email) VALUES('张三', 'zhangsan@example.com');";
    
    rc = sqlite3_exec(db, insertSQL, nullptr, nullptr, &errMsg);
    check_error(db, rc, "插入数据");
    std::cout << "数据已插入" << std::endl;
    
    // 查询数据（使用回调函数）
    auto callback = [](void *data, int argc, char **argv, char **colName) -> int {
        for (int i = 0; i < argc; i++) {
            std::cout << colName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
        }
        std::cout << "---" << std::endl;
        return 0;
    };
    
    const char *selectSQL = "SELECT * FROM users;";
    rc = sqlite3_exec(db, selectSQL, callback, nullptr, &errMsg);
    check_error(db, rc, "查询数据");
    
    // 关闭数据库
    sqlite3_close(db);
    std::cout << "数据库已关闭" << std::endl;
    
    return 0;
}
```

### 5. 使用 sqlite3_key 函数（可选）

如果需要在打开数据库时直接设置密钥，可以使用 `sqlite3_key` 函数：

```cpp
#include "sqlite3.h"
#include <iostream>

int main() {
    sqlite3 *db = nullptr;
    int rc;
    
    // 打开数据库
    rc = sqlite3_open("encrypted.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "无法打开数据库: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }
    
    // 使用 sqlite3_key 设置密钥（必须在任何数据库操作之前调用）
    const char *key = "your-secret-key-12345";
    rc = sqlite3_key(db, key, (int)strlen(key));
    if (rc != SQLITE_OK) {
        std::cerr << "设置密钥失败: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }
    
    // 验证密钥（尝试执行一个简单查询）
    rc = sqlite3_exec(db, "SELECT COUNT(*) FROM sqlite_master;", nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "密钥验证失败（可能是错误的密钥）: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }
    
    // 执行其他数据库操作...
    
    sqlite3_close(db);
    return 0;
}
```

## 部署要求

### 运行时依赖

确保以下 DLL 与你的可执行文件在同一目录（或在系统 PATH 中）：

**Debug 模式**:
- `sqlite3.dll` (从 `bin/debug/` 复制)
- `libcrypto-1_1-x64.dll` (从 `bin/debug/` 复制)
- `libssl-1_1-x64.dll` (从 `bin/debug/` 复制，可选，仅在需要 SSL 功能时)

**Release 模式**:
- `sqlite3.dll` (从 `bin/release/` 复制)
- `libcrypto-1_1-x64.dll` (从 `bin/release/` 复制)
- `libssl-1_1-x64.dll` (从 `bin/release/` 复制，可选，仅在需要 SSL 功能时)

**重要**: SQLCipher 依赖 OpenSSL 的 `libcrypto-1_1-x64.dll`，缺少此文件将导致程序无法启动。

### Windows 7 兼容性

此版本使用 **OpenSSL 1.1.1w**，与以下系统完全兼容：
- ✅ Windows 7 SP1 (x64) 及更高版本
- ✅ Windows 8/8.1
- ✅ Windows 10/11
- ✅ Windows Server 2008 R2 及更高版本

## 版本说明

- **SQLCipher 4.5.5** - 基于 SQLite 3.42.0
- **OpenSSL 1.1.1w** - 最后一个 1.1.x 系列版本，Windows 7 兼容
- **编译器**: MSVC 19.44 (Visual Studio 2022)

## 注意事项

1. **Debug 和 Release 版本不能混用** - 必须根据项目的构建模式选择对应的库文件
2. **OpenSSL DLL 必须一起部署** - `libcrypto-1_1-x64.dll` 是必需的，`libssl-1_1-x64.dll` 仅在需要 SSL 功能时使用
3. **运行时库兼容性** - 本版本使用动态链接运行时库（`/MD` 和 `/MDd`），确保你的项目也使用相同的设置
4. **DLL 路径** - 确保 `sqlite3.dll` 和 `libcrypto-1_1-x64.dll` 在运行时可访问（通常在可执行文件同一目录）
5. **加密密钥设置时机** - `PRAGMA key` 或 `sqlite3_key()` 必须在打开数据库后、执行任何其他操作之前立即调用

## 技术支持

- SQLCipher 官方: https://www.zetetic.net/sqlcipher/
- OpenSSL: https://www.openssl.org/
