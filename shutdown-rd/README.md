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

## 動作確認

サーバー側はtcp-server-client/serverを使って

```
./server -b 4k -s 1000000
```

と起動し、1秒に1回、4kBのデータを送るようにセットした。
tcpdumpを実行し、別端末で

```
./shutdown 127.0.0.1 1234
```

を実行する。

- クライアント側はconnect()したらsetsockopt()でSO_RCVBUFを
64kBにセットし、linuxの自動調節機能を無効化しておく（Linuxでは
64kBでsetsockopt()すると実際には2倍の128kBがセットされる）。
- 10回、read()でデータを読んだあと3秒スリープしデータをソケット
レシーブバッファにためておく。
- ioctl(socfd, FINOREAD, &nbytes)でソケットレシーブバッファにたまっている
データ数を確認する。
- 続けてshutdown(sockfd, SHUT_RD)を実行し、直後にソケットレシーブ
バッファにたまっているデータ数を確認する。
- ここでキー入力待ちになる。サーバーからのデータにTCPがふだんどおり
にやってくるのと普通にackを返しているのを確認する。
- しばらくするとソケットレシーブバッファがいっぱいになるので
キー入力するとプログラムは再び走り出し、ソケットレシーブバッファの
データ数を取得、表示したあとclose()を実行しプログラムは終了する。

## log

(linux.txt)[linux.txt] Linuxでのログ。

(openbsd.txt)[openbsd.txt] OpenBSDでのログ。

## 結果

OpenBSDではUnix Network Programmingに書かれているように
shutdown(, SHUT_RD)で
ソケットレシーブバッファのデータがクリアされている（正確には
ioctl(FIONREAD)で取得できる値が0になっている）。

一方Linuxではshutdown(, SHUT_RD)でソケットレシーブバッファは
クリアされていないようだ。

どちらもshutdown(, SHUT_RD)後もサーバ―からのデータは普段どおりに
受信し、ackもふつうに返している。
受信したデータはOpenBSDもLinuxもソケットレシーブバッファにいれている
ようだが、OpenBSDではioctl(FIONREAD)は0を返している。
