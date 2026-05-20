##################################################
# 脚本具备功能:
# 1> 编译自动拷贝至对应目录(NFS/U盘)
# 2> 根据版本号快速打包备份
# 3> 实现不修改 Makefile文件而改变编译时的附加功能
##################################################

#!/bin/bash


# <SYSTEM_VERSION 所在文件>
info_file=layout/layout_define.h

# <定义相关路径>
project_path=$(cd $(dirname $0);pwd -P)
project_name="${project_path##*/}"
nfs_path=/home/xiaole/nfs
usb_file_path=/mnt/hgfs/share

# <编译生成的目标文件>


bin=$project_path/build/CDV1004QT.BIN


day=$(date +%y%m%d-%H%M)
# <打包文件名前缀>
tarPrefix="MEIO_${project_name}_${day}"


# 版本信息所在行数
ver_info_line=""

# 获取版本号   用 -F '["]' 指定 " 为分割符  用 -v 进行变量赋值
version=""

tarName=""


####################################################
# 将.BIN 拷贝至 NFS 文件夹  将 OS  拷贝至 USB 文件夹 #
####################################################
function copy() {

    target=$(ls $project_path | grep '^CDV' | grep 'OS$')
    if [ ! -e $target ]; then
    {
        echo -e "\033[1;31m$target not found""\033[0m"
        exit 0;
    }
    fi

    if [ ! -e $bin ]; then
    {
        echo -e "\033[1;31m$bin not found" "\033[0m"
        exit 0;
    }
    fi

    sudo cp -v $bin $nfs_path

    # if test -e $usb_file_path; then
    #     rm -rf -v ${usb_file_path}/*OS
    #     cp -v $target ${usb_file_path}
    # else
    #     echo -e "\033[33;1m""tips: ${usb_file_path} not exists,copy file to usb fail...\033[0m"
    # fi

    # echo -e "copy file done \033[0m"
}

#######################################################################################
# 根据define.h文件里的 #define SYSTEM_VERSION "v0.1.01_220907_dev" 版本号,进行打包备份  #
#######################################################################################
function backup() {
    # 清理
    # make clean

    # 打包在上层路径
    cd ..


    # 打包,生成 v0.1.01_220907_dev.tar.gz
    tar -zcvf $tarName.tar.gz $project_name
    echo "================ <BACKUP DONE> ================"
	pwd
	ls -lh | grep $tarName |awk '{for(i=1;i<5;i++){$i=""}; {print $0}}'
}


function getInfo(){

    if [ ! -e $info_file ]; then
    {
        echo -e "\033[1;31m$info_file not found"
        exit 0;
    }
    fi
    


    ver_info_line=$(sed -n '/SYSTEM_VERSION/=' $info_file)
    version=$(awk -F '["]' -v line="$ver_info_line" 'NR==line {print $2}' $info_file)
    tarName="${tarPrefix}_${version}"

}


function main() {

    
    # 提前删除文件,因为有时候在make里删除失败
    rm -rf SRA_704MAHT.BIN

    if [ "$option" == "-all" ]; then
        echo "make"
        make

        # 查看编译是否成功
        if [ $? == 0 ]; then
            copy
        fi
        date

        exit 0
    elif [ "$option" == "-ui" ]; then
        echo "make ui"
        make ui

        # 查看编译是否成功
        # if [ $? == 0 ]; then
        #     copy
        # fi
        # sudo cp -v res/rom.bin $nfs_path

        date
        exit 0
    elif [ "$option" == "-cp" ]; then
        copy

        date
        exit 0

    elif [ "$option" == "-bk" ]; then
        echo "backup"
        backup
        exit 0
    else
        echo " ************** <menu> ************** "
        echo "  make all    ./autoBuild.sh -all"
        echo "  make ui     ./autoBuild.sh -ui"
        echo "  copy file   ./autoBuild.sh -cp"
        echo "  backup      ./autoBuild.sh -bk"
        echo " ************************************ "

        exit 0
    fi

}

# $1 为 shell脚本传入的第一个参数
option=$1

getInfo
main


