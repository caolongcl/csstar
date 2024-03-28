# star

## 开发

设置环境变量，不设置的话默认是 Debug。
- 编译 debug 版本 `export DSG_BUILD_TYPE=Debug`
- 编译 release 版本 `export DSG_BUILD_TYPE=Release`

### 1. 编译第三方依赖

执行 `build_deps.sh` 编译 `deps` 中第三方库。执行成功会将编译好的库安装到 `jbcore/lib`目录下。
- `paho_mqtt_c` mqtt 库
- `zxingcpp`  二维码库

### 2. 编译 jbcore 库

> 注意：每次修改任何 CMakeLists.txt 后，如果编译失败，可以执行 `rm -rf build` 再重编。

执行 `./jbcore/script/build.sh` 编译并安装产物到 `out/jbcore`
- `bin` 测试程序
  - `sample_mqttd` 测试 mqtt 功能
  - `sample_qrcoded [二维码绝对路径]` 测试二维码解码功能
    比如 `sample_qrcoded /xxx/star/jbcore/docs/wifi.png`
- `include`
- `lib`

### 3. 编译 jbapp 可执行程序

> jbapp 就是在板子上运行的程序
> 
> 注意：每次修改任何 CMakeLists.txt 后，如果编译失败，可以执行 `rm -rf build` 再重编。

执行 `./app/script/build.sh` 编译并安装产物到 `bin`
- `jbappd` 如果 DSG_BUILD_TYPE=Debug
- `jbapp` 如果 DSG_BUILD_TYPE=Release
