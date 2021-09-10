# graceful-server-client

接続するとどんどんデータを送るサーバーとクライアントのプログラム。

## プロトコル

- サーバー側起動
- クライアント側が接続を行う（アクティブオープン）
- サーバー側は接続されたらデータを送り始める
- クライアント側からデータ送信終了リクエストをTCP FINを使ってサーバーに伝達する
- サーバー側はデータ送信終了リクエストを受信したらデータ送信をやめ、その後
TCP FINを送りデータ送信が終了したことをクライアントに伝達する
- クライアント側は残りデータを読んで終了する。

## 終了方法

終了はクライアント側から行う。終了合図としてshutdown(SHUT_WR)でサーバーに
TCP FINを送る。
TCPは全二重なので、サーバー側にTCP FINがきたからといってサーバーがデータ送信を
停止する義務はない。FINを受信したら停止するという協約（プロトコル）が必要である。

アプリケーションでTCP FINが送られてきたことを検出するためにはread()して0が返って
くるのを利用する必要がある（これ以外にはないような気がする）。

ということでサーバー側ソケットは読み書き両方を行う必要がある。ふつうよくやるように
read()を実行するとデータがきていない場合（あるいはTCP FINが来ていない場合）は
データがくるまで永遠にブロックするのでread()するときにはノンブロックに設定するか
select()、epoll()などを使って多重IOする必要がある。ここではselect()を使っている。

サーバー側ではselect()を使ってソケットが読みだせるようになったら読み出し、
戻り値が0ならshutdown(SHUT_WR)を使ってTCP FINを送出している。

クライアント側ではshutdown(SHUT_WR)でTCP FINを送出後、すでにサーバー側から
送られてきているデータがソケットレシーブバッファにたまっているはずなので
それを読んでソケットレシーブバッファを空にしている。
OpenBSDのようにshutdown(SHUT_RD)でソケットレシーブバッファが空になれば
いいのだがLinuxではそうはなっていないので、read()が0を返すまでくりかえし
データを読んでいる。サーバー側は終了前にshutdown(SHUT_WR)しているので
くりかえしread()すればそのうち0が返ってくる。