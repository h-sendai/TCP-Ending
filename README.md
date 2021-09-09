# TCPの読み出し終了方法

読み出しを開始するのは簡単だが、きれいに終了しようとすると
いろいろ考えないといけないのでまとめておこうと思う。

[ns-client](ns-client) なにもしないクライアント。サーバーに接続して10秒間なにもせず
close()するプログラム。

[shutdown-rd](shutdown-rd) shutdown(sockfd, SHUT_RD)

[shutdown-wr](shutdown-wr) shutdown(sockfd, SHUT_WR)

[graceful-server-client](graceful-server-client) 接続するとサーバーからデータが
どんどんやってくるプログラムのサーバーとクライアント。
