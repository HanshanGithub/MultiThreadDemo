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

## Qt 项目集成

### 1. 在 .pro 文件中配置

```qmake
# SQLCipher 路径
SQLCIPHER_DIR = $$PWD/3rdparty/sqlcipher-4.5.5

# 包含头文件
INCLUDEPATH += $$SQLCIPHER_DIR/include

# 根据构建模式选择不同的库
CONFIG(debug, debug|release) {
    # Debug 模式 - 使用 Debug 版本
    LIBS += -L$$SQLCIPHER_DIR/lib/debug -lsqlite3
    
    # 复制 Debug DLL 和 OpenSSL DLL
    QMAKE_POST_LINK += $$quote(copy /Y $$shell_path($$SQLCIPHER_DIR/bin/debug/sqlite3.dll) $$shell_path($$OUT_PWD) $$escape_expand(\\n\\t))
    QMAKE_POST_LINK += $$quote(copy /Y $$shell_path($$SQLCIPHER_DIR/bin/debug/libcrypto-1_1-x64.dll) $$shell_path($$OUT_PWD) $$escape_expand(\\n\\t))
    QMAKE_POST_LINK += $$quote(copy /Y $$shell_path($$SQLCIPHER_DIR/bin/debug/libssl-1_1-x64.dll) $$shell_path($$OUT_PWD) $$escape_expand(\\n\\t))
} else {
    # Release 模式 - 使用 Release 版本
    LIBS += -L$$SQLCIPHER_DIR/lib/release -lsqlite3
    
    # 复制 Release DLL 和 OpenSSL DLL
    QMAKE_POST_LINK += $$quote(copy /Y $$shell_path($$SQLCIPHER_DIR/bin/release/sqlite3.dll) $$shell_path($$OUT_PWD) $$escape_expand(\\n\\t))
    QMAKE_POST_LINK += $$quote(copy /Y $$shell_path($$SQLCIPHER_DIR/bin/release/libcrypto-1_1-x64.dll) $$shell_path($$OUT_PWD) $$escape_expand(\\n\\t))
    QMAKE_POST_LINK += $$quote(copy /Y $$shell_path($$SQLCIPHER_DIR/bin/release/libssl-1_1-x64.dll) $$shell_path($$OUT_PWD) $$escape_expand(\\n\\t))
}
```

### 2. C++ 代码示例

```cpp
#include "sqlite3.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    sqlite3 *db;
    int rc = sqlite3_open("encrypted.db", &db);
    
    if (rc != SQLITE_OK) {
        qDebug() << "无法打开数据库:" << sqlite3_errmsg(db);
        return 1;
    }
    
    // 设置加密密钥（必须在打开数据库后立即执行）
    rc = sqlite3_exec(db, "PRAGMA key = 'your-secret-key';", nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        qDebug() << "设置密钥失败:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        return 1;
    }
    
    // 执行 SQL 操作
    rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users(id INTEGER PRIMARY KEY, name TEXT);", 
                      nullptr, nullptr, nullptr);
    
    sqlite3_close(db);
    return 0;
}
```

## 部署要求

### 运行时依赖

确保以下 DLL 与你的可执行文件在同一目录：

**Debug 模式**:
- `sqlite3.dll` (从 `bin/debug/` 复制)
- `libcrypto-1_1-x64.dll` (从 `bin/debug/` 复制)
- `libssl-1_1-x64.dll` (从 `bin/debug/` 复制，如果使用 SSL)

**Release 模式**:
- `sqlite3.dll` (从 `bin/release/` 复制)
- `libcrypto-1_1-x64.dll` (从 `bin/release/` 复制)
- `libssl-1_1-x64.dll` (从 `bin/release/` 复制，如果使用 SSL)

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

1. **Debug 和 Release 版本不能混用** - 必须根据 Qt 项目的构建模式选择对应的库文件
2. **OpenSSL DLL 必须一起部署** - `libcrypto-1_1-x64.dll` 是必需的，`libssl-1_1-x64.dll` 仅在需要 SSL 功能时使用
3. **运行时库兼容性** - 本版本使用动态链接运行时库（`/MD` 和 `/MDd`），与 Qt 默认配置兼容

## 技术支持

- SQLCipher 官方: https://www.zetetic.net/sqlcipher/
- OpenSSL: https://www.openssl.org/
