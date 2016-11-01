/* $Id: file-open.c,v 1.4 2004/07/18 11:37:55 68user Exp $ */

#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(){
    int fd;       /* ファイルディスクリプタ */


    /* ファイルをオープン。オープンできなかったらエラー */
    fd = open("data.txt", O_RDONLY);
    if ( fd < 0 ){
        char err_message[] = "ファイルをオープンできません。";
        write(2, err_message, strlen(err_message));
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n", 1);
        return 1;
    } else {
        char message[256];
        sprintf(message,
                "ファイル data をオープンしました。ファイルディスクリプタは %d です。\n",
                 fd);
        write(1, message, strlen(message));
    }

    /* 10 バイト単位でファイルから読み込み、標準出力に書き出す */
    while (1){
      char buf[10];
        int read_size;

        read_size = read(fd, buf, sizeof(buf));

        if ( read_size > 0 ){
            write(1, buf, read_size);
        } else if ( read_size == 0 ){
            break;
        } else {
            char err_message[] = "read(2) でエラーが発生しました。";
            write(2, err_message, strlen(err_message));
            write(2, strerror(errno), strlen(strerror(errno)));
            write(2, "\n", 1);
            return 1;
        }
    }
    close(fd);
    return 0;
}
