#!/bin/bash
# Author: Xiakun Lu

COLOR_RED=$(    echo -e "\033[31;1m" )
COLOR_GREEN=$(  echo -e "\033[32;1m" )
COLOR_RESET=$(  echo -e "\033[0m"    )

prog=$1

rm -fr zz_*

doTest() {
    src=$1
    dst=$2
    expe=$3

    echo "./${prog} ${src} ${dst}"
    ./${prog} ${src} ${dst}
    if [ $? -ne 0 ]; then
        [ $expe != "" ] && echo "${COLOR_GREEN}pass${COLOR_RESET}" || echo "${COLOR_RED}no${COLOR_RESET}"
        echo ""
        return 1;
    fi

    if [ -d $dst ]; then
        dst=${dst}/"`basename $src`"
    fi
    if [ ! -f $src -o ! -f $dst ]; then
        echo "${COLOR_RED}no${COLOR_RESET}"
    else
        md5_s="a"
        md5_d="b"
        md5_s=`md5sum $src | cut -d' ' -f1`
        md5_d=`md5sum $dst | cut -d' ' -f1`

        [ "$md5_s" == "$md5_d" ] && echo "${COLOR_GREEN}pass${COLOR_RESET}" || echo "${COLOR_RED}no${COLOR_RESET}"
        echo ""
    fi
}

s=""
d=""

# $prog file1 file2
s=${prog}.c
d=zz_regular
doTest $s $d

# $prog file1 dir
dst_dir=zz_dir
mkdir -p $dst_dir
s=${prog}.c
d=${dst_dir}
doTest $s $d

# $prog file1 dir/file2
dst_dir=zz_dir2
mkdir -p $dst_dir
s=${prog}.c
d=${dst_dir}/zz_regular
doTest $s $d

# $prog file1 dir/subdir/subsubdir/file2
dst_dir=zz_dir3/subdir/subdirdir
mkdir -p ${dst_dir}
s=${prog}.c
d=${dst_dir}/zz_regular
doTest $s $d

# $prog dir/file1 dir/subdir/subsubdir/file2
src_dir=zz_dir_src
dst_dir=zz_dir4/subdir/subdirdir
mkdir -p ${src_dir}
mkdir -p ${dst_dir}
cp ${prog}.c ${src_dir}
s=${src_dir}/${prog}.c
d=${dst_dir}/zz_regular
doTest $s $d

s=none_exist
d=zz_regular
doTest $s $d 1

s=${prog}.c
d=none_exist_dir/
doTest $s $d 1

s=${prog}.c
d=none_exist_dir/none_exist_dir/
doTest $s $d 1

#mkfifo zz_fifo
#s=zz_fifo
#d=file_by_fifo
#doTest $s $d

mkfifo zz_fifo
src=zz_fifo
dst=zz_file_by_fifo
echo "./${prog} $src $dst &"
./${prog} $src $dst &
cat ${prog}.c > $src
md5_s=`md5sum ${prog}.c | cut -d' ' -f1`
md5_d=`md5sum $dst | cut -d' ' -f1`
[ "$md5_s" == "$md5_d" ] && echo "${COLOR_GREEN}pass${COLOR_RESET}" || echo "${COLOR_RED}no${COLOR_RESET}"
echo ""

ln -s ${prog}.c zz_link.c
s=zz_link.c
d=zz_link
doTest $s $d