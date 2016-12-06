# DrawLine

-----

## is何

* 色々線を引けるプラグインです。シェイプレイヤーだとエクスプレッション使えなくて好きな線が引けない、と言う時などにどうぞ

## Requirements

* After Effects CC 2017
* SSE4.1をサポートしたCPU

## Install

* Plug-insディレクトリにコピーします

## Uninstall

* Plug-insディレクトリから削除します

## Parameters

### Position

* 線の先端の位置

### Width

* 線の太さ

### Color

* 線の色

### Subdivision

* フレーム間の分割数
    * エフェクト適用後、1フレームインジケータを進めた後にPositionに次のようなエクスプレッションを適用するとわかりやすいです
```
    var rad = Math.PI * time / thisComp.frameDuration;
    [Math.cos(rad) * thisLayer.width, Math.sin(rad) * thisLayer.height] * 0.25 + [thisLayer.width, thisLayer.height] * 0.5;
```

### ReferenceTime

* 何秒前までの値を参照するか

## License

MIT