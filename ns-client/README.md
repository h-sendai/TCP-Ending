# ns-client (なにもしないクライアント)

connect後10秒まってclose()するクライアント。
Linuxではソケットレシーブバッファにデータがある状態で
close()するとTCP RSTが送出されるのを検証するためのプログラム。

## 使い方

サーバーはncを使う。
端末上で

```
nc -l 1234
```

別端末で

```
./nc-client 127.0.0.1 1234
```

として放置。
nc端末で適当に入力してエンターキーを押すとデータが
送られる。

10秒後にnc-clientがclose()を実行する。close()時に
ncサーバーから送られたデータがソケットレシーブバッファに
あり、それをread()していないのでnc-client側からTCP RSTが
送られる。

パケットキャプチャの図:

```
0.000000 0.000000 IP 127.0.0.1.54854 > 127.0.0.1.1234: Flags [S], seq 4002075201, win 43690, options [mss 65495,sackOK,TS val 3761904582 ecr 0,nop,wscale 7], length 0
0.000037 0.000037 IP 127.0.0.1.1234 > 127.0.0.1.54854: Flags [S.], seq 2369574148, ack 4002075202, win 43690, options [mss 65495,sackOK,TS val 3761904582 ecr 3761904582,nop,wscale 7], length 0
0.000057 0.000020 IP 127.0.0.1.54854 > 127.0.0.1.1234: Flags [.], ack 1, win 342, options [nop,nop,TS val 3761904582 ecr 3761904582], length 0
2.604197 2.604140 IP 127.0.0.1.1234 > 127.0.0.1.54854: Flags [P.], seq 1:7, ack 1, win 342, options [nop,nop,TS val 3761907186 ecr 3761904582], length 6
2.604209 0.000012 IP 127.0.0.1.54854 > 127.0.0.1.1234: Flags [.], ack 7, win 342, options [nop,nop,TS val 3761907186 ecr 3761907186], length 0
10.000249 7.396040 IP 127.0.0.1.54854 > 127.0.0.1.1234: Flags [R.], seq 1, ack 7, win 342, options [nop,nop,TS val 3761914582 ecr 3761907186], length 0
```
