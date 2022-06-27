## 实验选题：鼠标驱动和简单的图形接口实现

参考文献:《操作系统原理、实现与实践》-李治军-哈工大



### 0x01 创建git仓库

这里记录一些常用的命令，或者说是基本命令

```shell
// 初始化创建一个git仓库
git init

// 把文件添加到仓库
git add x.file

// 提交到版本库
git commit -m "ZhuShi"

// 查看文件状态
git status

// 查看修改记录
git log --pretty=oneline

// 回退版本，上个版本是HEAD^,上上个是HEAD^^,100个之前是HEAD~100
// 也可以输入版本id号具体回退，注意倒退后再想回来就只能通过版本号了
git reset --hard HEAD^
git reset --hard 1e5d(id号)

git push origin master 推到github里
```

那么试着记录现在的状态到git里吧，然后我们就可以正式开始了

版本记录号采用0.0.0这样

修改某个文件具体代码变动第三位

修改一组文件实现某个具体功能变动第二位

修改实现新的一些功能变动第一位

开始吧！
