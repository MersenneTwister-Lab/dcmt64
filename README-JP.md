# dcmt64
Dynamic Creator of 64bit Mersenne Twister
-----------------------------------------

```
./dcmt64 -m mexp -I id [-s seed] [-v] [-c count] [-f outputfile]
         [-l logfile] [-S start_seq] [-C log_count] [-M max_defect]

--mexp, -m mexp      mersenne exponent.
--verbose, -v        Verbose mode. Output parameters, calculation time, etc.
--file, -f filename  Parameters are outputted to this file. without this
                     option, parameters are outputted to standard output.
--count, -c count    Output count. The number of parameters to be outputted.
--id, -I id          start id. The first id.
--start-seq, -S seq  start seq. seq will be count ***down***.
--seed, -s seed      seed of randomness.
--fixed-pos          fix the parameter pos to given value.
--log-count count    log output interval.
--max-defect max     total dimensiton defect larger than max will be skipped.
```

説明
---

+ --mexp  メルセンヌ素数 19937 など 必須項目だが --mexp で指定すること。
+ --id   いわゆる id, 0も指定可能
+ --seed 擬似乱数の種。fixedPOS を指定しなければ使わないはず。
+ --verbose verbose mode 引数なし。指定してもあまり変わらないような気がする。
+ --count この個数だけパラメータを出力したら終了する。
+ --file  パラメータを出力するファイル名。指定されないと、標準出力にパラメータが出力されれる。
+ --logfile ログを出力するファイル名。指定されないとパラメータを出力するところにログも出力される。
+ --start-seq 再開用seq, seqはカウントダウンされるので、log に出力されている最後のseq - 1
  を指定するとよいはず。
+ --log-count どのくらいの頻度でlogを出力するか。デフォルトは mexp / 2 となっている。
+ --fixed-pos パラメータのPOSを指定された値にする。これを指定すると発見率が低くなるかも知れないし、
  高くなるかも知れないし、変わらないかも知れない。
+ --max-defect 均等分布次元の悪いものは出力しない。

これらのコマンドライン引数は options.h の中で定義されている構造体に入っているので、MPI利用時などは
構造体のデータを直接変更して実行して下さい。

この探索では均等分布次元のあまりよくないものも出力されます。とりあえず出力させて後から選別しても
構いませんが、--max-defect で指定してスキップさせてもよいと思います。
ちなみに MT19937-64 の defect は 7820 なので8000程度を指定すればよいかと思います。
大きいほど性質が悪いということになります。

fileのopen などは MPI を意識していないので、そのままMPIで使用するとまずいかも知れない。
dcmt64mpi.cpp で少し書いてみたが変更して下さい。