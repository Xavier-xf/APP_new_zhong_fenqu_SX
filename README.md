## 2025-01-14

### 美鸥松下版本软件编译：
1. 美鸥WiFi版本共用一个内核SDK
2. 运行autoBuild.sh脚本，可以进行应用层的编译
3. 在build文件中生成应用层的.BIN文件CDV1004QT.BIN
4. 其他的配置文件放置在res文件中
5. 如果是uboot升级版本：将第3、4点的文件放在内核SDK的\rootfs\resource\app中，然后编译
6. 如果是旧内核版本：在进行第5时将在应用层的upgrade/文件下生成CDV1004QTOS，如果需要升级其他的分区，请将分区映射文件放到upgrade/platform下，然后编译应用层


