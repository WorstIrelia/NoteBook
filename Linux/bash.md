# bash

"" 双引号保持原有内容
```bash
name=fc
var="$name hello"
echo ${var}
>fc hello
var='$name hello'
echo ${var}
>$name hello

```


```bash
cd /lib/modules/`uname -r`/kernel
cd /lib/modules/$(uname -r)/kernel
```

```bash

grep -n//标序号 -v//反向// -i//取反


```