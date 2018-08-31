拷贝一份config.h到安装的对应的protobuf目录下例如
：/usr/local/include/google/protobuf， 
然后再stubs.h顶上加上一行包含头文件代码
#include <google/protobuf/config.h>