# shutdown(sockfd, SHUT_WR)

Stevens, Fenner, Rudoffのネットワークプログラミングの教科書
UNIX Network Programming にはshutdown(sockfd, SHUT_WR)について
以下のように書いてある： Section 6.7

> SHUT_WR

> The write half of the connection is closed -- In the case of
> TCP, this is called a half-close (Section 18.5 of TCPv1).
> Any data currently in the socket send buffer will be sent,
> followed by TCP's normal connection termination sequence.
> As we mentioned earlier, this closing of the write half
> is done regardless of whether or not the socket descriptor's
> reference count is corrently greater than 0.  The process
> can no longer issue any of the write functions on the socket.

# shutdown(sockfd, SHUT_WR)で起こること

shutdown(sockfd, SHUT_WR)を実行すると相手にFINが送られる。
アプリケーション側で相手からFINが送られてきたかどうか確認するには
read()が0を返すのを見る必要がある。
