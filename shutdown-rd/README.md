# shutdown(sockfd, SHUT_RD)

Stevens, Fenner, Rudoffのネットワークプログラミングの教科書
UNIX Network Programming にはshutdown(sockfd, SHUT_RD)について
以下のように書いてある： Section 6.7

> SHUT_RD
>
> The read half of the connection is closed -- No more data can
> be received on the socket and any data currently in the socket
> receive buffer is discarded.  The process can no longer issue
> any of the read functions on the socket.  Any data received
> after this call for a TCP socket is acknowledged and then silently
> discarded.

ということはshutdown(sockfd, SHUT_RD)を発行するとソケットレシーブバッファ
のデータは捨てられ、shutdown(SHUT_RD)後に受信したパケットについては
通常通りACKが返されるがソケットレシーブバッファには入らないという
動作になると書いてある気がする。
