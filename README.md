# AYU\_Poc

## これは何？

伺かのAYUプロトコルのProof of Conceptです。
現状ninix-kagariのみの対応です。

伺かのシェルの描画処理を肩代わりするものになっています。

## Requirements

- OpenGL4.1に対応したPC
- C++20対応のコンパイラ
- GLFW3
- glm
- ゴースト「[AYU\_sample](https://github.com/Tatakinov/ayu_sample)」

## 使い方

あらかじめAYU\_sampleをインストールしておきます。(起動はしない)

`make`して実行ファイル`ayu_poc.exe`を作り、適当なディレクトリ`/path/to/ayu`に放り込みます。

ninixを次の様に呼び出します。

```
AYU_PATH="/path/to/ayu" ninix --ghost ayu_poc --exit-if-not-found
```

## 出来ること

- サーフェスの移動(に伴うバルーンの移動)
- OnMouseClick/OnMouseDoubleClick

## 注意

1. 作りが雑
2. OpenGL周りがさっぱり分かってない人の書いたコード
3. マルチスレッドのこともよく分かってない

仕様以外の部分はあまり参考にしないように。

## 参考にしたもの

[GLFWによるOpenGL入門](https://tokoik.github.io/GLFWdraft.pdf)
[OpenGLで立方体を作る](https://zenn.dev/k41531/articles/9898945646e1f0)
