#!/bin/sh
#在应用层只需修改app分区即可

upgrade_bin_version=$(date +"%Y%m%d%H%M%S")
export upgrade_bin_version

#创建一个用于保存升级文件的目录
create_platform()
{
	if [ -d "platform" ]; then
                echo "remove platform dir"
                # rm -rf "platform"
                # mkdir "platform"
        else
                echo "mkdir platform"
                mkdir "platform"
        fi
}

#用mksquashfs工具将app目录打包成 app.sqsh4 文件系统
make_squashfs_images()
{
        ./tools/mksquashfs ./app platform/app.sqsh4 -noappend -comp xz
}

#把 升级脚本 和 进度条显示程序 也放进去打包压缩
images_compress()
{
        cp scripts/update.sh                    platform/
        cp upgrade_progress/upgrade_progress    platform/

        rm -rf CDV1004QTOS
        cd platform/
        tar -zcvf CDV1004QTOS *
        mv CDV1004QTOS ../
        cd ../
}


create_platform

make_squashfs_images

# ./partition_image.sh

images_compress


