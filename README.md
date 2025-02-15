# kirikiroid2-miyoo-a30
[WIP] kirikiroid2 MIYOO A30, TRIMUI SMART PRO, TRIMUI BRICK port for code hacking and kirikiri2 gbk non-commercial unencrypted games

## Status  
* It can run a simple GBK kirikiri game, but dpad arrow key is not available, I make the up and down key to emulate TAB key  
* Dpad up and down button (actually TAB key) have no visual feedback when in start main menu  
* Currently only tested for Trimui Smart Pro. Trimui brick and Miyoo A30 are not tested   
* It uses cocos2d-x design resolution instead full screen, need to be fixed  

## History  
* 2025-02-02: final version before open source  
* 2025-01-01: first study and compiling     

## References  
* https://github.com/zeas2/Kirikiroid2  
* Kirikiroid2 1.3.8   
* https://github.com/zeas2/Kirikiroid2/releases/tag/1.3.8  
* https://github.com/weimingtom/kirikiroid2lite  
* https://github.com/weimingtom/cocos2d-x-3.6-miyoo-a30  
* https://github.com/weimingtom/linnea  
* http://www.cocos2d-x.org/filedown/cocos2d-x-3.6.zip  
* http://www.cocos2d-x.org/filedown/CocosForWin-v3.10.exe  
* https://github.com/fusijie/Cocos-Resource  
* https://github.com/weimingtom/Kirikiroid2  
* https://gitee.com/weimingtom/kirikiroid2_fork  
* https://gitee.com/weimingtom/Kirikiroid2  

## Key code:  
* src/core/environ/ui/MainFileSelectorForm.cpp  
src/core/environ/ui/FileSelectorForm.cpp  
src/core/environ/ui/BaseForm.cpp  
src/core/environ/cocos2d/readme_kirikiroid2_ui.txt  
cocosstudio/kr2/cocosstudio/ui/NaviBarWithMenu2.csd  
cocosstudio/kr2/cocosstudio/ui/NaviBarWithMenu.csd  
Resources/ui/NaviBarWithMenu2.csb  
* src/core/environ/cocos2d/MainScene.cpp  
The game screen after clicking the item of MainFileSelectorForm  
* cocos/platform/desktop/CCGLViewImpl-desktop.cpp  
cocos/platform/desktop/CCGLViewImpl-desktop.h   
* cocos/ui/UIButton.cpp  
* src/core/sound/win32/WaveMixer.cpp  
Audio output  
* proj.linux/main.cpp  
* src/core/environ/cocos2d/breakpoint.txt  
(gdb) catch throw  
* src/core/environ/android/AndroidUtils.cpp  
* src/core/environ/cocos2d/AppDelegate.cpp  
glview->setDesignResolutionSize(1280, 720, ResolutionPolicy::SHOW_ALL);  
src/core/environ/cocos2d/design_resolution.txt    
* src/core/environ/cocos2d/CustomFileUtils.cpp  
src/core/environ/cocos2d/CustomFileUtils.h  
* src/core/tjs2/tjsConfig.cpp  
valist pointer bug  
* src/core/tjs2/tjsString.h  
Ptr == NULL bug, if defined(LINUX), if without this code, then gcc -O3 -g0 and run will crash  
* src/core/utils/minizip/ioapi.cpp  
Handle = open(filename, rw, 0600);  

## Build  
* For PC, xubuntu 20.04 64bit (Should support glfw3)      
```
sudo apt install libglfw3-dev libsdl2-dev libfreetype-dev  
make MIYOO=0 clean && make MIYOO=0 -j8  
```
* For Trimui Smart Pro  
```
(Edit Makefile to add toolchain path)  
make MIYOO=2 clean && make MIYOO=2 -j8  
```

## weibo record
```
kirikiroid2研究。打算先从SDL2入手研究声音播放。我猜测声音问题
（或者说运行原理）应该不会很难，
可能就是格式有区别或者存在转换问题罢了（例如采样率，码率，单声道之类）。
为什么是SDL2而不是SDL1.2呢？因为我发现它用的是SDL_OpenAudioDevice
而不是SDL_OpenAudio，
那很有可能它用的是更兼容安卓的SDL2，甚至可能直接用它播放声音都可以，
而不只是拿来转换格式或者采样率

kirikiroid2研究。我的天，我把声音功能弄出来了，代码看得我汗流浃背——
其实kirikiroid2本身已经实现了SDL2声音播放，只是它弄得很长且复杂——
难道是从哪里得到的代码，连ARM特定代码都来了，用于混音。不过跳过不
执行这些看不懂的代码（src/core/sound/ARM/wavemix_arm.c），
在windows下仍然是可以播放的，具体流程我后面再研究分析

kirikiroid2研究。我把关于声音部分的修改放到gh上：weimingtom/kirikiroid2lite，
目前的解决办法是用原版的SDL2播放（不是我想出来的，是原版就有的功能），
然后去掉一些可以省略的过程——其实我怀疑SDL2本身就有混音功能，
似乎没必要写得那么复杂，不过我不清楚具体细节。
注意kirikiroid2是半开源的，有很多问题，然后我又继续往里面改，
所以有更多的问题和bug，所以这目前没什么实用，而且我也还没有
把声音问题的解决用安卓跑通，现在只是windows版的大概能听到声音和背景音乐罢了，
还有非常多的运行期报错。可能暂时先改到这里，以后再完善安卓端的声音功能

kirikiroid2移植到Linux掌机研究。费了九牛二虎之力终于可以在PC上Linux环境
下编译和运行起来，但目前只能看到开头（PC模拟非交叉编译），
后面就跑不下去了（crash）。基本上我是照着Android版移植去改，但比较麻烦，
比移植krkrsdl2要麻烦，主要因为代码太复杂，引用了更多的库，
容易产生更多的编译错误。保守估计要几个月时间才能跑起来
（当然如果心情不好的话可能很快就能跑通）。
另外虽然我之前已经移植过cocos2d-x，
但这个可能还需要继续研究，因为我可能遗漏了一些关于cocos2d-x的问题，
这也需要更多时间

kirikiroid2移植linux掌机研究，我试试用linux掌机trimui smart pro
跑了一下，可以勉强跑起来，大概效果是这样，但看来还是有很多问题，
总之目前还只是简单修改阶段，大概距离可用还有很长的时间，
应该是没办法在过年前整出来的，预计起码要两三个月，
但至少可以跑起来，也算是很大的进度吧，但比移植krkrsdl2要困难许多，
毕竟krkrsdl2本来就有linux版的，kirikiroid2实际上连linux版都没有我
就想一下子就移植到linux掌机上，应该是会更麻烦许多

kirikiroid2移植linux掌机研究，终于可以用pc版linux跑起来，好像声音
有点问题暂时听不到。调试了半天，发现xubuntu 20下va_list居然不能用
指针传递（它本身就是指针），我晕，改了这个问题就没太大难度了，
照着Android的方法改就能简单跑起来 ​​​

kirikiroid2移植linux掌机研究，回炉重来系列，我也没办法，在掌机上应
该是不行的，而在PC上可能正常，但我把代码补回去cocos2d-x代码中，
然后自己随便导出一个csb测试加载它运行，发现在PC上无法显示按钮和
文本框控件（但文字可以显示），莫非加载csb文件（cocos studio的
导出文件格式）有什么地方改出问题了？后续可能要一点点去测试，
直到能显示出开头的界面，我没有信心能搞出来，只能走一步算一步了

kirikiroid2移植linux掌机研究。目前的想法是，我有一个GLES2版的
cocos2d-x 3.6，无法正确显示csb文件，我现在是另外重新创建一个
加载csb的PC版的cocos2d-x 3.6代码库，想办法把GLES2版的修改逐步
合并回去PC版（目前这个PC版是正常显示的），最终就能定位到哪里
出问题了——当然就算知道问题所在，要怎么改也是个问题

kirikiroid2移植linux掌机研究。现在可以显示csb文件中的按钮了，
但文字标签和按钮上的字体目前还没办法显示出来，我想想有没有办法解决。
之前显示的bug似乎也不算是bug，应该是cocos2d-x的特性，
如果要显示csb文件就必须设置设计分辨率（setDesignResolutionSize），
而设计分辨率和窗口大小无关，它类似于SDL2视口那样由引擎进行缩放转换，
可以是任意的。如果不按照cocos studio导出的代码去初始化，
就有可能显示不出控件

kirikiroid2移植linux掌机研究。显示不了控件文字的问题暂时解决了
（只考虑按钮和文本标签控件），两个bug：第一个bug是默认字体文件缺失，
原版CCDevice-linux.cpp会读取ubuntu的一个字体文件，
linux掌机上没有这个文件，我把它改成读elf旁边的ttf文件
（这也顺便解决中文显示问题）；
第二个bug是UIButton的setTitleColor方法的内部实现，
我改成setColor而不是setTextColor

kirikiroid2移植linux掌机研究。目前修改了一些和csb场景文件加载显示有关的bug，
但在linux掌机上运行kirikiroid2的显示效果依然没变化（还是不对），
为了稍微能加快点进度，我打算一边复制kirikiroid2的代码到
cocos2d-x-3.6-miyoo-a30上，一边测试，看哪里出问题，
目前比较乐观的是我只要跑通某个csb即可，因为通常一个场景对应一个csb，
只要能显示正确，我就可以重新测试kirikiroid2的代码，
看能不能解决——其实我已经能跑通linux pc ubuntu版的kirikiroid2，
只是linux掌机的运行效果不对罢了，一开始时界面就显示不对了

kirikiroid2移植linux掌机研究。现在尝试用VS2013把cocos2d-x ui
部分代码拆出来用于调试UI显示效果，主要在src\core\environ\ui的
MainFileSelectorForm.cpp，BaseForm.cpp，FileSelectorForm.cpp，
GameMainMenu.cpp，以及src\core\environ\cocos2d的MainScene.cpp。
用到的csb主要是这些（可以通过断点CSLoader::createNode第二重载方法知晓）：
MainFileSelector，NaviBarWithMenu，TableView，RecentListItem，
FileItem，GameMenu。总体来说类似于安卓，除了上述那些csb，
还有一些显示内容是不需要csb的

kirikiroid2移植linux掌机研究。我把代码中和csb文件有关的文件选择器
部分剥离出来，现在不需要kirikiroid2的代码也能编译运行了，
我的目标现在变成是要在linux掌机上跑通这个界面（用pc跑则正常显示，
只是linux掌机上不行）

kirikiroid2移植linux掌机研究。试试用纯cocos2d-x界面的代码跑，
结果依然和以前的效果一样，整个界面都是白色的（实际应该是黑色），
只能正确显示中间的条和文件管理器正中上面的按钮后和右面的菜单
图标按钮——其实和以前的效果基本上一模一样。好吧，我继续减代码——
很可能类似uibutton那样，可能是cocos2d-x代码内部的bug——
虽然编译成安卓、windows和linux/x11版是看不出来这些bug的

kirikiroid2移植linux掌机研究。我现在改成设计分辨率是csb原来的
设计分辨率数值（安卓的一般分辨率）——kirikiroid2原版是自己计算
出符合屏幕大小的设计分辨率来实现全屏，我觉得这很奇怪技巧。
我晚上试试能不能让掌机显示成这样，这已经是最简单的情况了
（不加载右侧文件管理器上面的按钮和图标，就是这样子的）

kirikiroid2移植linux掌机研究，我发现简化到只剩下一个csb和
使用cocosstudio的设计分辨率时，掌机就显示正常了，我晕，
我继续研究吧，看能不能把开始的界面搭起来（比较费劲，
但没其他办法了，只能一点点改） ​​​

kirikiroid2移植linux掌机研究。过了一两个星期了，还是没有头绪。
目前看上去像是距离目标很近，但又感觉很遥远。
我尝试只动态addChild左边栏的最近记录，显示是正常，
然后我动态addChild右边的文件管理器，不加载ListView列表，
只加载头部的导航条按钮，显示就会出问题，
整个屏就会变成白色背景，按钮是绿色。
难道是csb文件的问题么？实在没想通，下一步可能需要自己
建一个csb来测试

kirikiroid2移植linux掌机研究。我尝试自己画一个csb文件
NaviBarWithMenu2——先创建一个layer，然后在上面放置1个
基础容器（Panel）和3个按钮（Button），自己设置按钮的
3个状态的图片路径，宽高的缩放比都是100，手工设置一下
宽高W和H，然后就可以正常在掌机上显示了，不会出现白屏
问题和绿色按钮问题。我打算继续改下去，看能不能达到
期待的界面效果

kirikiroid2移植Linux掌机研究。可以看到简单的文件管理器界面了，
当然这份代码去掉了kirikiroid2相关代码，单独的纯cocos2d-x csb加载代码，
还需要合并回去。下一步，把界面装回去，
并且想办法跳过触摸操作直接进入游戏。你可能会问，为什么右边中间的字体会变窄？
因为它动态缩放了——我觉得cocos2d-x在这点上很别扭，它有两种宽度，
一种是原来的设计宽度，一种是缩放后的宽度，缩放会导致字体变形，
但设计宽度改变则不会。你可能还会问，还需要研究多久，
我觉得还要几个月甚至更长，我可能研究到大概可以确定能做出来就立刻开源，
不会等到所有问题都解决，否则可能要一年以上——说真的我没想过一定能做出来，
只是心情好了会拿来研究一下

kirikiroid2移植linux掌机研究。今天试试在PC版跳过触屏操作直接开进去游戏里面，
不难，可以这样改：在TVMainSelectorForm::show()的ListDir操作下面加上这样的代码：
TVMainScene::GetInstance()->scheduleOnce和
TVMainScene::GetInstance()->startupFrom("xp3路径")即可。这是怎么想出来的呢？
全靠日志输出，在输出xp3路径时候加gdb断点，就能看到点击xp3文件时
的整个调用栈（这个过程是同步操作），就可以知道是通过startupFrom进入游戏界面的，
然后我之前调试UI过程知道ListDir操作是文件管理器的初始化时候执行，
所以只要在这个操作后面加个延时delay操作即可（必须延时，
否则TVMainScene未完全初始化），这样就能开启TVMainScene界面，
TVMainScene相当于去掉文件管理器界面的kirikiroid2主界面
（包括日志滚动输出的界面）。如果这个启动过程失败，整个程序会退出。
另外TVMainScene似乎最开始是隐藏的，在进入程序后就会初始化，
不是在点击xp3后初始化

kirikiroid移植linux掌机研究。掌机版现在可以进入游戏了，但交互不行，
因为我没移植cocos2d-x的输入事件。但跑通这个有点曲折。
我合并了之前的ui调试代码进去kirikiroid2，然后运行，但-O3版会crash，
-O0版则不会，然后我分析了gdb调用栈，发现tjsString.h有个判断
Ptr为空的保护代码（!Ptr）会被gcc -O3优化掉，我把预编译条件
改成if 1就可以解决这个crash问题。下一步，研究如何移植cocos2d-x
输入事件（键盘鼠标手柄）

kirikiroid2移植linux掌机。现在可以在PC和掌机上简单添加SDL2手柄输入
交互功能了——因为后面还要添加SDL2音频输出，所以这里也是通过SDL2获取
输入事件。我测试过可以加上去（虽然有点问题），但游戏内的主菜单无法
实现手柄方向上下键切换——因为原版也做不到，原版只能通过鼠标——只能通
过模拟TAB键来进入菜单，代价是无法显示出当前选择的菜单（虽然能切换，
然后通过手柄A键模拟回车键开始游戏）。除了无法上下键选择菜单这个较
大的问题，另外PC版需要创建一个SDL2窗口来接收手柄事件，虽然掌机版
可以跳过窗口创建直接接收输入事件，所以这第二个问题是PC版会出现两
个窗口（但掌机版不用这么麻烦），但因为PC版本身支持glfw3键盘输入
（虽然不支持手柄），而且PC版也不是很重要的研究目标，所以这第二个
问题两个窗口还不算是很麻烦和难解决。关于我修改pollevents方法加入
手柄输入功能的修改，我是参考了yangosoft/cocos2d-x-arm-opengles
（大概对应3.10）的思路和SDL2 test示例的代码，事实上也有
其他很少的地方也尝试添加SDL2的SDL_PollEvent代码，
例如cocos2d-x/cocos2dx-webos（1.0版本）和emscripten-ports/Cocos2d
（3.0版本或之前），不过我主要还是沿用了cocos2d-x-arm-opengles
的类似思路，自己另外加代码去处理手柄。下一步，
先添加SDL2音频输出功能，最后收尾再考虑改良这两个问题

kirikiroid2移植linux掌机研究，打算这一两天内开源，
开源前的最终版了（后续还要改很多问题，估计要半年到一年），
今天把音频输出对接好（复制之前windows版的SDL2音频输出代码
wavemixer.cpp），默认data.xp3路径（原版不支持默认data.xp3），
加上miyoo a30的键位代码。开源后要注意，当前版本不能方向键，所以只能
用上下按键来模拟TAB来切换菜单，但TAB切换没有显示反馈（虽然是有效的），
然后按A（模拟回车）来进入游戏——或者简单说就是当前的版本会很难用，
要先TAB后回车，所以我也没打算打包demo测试程序，bug太多，
也没做多少掌机硬件测试，设计分辨率问题还没完全测试解决
（目前只测试使用固定设计分辨率导致无法全屏，除了宽屏掌机较好）
```


