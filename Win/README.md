# DrawLine

-----

## is��

* �F�X����������v���O�C���ł��B�V�F�C�v���C���[���ƃG�N�X�v���b�V�����g���Ȃ��čD���Ȑ��������Ȃ��A�ƌ������Ȃǂɂǂ���

## Requirements

* After Effects CC 2017
* SSE4.1���T�|�[�g����CPU

## Install

* Plug-ins�f�B���N�g���ɃR�s�[���܂�

## Uninstall

* Plug-ins�f�B���N�g������폜���܂�

## Parameters

### Position

* ���̐�[�̈ʒu

### Width

* ���̑���

### Color

* ���̐F

### Subdivision

* �t���[���Ԃ̕�����
    * �G�t�F�N�g�K�p��A1�t���[���C���W�P�[�^��i�߂����Position�Ɏ��̂悤�ȃG�N�X�v���b�V������K�p����Ƃ킩��₷���ł�
```
    var rad = Math.PI * time / thisComp.frameDuration;
    [Math.cos(rad) * thisLayer.width, Math.sin(rad) * thisLayer.height] * 0.25 + [thisLayer.width, thisLayer.height] * 0.5;
```

### ReferenceTime

* ���b�O�܂ł̒l���Q�Ƃ��邩

## License

MIT