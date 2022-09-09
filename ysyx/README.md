# SoC集成测试Checklist
**进行SoC集成测试的所有步骤都可以在当前目录下完成**，ysyx的目录结构如下所示：
```sh
ysyxSoC/ysyx
├── README.md                      # SoC集成步骤说明
├── main.py                        # 测试主脚本
├── prog                        # 集成测试程序
├── lint
│   ├── Makefile                   # 代码规范检查脚本
│   └── warning.md                 # Verilator中Warning无法清理说明
├── module-prefix
│   ├── AddModulePrefix.scala      # 为Chisel开发的模块名添加前缀的firrtl transform
│   └── README.md                  # transform使用说明
├── perip
│   ├── spi                        # SPI控制器
│   │   ├── doc
│   │   │   └── spi.pdf            # 文档
│   │   └── rtl
│   │       ├── amba_define.v
│   │       ├── spi_clgen.v
│   │       ├── spi_defines.v
│   │       ├── spi_shift.v
│   │       ├── spi_top.v
│   │       └── spi.v              # 顶层文件(包含flash的XIP模式)
│   ├── spiFlash                   # 支持SPI模式的Flash颗粒简化模型
│   │   ├── spiFlash.cpp
│   │   └── spiFlash.v
│   └── uart16550                  # UART16550控制器
│       ├── doc
│       │   └── UART_spec.pdf      # 文档
│       └── rtl
│           ├── raminfr.v
│           ├── timescale.v
│           ├── uart_apb.v         # 顶层文件
│           ├── uart_defines.v
│           ├── uart_receiver.v
│           ├── uart_regs.v
│           ├── uart_rfifo.v
│           ├── uart_sync_flops.v
│           ├── uart_tfifo.v
│           └── uart_transmitter.v
├── ram
│   ├── README.md                  # RAM端口说明
│   ├── S011HD1P_X32Y2D128_BW.v    # 端口与流片用RAM一致的简化行为模型(带写掩码)
│   └── S011HD1P_X32Y2D128.v       # 端口与流片用RAM一致的简化行为模型(不带写掩码)
└── soc
    ├── interface.md               # CPU端口规范说明
    ├── io-check.py                # CPU命名规范自查脚本
    ├── Makefile                   # 用于将Chisel代码编译成ysyxSoCFull.v，同学无需使用
    └── ysyxSoCFull.v              # SoC的Verilog实现
```

同学们执行SoC集成的所有测试任务都可以运行该目录下的`main.py`完成，我们提供的`main.py`脚本包含有端口命名检查、代码规范检查和Verilator程序编译与仿真测试的全部功能，可以输入`./main.py -h`来获得其支持的功能列表：
```sh
$> ./main.py -h
usage: main.py [-h] [-s] [-l] [-lu] [-c] [-fc] [-t TEST TEST] [-r] [-fr] [-su] [-y]

OSCPU Season 4 SoC Test

optional arguments:
  -h, --help            show this help message and exit
  -s, --stand           run interface standard check
  -l, --lint            run code lint check
  -lu, --lint_unused    run code lint with unused check
  -c, --comp            compile core with SoC in normal flash mode
  -fc, --fst_comp       compile core with SoC in fast flash mode
  -t TEST TEST, --test TEST TEST
                        Example: ./main.py -t [flash|loader] [hello|memtest|rttread|keyboard|vga|pal]
  -r, --regress         run all test in normal flash mode
  -fr, --fst_regress    run all test in fast flash mode
  -su, --submit         submit code and spec to CICD
  -y, --ysyx            compile ysyxSoCFull framework[NOT REQUIRED]
```

具体来说，每个同学都需要按照顺序进行：

>命名规范检查 -> CPU内部修改 -> 代码规范检查 -> Verilator仿真 -> 提交代码

细心的同学可能会发现，`main.py`其实也是分别调用各个子目录下的Makefile或者python脚本来实现的。另外，该集成任务Checklist是按照**任务先后顺序**排列的，所以同学们要确保前面的任务完成后再进行下一个任务。

> 推荐用`python3`而非`python2`运行`main.py`。


## 一些准备工作
为了使用进行测试，需要：
* 将CPU代码合并到一个`.v`文件，文件名为`ysyx_学号后六位.v`，如`ysyx_040228.v`。
    * 在Linux上可通过`cat`命令实现：
    ```sh
    $> cat CPU.v ALU.v regs.v ... > ysyx_040228.v
    ```
* 将CPU顶层命名修改为`ysyx_学号后六位.v`，如`ysyx_040228`。
* 按照[CPU端口命名规范](./stand/interface.md)修改CPU顶层端口名。
* 为CPU内的所有模块名添加前缀`ysyx_学号后六位_`。
    * 如`module ALU`修改为`module ysyx_040228_ALU`。
    * Chisel福利：我们提供一个[firrtl transform](./utils/AddModulePrefix.scala)来自动添加模块名前缀，使用方法参考[相关说明文档](./utils/README.md)。
* 对于手动开发的Verilog代码，目前无法进行模块名前缀的自动添加，请手动进行添加。
* **将改好的`ysyx_040228.v`放到的soc/目录下**。
* 将`main.py`中的`stud_id`**设置为学号的后六位**，比如学号为22040228的同学，设置`stud_id='040228'`。

## 命名规范检查(北京时间 2022/10/07 23:59:59前完成)
运行脚本执行命名规范检查，该脚本会检测设计的.v文件是否符合命名规范，并会生成日志文件`check.log`。可执行的测试环境为`Debian10`、`Ubuntu 20.04`、 `WSL2-Ubuntu 20.04`和`Windows10`。
* 在当前目录下运行`./main.py -s`。
* 最后可以在终端看到检测结果，如果检查通过，则会在终端打印出：
    ```sh
    $> Your core is fine in module name and signal interface
    ```
* 同时，在该目录下会生成日志文件`check.log`。如果检测未通过，则会给出错误信息，并提示是`module name`错误还是`signal interface`错误。也可以打开⽬录下⽣成的log日志⽂件查看报错原因和提示。
* 注意：处理器核的端口定义要严格按照[CPU端口命名规范](./stand/interface.md)来修改，不能在端口中有多余的注释、`wire`和其他的自定义信号，否则可能会导致后续的任务出现问题。

## CPU内部修改(北京时间 2022/10/07 23:59:59前完成)
* 所有触发器都需要带复位端，使其复位后带初值。
    * **Chisel福利：可以通过以下命令对编译生成的`.fir`文件进行扫描，找出不带复位端的寄存器：**
    ```sh
    $> grep -rn "^ *reg " xxx.fir | grep -v "reset =>"
    ```
    其中`xxx.fir`的文件名与顶层模块名相关，通常位于`build/`目录下。若上述命令无输出，说明所有寄存器已经带上复位端。
* 另外Cache还需要：
    * 确认ICache和DCache的data array的大小均不大于4KB，总和不大于8KB。
    * 确认ICache和DCache的data array均采用单口RAM。
    * 对data array进行端口替换：我们提供端口与流片用RAM一致的简化行为模型，采用的是[带写掩码的单口RAM模型](./ram/S011HD1P_X32Y2D128_BW.v)。`ysyxSoCFull.v`已经在顶层集成了8个带写掩码的RAM，每个RAM的规格是128X64bits，共8KB。**同学们需要将Cache替换成访问核顶层的SRAM端口**。
    * 注意：实现大于8KB Cache的核要进行删减，小于8KB的仍要保留端口。实现小于8KB的核将核顶层不用的SRAM端口置常数(输入悬空，输出置`0`)。需要使用的SRAM中若不用写掩码，则需置`1`。另外tag array无需替换，**而且不允许在核内自行例化SRAM进行其他设计**，同时需要同学们自行维护程序加载时的Cache的一致性。具体RAM的端口定义请见[这里](./ram/README.md)。
* 若采用Verilog开发，则需要：
    * 确认代码中的锁存器(Latch)已经去除。
        * **Chisel福利：Chisel不会生成锁存器**
    * 确认代码中的异步复位触发器已经去除，或已经实现同步撤离。
        * **Chisel福利：Chisel默认生成同步复位触发器**
* 对于除了SRAM口之外，其他不使用的核顶层端口(io_interrupt和AXI4 slave)，需要将输出端口置`0`，输入端口悬空。


## 代码规范检查(北京时间 2022/10/07 23:59:59前完成)
对代码进行规范检查，并清除报告中的Warning。具体步骤如下：
* 运行`./main.py -l`，Verilator将会报告除`DECLFILENAME`和`UNUSED`之外所有类别的Warning，你需要修改代码来清理它们。Warning的含义可以参考[Verilator手册的说明](https://veripool.org/guide/latest/warnings.html#list-of-warnings)。
* 运行`./main.py -lu`，Verilator将会额外报告`UNUSED`类别的Warning，你需要修改代码来尽最大可能清理它们。
* 若某些`UNUSED`类别的Warning无法清理，需要填写`./lint`目录中的[warning.md](./lint/warning.md)并给出原因，用于向SoC团队和后端设计团队提供参考。其中[warning.md](./lint/warning.md)中已经给出了格式范例，同学们填写时可以删除。

## Verilator仿真(北京时间 2022/10/07 23:59:59前完成)
对代码使用verilator进行集成测试与仿真，其中SoC的地址空间分配如下：
| 设备 | 地址空间 |
| --- | --- |
| Reserve           | `0x0000_0000~0x01ff_ffff`|
| CLINT             | `0x0200_0000~0x0200_ffff`|
| Reserve           | `0x0201_0000~0x0fff_ffff`|
| UART16550         | `0x1000_0000~0x1000_0fff`|
| SPI               | `0x1000_1000~0x1000_1fff`|
| VGA               | `0x1000_2000~0x1000_2fff`|
| PS2               | `0x1000_3000~0x1000_3fff`|
| Reserve           | `0x1000_4000~0x2fff_ffff`|
| SPI-flash XIP Mode| `0x3000_0000~0x3fff_ffff`|
| ChipLink MMIO     | `0x4000_0000~0x7fff_ffff`|
| MEM               | `0x8000_0000~0xfbff_ffff`|
| SDRAM             | `0xfc00_0000~0xffff_ffff`|

其中:
* 处理器的复位PC需设置为`0x3000_0000`，第一条指令从flash中取出。
* CLINT模块位于处理器核内部，SoC不提供，需要同学们自行实现。
* 接入外部中断需要同学们**自行设计仲裁逻辑**。(核的top层已经预留有io_interrupt接口， 该口会从SoC引出并通过ChipLink接入到FPGA中。同学们需要自行在FPGA上实现PLIC。核在接收到中断会jump到异常处理程序，之后通过读ChipLink MMIO的相关寄存器来查看中断源信息并响应。异常处理完后可以通过写ChipLink MMIO的相关寄存器来清除中断源，外部中断功能是可选实现的，但不实现的话仍需保留io_interrupt接口)。
* 接入同学们自行设计的设备需要核内实现并将设备寄存器分配到**Reserve地址范围内**。
* 注意：地址空间中内没有设置与SoC时钟和管脚相关的功能寄存器，**即不支持通过软件访问某个确定地址来设置SoC相关参数**。

### Verilator仿真要求如下：
* 使用Verilator将自己核和ysyxSoCFull.v正确编译。
* 确认清除Warning后的代码可以成功启动hello、memtest和rtthread。
* 通过快速模式(跳过SPI传输，不可综合，适合快速调试和迭代)对flash进行模拟，运行并通过本框架提供的测试程序。为了打开flash的快速模式，你需要在`./perip/spi/rtl/spi.v`的开头定义宏`FAST_FLASH`：
  ```verilog
  // define this macro to enable fast behavior simulation
  // for flash by skipping SPI transfers
  `define FAST_FLASH
  ```
  * 直接在flash上运行的程序(位于`./prog/bin/flash`目录下)：
    * hello-flash.bin
    * memtest-flash.bin
    * rtthread-flash.bin
  * 通过loader把程序加载到memory，然后跳转运行(位于`./prog/bin/loader`目录下)。**注意需要额外实现`fence.i`指令**
    * hello-loader.bin
    * memtest-loader.bin
    * rtthread-loader.bin
* 通过正常模式(不跳过SPI传输，仿真速度慢，用于最终的系统测试)对flash进行模拟，重新运行上述测试程序。你需要在`./perip/spi/rtl/spi.v`的开头取消对宏`FAST_FLASH`的定义：
  ```verilog
  // define this macro to enable fast behavior simulation
  // for flash by skipping SPI transfers
  // `define FAST_FLASH
  ```
    * 然后再重新运行上面提到的3个flash测试程序和3个loader测试程序：
        * hello-flash.bin
        * memtest-flash.bin
        * rtthread-flash.bin
        * hello-loader.bin
        * memtest-loader.bin
        * rtthread-loader.bin

### Verilator仿真具体步骤如下：
为了方便同学们进行Verilator测试，我们**已经在`main.py`中实现了Verilator编译、仿真测试和回归测试功能**。同学们只需要：

* 运行`./main.py -c`可以编译生成flash正常模式下的仿真可执行文件`simv`，运行`./main.py -fc`可以编译生成flash快速模式下的仿真可执行文件`simv`。
* 在生成`simv`之后，使用：
    ```sh
    $> ./main.py -t APP_TYPE APP_NAME
    ```
    来对某个特定测试程序进行仿真，其中`APP_TYPE`可选值为`flash`和`loader`，分别表示flash和memory加载两种启动方式。`APP_NAME`的可选值有`hello`、`memtest`和`rtthread`。比如运行`./main.py -t flash hello`可以仿真flash模式下的hello测试程序。
* 运行`./main.py -r`和`./main.py -fr`就可以依次运行flash正常模式与快速模式下的回归测试。在测试过程中我们对于每个测试都设置了**预设运行时间**，当程序超过**预设运行时间**则会自行停止运行，同学们可以修改`./main.py`中的：
    ```python
    app = [('hello', 20), ('memtest', 50), ('rtthread', 350)]
    ```
    数字部分以适应自己核的运行，其中数字表示**预设运行时间**，单位为秒。想要仿真不停止可以通过设置一个较大的数字来实现，数字至少是int32类型的。另外为了保证测试时代码总是最新的，**回归测试时会对代码进行重新编译，编译之后再测试**。
> 注意：若为了正确运行测试程序而对处理器核进行了修改，**需要重新按照上述流程从头开始依次执行并通过**。

## 提交代码(北京时间 2022/10/07 23:59:59前完成)
>注意：此处提交是为了尽快运行综合流程并发现新问题，此后可以继续调试处理器的实现。

在接入ysyxSoC本框架并完成上述所有测试后，可以开始代码提交流程。提交前请确保所有触发器可复位。具体需要准备的工作如下：
* 将difftest成功运行指令集测试的截图文件`reg-test.png`放置于`./submit`目录下，截图不必包含所有的结果输出，但是必须包含使用date命令输出的当前时间，例如：
* 将成功运行本框架的flash正常模式`rtthread-loader.bin`的截图文件`rtthread-loader.png`放置于`./submit`目录下。
* 填写`./submit`目录下的cache规格文档[cache_spec.md](./submit/cache_spec.md)。
* 确认已经根据代码规范检查并在`./lint`目录下填写完[warning.md](./lint/warning.md)。
* 制作一份带数据流向的处理器架构图，并对图中各模块做简单说明，整理成ysyx_xxxxxx.pdf文件并放置于`./submit`目录下。
* 创建自己的gitee开源仓库，确认仓库的默认主分支是`master`。
* 确认仓库通过ssh的方式clone到`./submit`目录下并填写完成git的`user.email`和`user.name`。然后运行`./main.py -su`，该脚本会先检查上述提交文件是否齐全，齐全的话会将文件拷贝到本地clone的仓库中，并推送到远端gitee仓库。注意不要在`./submit`中添加额外的文件夹，因为提交脚本是通过`os.path.isdir()`来自动确定本地clone的仓库名字的，如果`./submit`中存在多个文件夹，则程序无法分辨哪个是本地clone的仓库了。
* 将自己仓库的`HTTPS格式的URL`和`ysyx_学号后六位`发送给组内助教以完成第一次代码提交。后续提交只需要重新运行`./main.py -su`命令即可。

> 注意：后续提交不可修改cache规格，只能根据report反馈修复bug。SoC和后端团队将定期检查新提交的代码，进行综合和仿真测试，并将结果以日志报告的形式上传至ysyx_submit仓库的**ysyx4分支**，具体说明请参考[ysyx_submit仓库的说明文档](https://github.com/OSCPU/ysyx_submit/blob/ysyx4/README.md)。


## 协助SoC团队在流片仿真环境中启动RT-Thread(北京时间 2022/11/07 23:59:59前完成)
提交代码后，请及时关注SoC团队的反馈。

> 注意：**本项目中的SoC只用于在Verilator中验证，不参与流片环节!** 此外本项目与流片SoC仿真环境仍然有少数不同，在本项目中通过测试并**不代表也能通过流片SoC仿真环境的测试**，在流片SoC仿真环境中的运行结果，以SoC团队的反馈为准，因此请大家务必重视SoC团队的反馈。

具体来说，相较于基于流片SoC仿真环境的仿真，基于Verilator的仿真:
* 没有不定态(x态)信号传播的问题
* 没有跨时钟域和异步桥
* 没有PLL
* 没有真实器件的延时信息

> **注意：以下内容不是同学们必须要完成的任务，而是给那些对我们提供的SoC仿真框架有定制化需求的同学们提供的。下面分别介绍了生成自己的verilator仿真程序所需要的必要步骤和使用chisel生成ysyxSoCFull框架的过程和注意要点**。

## ysyxSoCFull定制集成步骤
* 将`ysyxSoC/ysyx/perip`目录及其子目录下的所有`.v`文件加入verilator的Verilog文件列表。
* 将`ysyxSoC/ysyx/soc/ysyxSoCFull.v`文件加入verilator的Verilog文件列表。
* 将处理器Verilog文件加入verilator的Verilog文件列表。
* 将`ysyxSoC/ysyx/perip/uart16550/rtl`和`ysyxSoC/ysyx/perip/spi/rtl`两个目录加入包含路径中(使用verilator的`-I`选项)。
* 将`ysyxSoC/ysyx/perip/spiFlash/spiFlash.cpp`文件加入verilator的C++文件列表。
* 将处理器的复位PC设置为`0x3000_0000`。
* 在verilator编译选项中添加`--timescale "1ns/1ns"`。
* 在verilator初始化时对flash进行初始化，有以下两种方式:
   * 调用`spiFlash.cpp`中的`flash_init(img)`函数，用于将bin文件中的指令序列放置在flash中，其中参数`img`是bin文件的路径，在`ysyxSoC/ysyx/prog/bin/flash`和`ysyxSoC/ysyx/prog/bin/loader`目录下提供了一些示例。
   * 调用`spiFlash.cpp`中的`flash_memcpy(src, len)`函数，用于将已经读入内存的指令序列放置在flash中，其中参数`src`是指令序列的地址，`len`是指令序列的长度。
* 将`ysyxSoCFull`模块(在`ysyxSoC/ysyx/soc/ysyxSoCFull.v`中定义)设置为verilator仿真的顶层。
* 将`ysyxSoC/ysyx/soc/ysyxSoCFull.v`中的`ysyx_000000`模块名修改为自己的处理器模块名。
* 通过Verilator进行仿真即可。


## 自己编译并生成ysyxSoCFull.v步骤
* 更新并拉取当前仓库的子模块：
    ```sh
    $> git submodule update --init
    ```
* 指定RISCV环境变量为工具链的安装目录，为`riscv64-unknown-elf-xxx`开发版的根目录：
    ```sh
    $> export RISCV=/path/to/riscv/toolchain/installation
    ```
* 进入到`./ysyx`目录下执行`./main.py -y`编译SoC框架，框架的源码结构如下所示：
    ```sh
    ysyxSoC/src/main/scala/ysyx
    ├── chiplink
    │   └── ...                        # ChipLink的实现
    └── ysyx
        ├── AXI4ToAPB.scala            # AXI4-APB的转接桥，不支持burst，且只支持4字节以下的访问
        ├── ChipLinkBridge.scala       # ChipLink-AXI4的转接桥
        ├── CPU.scala                  # CPU wrapper，将会按照SoC端口规范实例化一个CPU实例
        ├── SoC.scala                  # SoC顶层
        ├── SPI.scala                  # SPI wrapper，将会实例化verilog版本的SPI控制器
        └── Uart16550.scala            # UART16550 wrapper，将会实例化verilog版本的UART16550控制器
    ```
> 注意：编译时需要使用Java 11，高版本的Java会抛出异常，具体见：https://github.com/chipsalliance/rocket-chip/issues/2789